#include "stdafx.h"
#include "DumpState.h"
#include "BinaryDumper.h"

std::map<int,CDumpState::LoadedData> CDumpState::LoadedDataCache;
UINT CDumpState::m_cacheSize = 0;
int CDumpState::m_newFileFormatFlag = 0;

CDumpState::CDumpState()
{
	mainTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mainTextureWidth = 1024;
	mainTextureHeight = 768;

	m_generatorMode = Dx10;

	m_sClearAll = "const float black[4] = { 0,0,0,0 };\n";

	m_bAddSetEidCommands = TRUE;
}

void *CDumpState::LoadFromBinaryFile(int pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	_ASSERT(pos >= 0);

	if (LoadedDataCache.find(pos) != LoadedDataCache.end())
	{
		LoadedData ld = LoadedDataCache[pos];
		if (pLen) *pLen = ld.len;
		if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
		if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;
		return ld.data;
	}

	using namespace std;

	ifstream myFile;
	myFile.open("data.bin", ios::in | ios::binary);

	if (!m_newFileFormatFlag)
	{
		DWORD null;
		myFile.read((char *)&null,sizeof(null));
		if (null == 0)
		{
			char str[4];
			myFile.read(str,4);
			if (memcmp(str,"iZ3D",4) != 0) __debugbreak();
			myFile.read(str,4);
			if (str[0] != 0 || str[1] != 0 || str[2] != 0 || str[3] != 2) __debugbreak();
			m_newFileFormatFlag = 1;
		}
		else
		{
			m_newFileFormatFlag = -1;
		}
	}

	myFile.seekg(pos,ios::beg);
	LoadedData ld;
	if (m_newFileFormatFlag == 1)
	{
		DWORD count;
		myFile.read((char *)&count,sizeof(count));
		DWORD *params = new DWORD[count];
		myFile.read((char *)params,sizeof(DWORD) * count);

		ld.count = count;
		ld.params = params;
	}
	else
	{
		ld.count = 0;
		ld.params = 0;
	}

	DWORD len;
	myFile.read((char *)&len,sizeof(len));
	ld.len = len;

	char *data = new char[len];
	myFile.read(data,len);
	ld.data = data;

	myFile.close();

	ld.size = sizeof(ld) + sizeof(*ld.params) * ld.count + ld.len;
	m_cacheSize += ld.size;
	LoadedDataCache[pos] = ld;

	DEBUG_TRACE1(L"LoadedDataCacheSize = %d\n",m_cacheSize);

	if (pLen) *pLen = ld.len;
	if (pitch1) *pitch1 = ld.count > 0 ? ld.params[0] : 0;
	if (pitch2) *pitch2 = ld.count > 1 ? ld.params[1] : 0;
	return ld.data;
}

UINT64 CDumpState::ReadIndexFile(int index)
{
	_ASSERT(index > 0);

	using namespace std;

	if (!m_IndexFile.is_open())
		m_IndexFile.open("index.bin", ios::in | ios::binary);

	UINT64 off = (index - 1) * sizeof(UINT64);
	m_IndexFile.seekg(off,ios::beg);
	if (m_IndexFile.fail()) __debugbreak();

	UINT res;
	m_IndexFile.read((char *)&res,sizeof(res));
	if (m_IndexFile.fail()) __debugbreak();

	return res;
}


void *CDumpState::LoadFromBinaryFileEx(int pos,DWORD *pLen,DWORD *pitch1,DWORD *pitch2)
{
	UINT64 pos64 = ReadIndexFile(pos);
	return LoadFromBinaryFile((int)pos64,pLen,pitch1,pitch2);
}

void CDumpState::SetUsedAsRenderTarget(D3D10DDI_HRESOURCE hres,BOOL bSet)
{
	if (bSet)
		wasUsedAsRenderTarget.insert(hres.pDrvPrivate);
	else
		wasUsedAsRenderTarget.erase(hres.pDrvPrivate);
}

BOOL CDumpState::IsUsedAsRenderTarget(D3D10DDI_HRESOURCE hres)
{
	return wasUsedAsRenderTarget.find(hres.pDrvPrivate) != wasUsedAsRenderTarget.end();
}

UINT64 CDumpState::ResaveBinaryData(UINT64 pos,BOOL bPitchRequired,BOOL bAlign)
{
#ifndef FINAL_RELEASE
	DWORD len,pitch1,pitch2;
	void *data = LoadFromBinaryFile((int)pos,&len,&pitch1,&pitch2);
	
	//if (bPitchRequired && (!pitch1 || !pitch2)) __debugbreak();

	UINT64 res = CBinaryDumper::GetInstance().SaveToBinaryFile2(data,len,pitch1,pitch2,bAlign);
	return res;
#else
	__debugbreak();
	return 0;
#endif
}

