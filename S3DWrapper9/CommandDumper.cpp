#include "StdAfx.h"
#include "CommandDumper.h"
#include <fstream>

#ifndef FINAL_RELEASE

using namespace LlamaXML;

CCommandDumper CCommandDumper::m_instance;
LONG CInsideDriverHelper::m_counter;

void CArgumentWrappers::SetNames(const char *names)
{
	unsigned count = (unsigned)m_args.size();
	//m_names.resize(count);

	char *p = new char[strlen(names) + 1];
	strcpy(p,names);
	const char *sDelimiter = ",";

	char *s = strtok(p,sDelimiter);
	for(unsigned i = 0;i < count;i++)
	{
		/*DEBUG_TRACE1(L"*0* %S\n",s);
		DEBUG_TRACE1(L"*1* %S\n",std::string(s).c_str());
		m_names.push_back(std::string(s));
		DEBUG_TRACE1(L"*2* %d from %d = %S\n",i,m_names.size(),m_names[i].c_str());*/
		m_args[i]->SetName(s);
		s = strtok(0,sDelimiter);
	}

	delete[]p;
}

void CArgumentWrappers::Store(CCommandDumper &dumper)
{
	for(unsigned i = 0;i < m_args.size();i++)
	{
		//DEBUG_TRACE1(L"*3* %S\n",m_names[i].c_str());
		//dumper.DumpCommandArgument(m_names[i].c_str(),m_args[i]->Value());
		m_args[i]->Dump(dumper);
	}
}

CCommandDumper::CCommandDumper(void)
{
	m_pOutputStream = 0;
	m_pXMLWriter = 0;

	m_EventId = 1;

	InitializeCriticalSection(&m_cs);
}

CCommandDumper::~CCommandDumper(void)
{
	if (m_pXMLWriter)
	{
		m_pXMLWriter->EndDocument();
		delete m_pXMLWriter;
		m_pXMLWriter = 0;
	}

	if (m_pOutputStream)
	{
		delete m_pOutputStream;
		m_pOutputStream = 0;
	}

	DeleteCriticalSection(&m_cs);
}

bool CCommandDumper::IsOK()
{
	return m_pXMLWriter != NULL;
}

void CCommandDumper::Init()
{
	if (!GINFO_DUMP_ON)
		return;
	try
	{
		wchar_t	fileName[MAX_PATH];
		swprintf_s(fileName, MAX_PATH, L"%s\\CommandFlow.xml", gData.DumpDirectory);

		m_pOutputStream = new FileOutputStream(fileName);
		m_pXMLWriter = new XMLWriter(*m_pOutputStream,TextEncoding::System());
		m_pXMLWriter->StartDocument("1.0",0,0);

		m_pXMLWriter->StartElement("DX9CommandFlow");
		m_pXMLWriter->WriteAttribute("appName",std::wstring(gInfo.ApplicationName));
	}
	catch ( LlamaXML::XMLException /*ex*/ )
	{
	}
}

BOOL CCommandDumper::CanDump()
{
	return IsOK() && (!CInsideDriverHelper::isInside() || m_bForce);
}

BOOL CCommandDumper::BeginCommand(const TCHAR *cmdName)
{
	if (!CanDump()) return FALSE;

	EnterCriticalSection(&m_cs);

	m_pXMLWriter->StartElement("COMMAND");
	m_pXMLWriter->WriteAttribute("name",std::wstring(cmdName));
	m_pXMLWriter->WriteAttribute("eid",m_EventId);

	m_EventId++;
	
	return TRUE;
}