UINT64 CDumpState::ResaveBinaryDataEx(int index, BOOL bAlign)
{
	UINT64 pos = ReadIndexFile(index);
	return ResaveBinaryData(pos,TRUE,bAlign);
}

void CDumpState::SetShadersOfLayout(Commands::CreateElementLayout *layout,Commands::CreateVertexShader *shader)
{
	//if(!layout) __debugbreak();
	shaderLayoutMap[layout].insert(shader);
}

std::set<Commands::CreateVertexShader *> CDumpState::GetShadersOfLayout(Commands::CreateElementLayout *layout)
{
	if(!layout) __debugbreak();
	//if (shaderLayoutMap.find(layout) == shaderLayoutMap.end()) std::set<Commands::CreateVertexShader *>();
	return shaderLayoutMap[layout];
}

D3D10DDIARG_CREATERESOURCE *CDumpState::GetCreateResourceStruct(D3D10DDI_HRESOURCE hres)
{
	if(resourcesMap.find(hres.pDrvPrivate) == resourcesMap.end()) return 0;
	return &resourcesMap[hres.pDrvPrivate];
}

template<>
void CDumpState::SaveCreateResourceStruct<D3D11DDIARG_CREATERESOURCE>(D3D10DDI_HRESOURCE hres,D3D11DDIARG_CREATERESOURCE &cres)
{
	// one additional field on the structure's end
	resourcesMap[hres.pDrvPrivate] = *(D3D10DDIARG_CREATERESOURCE *)&cres;
}

void CDumpState::AddClearTarget(UINT eventId,UINT cmdId)
{
	char buf[128];
	if (cmdId == idCreateRenderTargetView)
	{
		sprintf(buf,"ctx->ClearRenderTargetView(rtview_%d,black);\n",eventId);
		m_sClearAll += buf;
	}
	else if (cmdId == idCreateDepthStencilView || cmdId == idCreateDepthStencilView11_0)
	{
		sprintf(buf,"ctx->ClearDepthStencilView(sview_%d,7,1,0);\n",eventId);
		m_sClearAll += buf;
	}
}

std::string CDumpState::GetClearAllString()
{
	return m_sClearAll;
}





char *CDumpState::ToViewDim1(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres)
{
	D3D10DDIARG_CREATERESOURCE &cres = *GetCreateResourceStruct(hres);

	switch (type)
	{
	case D3D10DDIRESOURCE_BUFFER: 
		return "D3D10_1_SRV_DIMENSION_BUFFER";
	case D3D10DDIRESOURCE_TEXTURE1D: 
		return cres.ArraySize > 1 ? "D3D10_1_SRV_DIMENSION_TEXTURE1DARRAY" : "D3D10_1_SRV_DIMENSION_TEXTURE1D";
	case D3D10DDIRESOURCE_TEXTURE2D: 
		if (cres.SampleDesc.Count > 1)
			return cres.ArraySize > 1 ? "D3D10_1_SRV_DIMENSION_TEXTURE2DMSARRAY" : "D3D10_1_SRV_DIMENSION_TEXTURE2DMS";
		else
			return cres.ArraySize > 1 ? "D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY" : "D3D10_1_SRV_DIMENSION_TEXTURE2D";
	case D3D10DDIRESOURCE_TEXTURE3D: 
		return "D3D10_1_SRV_DIMENSION_TEXTURE3D";
	case D3D10DDIRESOURCE_TEXTURECUBE: 
		return "D3D10_1_SRV_DIMENSION_TEXTURECUBE";
	}
	__debugbreak();

	return "(unknown)";
}