void CCommandDumper::EndCommand()
{
	m_pXMLWriter->EndElement();

	LeaveCriticalSection(&m_cs);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL m_bDataBinCreated = 0;
std::map<std::vector<DWORD>,UINT64> m_storedDataCache;
UINT64 SaveToBinaryFile(const void *data,int len)
{
	std::vector<DWORD> key;
	key.push_back(CalculateCRC32CFast(data,len));
	key.push_back(len);
	//for(unsigned i = 0;i < count;i++) key.push_back(params[i]);

	auto &it = m_storedDataCache.find(key);
	if (it != m_storedDataCache.end()) return it->second;

	using namespace std;
	ofstream myFile;

	char fname[MAX_PATH];
	sprintf_s(fname, MAX_PATH,"%S\\data.bin", gData.DumpDirectory);

	for(int i = 0;;i++)
	{
		if (i > 100) __debugbreak();

		myFile.open(fname, ios::out | ios::binary | (m_bDataBinCreated ? ios::app : ios::trunc));
		if (!myFile.is_open())
		{
			DEBUG_TRACE1(L"SaveToIndexFile: file open failed, errno = %d\n",errno);
			::Sleep(100);
			continue;
		}

		myFile.seekp(0,m_bDataBinCreated ? ios::end : ios::beg);
		if (myFile.fail())
		{
			DEBUG_TRACE1(L"SaveToIndexFile: seekp failed, errno = %d\n",errno);

			myFile.close();
			::Sleep(100);
			continue;
		}

		// ok
		break;
	}

	if (!m_bDataBinCreated)
	{
		const char buf[] = { 0,0,0,0,'i','Z','3','D',0,0,0,2 };
		myFile.write(buf,sizeof(buf));
	}

	UINT64 res = (UINT)myFile.tellp();
	if ((LONG64)res < 0)
	{
		DEBUG_TRACE1(L"tellp return value %d < 0, errno = %d\n",(int)res,errno);
		__debugbreak();

		res = (UINT)myFile.tellp();
		DEBUG_TRACE1(L"second try: %d, %d\n",(int)res,errno);
	}

	DWORD count = 0;
	myFile.write((const char *)&count,sizeof(count));
	/*for(unsigned i = 0;i < count;i++)
	{
		myFile.write((const char *)(params + i),sizeof(DWORD));
	}*/

	myFile.write((const char *)&len,sizeof(DWORD));
	myFile.write((const char *)data,len);

	myFile.close();
	m_bDataBinCreated = TRUE;

	m_storedDataCache[key] = res;

	return res;
}

UINT GetSizeOfBuffer(IDirect3DVertexBuffer9 *buf)
{
	D3DVERTEXBUFFER_DESC desc;
	buf->GetDesc(&desc); // <-- Caching is required
	return desc.Size;
}

UINT GetSizeOfBuffer(IDirect3DIndexBuffer9 *buf)
{
	D3DINDEXBUFFER_DESC desc;
	buf->GetDesc(&desc); // <-- Caching is required
	return desc.Size;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <>
void CCommandDumper::WriteArgumentDataCustom<IDirect3DVertexBuffer9 *>(IDirect3DVertexBuffer9 *const &value)
{
	if (!value) return;

	UINT size = GetSizeOfBuffer(value);
	void *pData;
	HRESULT hr = value->Lock(0,size,&pData,D3DLOCK_READONLY);
	if (FAILED(hr))
	{
		DEBUG_TRACE1(_T("Cannot lock vertex buffer\n"));
		return;
	}

	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(pData,size));

	value->Unlock();
}

template <>
void CCommandDumper::WriteArgumentDataCustom<IDirect3DIndexBuffer9 *>(IDirect3DIndexBuffer9 *const &value)
{
	if (!value) return;

	UINT size = GetSizeOfBuffer(value);
	void *pData;
	HRESULT hr = value->Lock(0,size,&pData,D3DLOCK_READONLY);
	if (FAILED(hr))
	{
		DEBUG_TRACE1(_T("Cannot lock index buffer\n"));
		return;
	}

	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(pData,size));

	value->Unlock();
}