char *CDumpState::ToViewDim(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres)
{
	D3D10DDIARG_CREATERESOURCE &cres = *GetCreateResourceStruct(hres);

	switch (type)
	{
	case D3D10DDIRESOURCE_BUFFER: 
		return "D3D10_SRV_DIMENSION_BUFFER";
	case D3D10DDIRESOURCE_TEXTURE1D: 
		return cres.ArraySize > 1 ? "D3D10_SRV_DIMENSION_TEXTURE1DARRAY" : "D3D10_SRV_DIMENSION_TEXTURE1D";
	case D3D10DDIRESOURCE_TEXTURE2D: 
		if (cres.SampleDesc.Count > 1)
			return cres.ArraySize > 1 ? "D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY" : "D3D10_SRV_DIMENSION_TEXTURE2DMS";
		else
			return cres.ArraySize > 1 ? "D3D10_SRV_DIMENSION_TEXTURE2DARRAY" : "D3D10_SRV_DIMENSION_TEXTURE2D";
	case D3D10DDIRESOURCE_TEXTURE3D: 
		return "D3D10_SRV_DIMENSION_TEXTURE3D";
	case D3D10DDIRESOURCE_TEXTURECUBE: 
		return "D3D10_SRV_DIMENSION_TEXTURECUBE";
	}
	__debugbreak();

	return "(unknown)";
}

char *CDumpState::ToViewDimDsv(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres)
{
	D3D10DDIARG_CREATERESOURCE &cres = *GetCreateResourceStruct(hres);

	switch (type)
	{
	case D3D10DDIRESOURCE_BUFFER: 
	case D3D10DDIRESOURCE_TEXTURE1D: 
		return cres.ArraySize > 1 ? "D3D10_DSV_DIMENSION_TEXTURE1DARRAY" : "D3D10_DSV_DIMENSION_TEXTURE1D";
	case D3D10DDIRESOURCE_TEXTURE2D: 
		if (cres.SampleDesc.Count > 1)
			return cres.ArraySize > 1 ? "D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY" : "D3D10_DSV_DIMENSION_TEXTURE2DMS";
		else
			return cres.ArraySize > 1 ? "D3D10_DSV_DIMENSION_TEXTURE2DARRAY" : "D3D10_DSV_DIMENSION_TEXTURE2D";
	case D3D10DDIRESOURCE_TEXTURE3D: 
	case D3D10DDIRESOURCE_TEXTURECUBE: 
		return cres.SampleDesc.Count > 1 ? "D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY" : "D3D10_DSV_DIMENSION_TEXTURE2DARRAY";
	}
	__debugbreak();
	return "(unknown)";
}

char *CDumpState::ToViewDimRtv(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres)
{
	D3D10DDIARG_CREATERESOURCE &cres = *GetCreateResourceStruct(hres);

	switch (type)
	{
	case D3D10DDIRESOURCE_BUFFER: 
		return "D3D10_RTV_DIMENSION_BUFFER";
	case D3D10DDIRESOURCE_TEXTURE1D: 
		return cres.ArraySize > 1 ? "D3D10_RTV_DIMENSION_TEXTURE1DARRAY" : "D3D10_RTV_DIMENSION_TEXTURE1D";
	case D3D10DDIRESOURCE_TEXTURE2D: 
		if (cres.SampleDesc.Count > 1)
			return cres.ArraySize > 1 ? "D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY" : "D3D10_RTV_DIMENSION_TEXTURE2DMS";
		else
			return cres.ArraySize > 1 ? "D3D10_RTV_DIMENSION_TEXTURE2DARRAY" : "D3D10_RTV_DIMENSION_TEXTURE2D";
	case D3D10DDIRESOURCE_TEXTURE3D: 
		return "D3D10_RTV_DIMENSION_TEXTURE3D";
	case D3D10DDIRESOURCE_TEXTURECUBE: 
		return "D3D10_RTV_DIMENSION_TEXTURE2DARRAY";
	}
	__debugbreak();

	return "(unknown)";
}

char *CDumpState::ToViewDimUav(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres)
{
	D3D10DDIARG_CREATERESOURCE &cres = *GetCreateResourceStruct(hres);

	switch (type)
	{
	case D3D10DDIRESOURCE_BUFFER: 
	case D3D11DDIRESOURCE_BUFFEREX: 
		return "D3D11_UAV_DIMENSION_BUFFER";
	case D3D10DDIRESOURCE_TEXTURE1D: 
		return cres.ArraySize > 1 ? "D3D11_UAV_DIMENSION_TEXTURE1DARRAY" : "D3D11_UAV_DIMENSION_TEXTURE1D";
	case D3D10DDIRESOURCE_TEXTURE2D: 
		return cres.ArraySize > 1 ? "D3D11_UAV_DIMENSION_TEXTURE2DARRAY" : "D3D11_UAV_DIMENSION_TEXTURE2D";
	case D3D10DDIRESOURCE_TEXTURE3D: 
		return "D3D11_UAV_DIMENSION_TEXTURE3D";
	}
	__debugbreak();

	return "D3D11_UAV_DIMENSION_UNKNOWN";
}