template <>
void CCommandDumper::WriteArgumentDataCustom<CONST D3DMATRIX *>(CONST D3DMATRIX *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("D3DMATRIX");
	for(int i = 0;i < 4;i++)
	{
		for(int j = 0;j < 4;j++)
		{
			char buf[10];
			sprintf(buf,"item_%d%d",i,j);

			DumpCommandArgument(buf,value->m[i][j]);
		}
	}
	m_pXMLWriter->EndElement();
}

template <>
void CCommandDumper::WriteArgumentDataCustom<CONST D3DMATERIAL9 *>(CONST D3DMATERIAL9 *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("D3DMATERIAL9");
	DumpCommandArgument("Diffuse",value->Diffuse);
	DumpCommandArgument("Ambient",value->Ambient);
	DumpCommandArgument("Specular",value->Specular);
	DumpCommandArgument("Emissive",value->Emissive);
	DumpCommandArgument("Power",value->Power);
	m_pXMLWriter->EndElement();
}

template <>
void CCommandDumper::WriteArgumentData<D3DCOLORVALUE>(D3DCOLORVALUE const &value)
{
	m_pXMLWriter->StartElement("D3DCOLORVALUE");
	DumpCommandArgument("r",value.r);
	DumpCommandArgument("g",value.g);
	DumpCommandArgument("b",value.b);
	DumpCommandArgument("a",value.a);
	m_pXMLWriter->EndElement();
}

template <>
void CCommandDumper::WriteArgumentDataCustom<CONST D3DLIGHT9 *>(CONST D3DLIGHT9 *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("D3DLIGHT9");
	DumpCommandArgument("Type",value->Type);
	DumpCommandArgument("Diffuse",value->Diffuse);
	DumpCommandArgument("Specular",value->Specular);
	DumpCommandArgument("Ambient",value->Ambient);
	DumpCommandArgument("Position",value->Position);
	DumpCommandArgument("Direction",value->Direction);
	DumpCommandArgument("Range",value->Range);
	DumpCommandArgument("Falloff",value->Falloff);
	DumpCommandArgument("Attenuation0",value->Attenuation0);
	DumpCommandArgument("Attenuation1",value->Attenuation1);
	DumpCommandArgument("Attenuation2",value->Attenuation2);
	DumpCommandArgument("Theta",value->Theta);
	DumpCommandArgument("Phi",value->Phi);
	m_pXMLWriter->EndElement();
}

template <>
void CCommandDumper::WriteArgumentData<D3DVECTOR>(D3DVECTOR const &value)
{
	m_pXMLWriter->StartElement("D3DVECTOR");
	DumpCommandArgument("x",value.x);
	DumpCommandArgument("y",value.y);
	DumpCommandArgument("z",value.z);
	m_pXMLWriter->EndElement();
}

template <>
void CCommandDumper::WriteArgumentDataCustom<D3DPRESENT_PARAMETERS *>(D3DPRESENT_PARAMETERS *const &value)
{
	m_pXMLWriter->StartElement("D3DPRESENT_PARAMETERS");
	DumpCommandArgument("BackBufferWidth",value->BackBufferWidth);
	DumpCommandArgument("BackBufferHeight",value->BackBufferHeight);
	DumpCommandArgument("BackBufferFormat",value->BackBufferFormat);
	DumpCommandArgument("BackBufferCount",value->BackBufferCount);
	DumpCommandArgument("MultiSampleType",value->MultiSampleType);
	DumpCommandArgument("MultiSampleQuality",value->MultiSampleQuality);
	DumpCommandArgument("SwapEffect",value->SwapEffect);
	DumpCommandArgument("hDeviceWindow",value->hDeviceWindow);
	DumpCommandArgument("Windowed",value->Windowed);
	DumpCommandArgument("EnableAutoDepthStencil",value->EnableAutoDepthStencil);
	DumpCommandArgument("AutoDepthStencilFormat",value->AutoDepthStencilFormat);
	DumpCommandArgument("Flags",value->Flags);
	DumpCommandArgument("FullScreen_RefreshRateInHz",value->FullScreen_RefreshRateInHz);
	DumpCommandArgument("PresentationInterval",value->PresentationInterval);
	m_pXMLWriter->EndElement();
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DVERTEXELEMENT9 *>(CONST D3DVERTEXELEMENT9 *const &value)
{
	if (!value) return;

	const D3DVERTEXELEMENT9 *last = value;
	const D3DVERTEXELEMENT9 end = D3DDECL_END();
	while (memcmp(last,&end,sizeof(end)) != 0) last++;

	UINT size = (UINT)((last + 1 - value) * sizeof(*value));
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(value,size));
}

template <> void CCommandDumper::WriteArgumentData<ShaderFunction>(ShaderFunction const &value)
{
	CONST DWORD *data = value;
	DWORD size = D3DXGetShaderSize(data);//*(data + 6) * sizeof(*data);
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(data,size));
}

template <> void CCommandDumper::WriteArgumentData<ShaderConstantsF>(ShaderConstantsF const &value)
{
	const float *data = value;
	UINT count = value;
	UINT size = count * 4 * sizeof(*data);
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(data,size));
}

template <> void CCommandDumper::WriteArgumentData<ShaderConstantsI>(ShaderConstantsI const &value)
{
	const int *data = value;
	UINT count = value;
	UINT size = count * 4 * sizeof(*data);
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(data,size));
}

template <> void CCommandDumper::WriteArgumentData<JustData>(JustData const &value)
{
	const void *data = value;
	UINT size = value;
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(data,size));
}

template <> void CCommandDumper::WriteArgumentData<ClipPlaneData>(ClipPlaneData const &value)
{
	const float *data = value;
	UINT count = value;
	UINT size = count * sizeof(*data);
	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(data,size));
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST RECT *>(CONST RECT *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("RECT");
	DumpCommandArgument("left",value->left);
	DumpCommandArgument("top",value->top);
	DumpCommandArgument("right",value->right);
	DumpCommandArgument("bottom",value->bottom);
	m_pXMLWriter->EndElement();
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST POINT *>(CONST POINT *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("POINT");
	DumpCommandArgument("x",value->x);
	DumpCommandArgument("y",value->y);
	m_pXMLWriter->EndElement();
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DRECT *>(CONST D3DRECT *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("D3DRECT");
	DumpCommandArgument("x1",value->x1);
	DumpCommandArgument("y1",value->y1);
	DumpCommandArgument("x2",value->x2);
	DumpCommandArgument("y2",value->y2);
	m_pXMLWriter->EndElement();
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DVIEWPORT9 *>(CONST D3DVIEWPORT9 *const &value)
{
	if (!value) return;

	m_pXMLWriter->StartElement("D3DVIEWPORT9");
	DumpCommandArgument("X",value->X);
	DumpCommandArgument("Y",value->Y);
	DumpCommandArgument("Width",value->Width);
	DumpCommandArgument("Height",value->Height);
	DumpCommandArgument("MinZ",value->MinZ);
	DumpCommandArgument("MaxZ",value->MaxZ);
	m_pXMLWriter->EndElement();
}

template <> void CCommandDumper::WriteArgumentDataCustom<CONST D3DGAMMARAMP *>(CONST D3DGAMMARAMP *const &value)
{
	if (!value) return;

	m_pXMLWriter->WriteAttribute("databin_pos",SaveToBinaryFile(value,sizeof(*value)));
}

template <> void CCommandDumper::WriteArgumentData<DatabinPoses>(DatabinPoses const &value)
{
	m_pXMLWriter->StartElement("databin_poses");
	m_pXMLWriter->WriteAttribute("count",(UINT)value);
	for(UINT i = 0;i < value;i++)
	{
		DumpCommandArgument("databin_pos",*((const UINT64 *)value + i));
	}
	m_pXMLWriter->EndElement();
}

#endif // FINAL_RELEASE