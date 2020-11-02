#pragma once

#include <d3d10umddi.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>

namespace Commands
{
	class CreateElementLayout;
	class CreateVertexShader;
}

class CDumpState
{
public:
	enum GeneratorDxMode
	{
		Dx10   = 0,
		Dx10_1 = 1,
		Dx11   = 2
	};

	CDumpState();

	UINT64 ReadIndexFile(int index);
	void *LoadFromBinaryFileEx(int pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0);
	static void *LoadFromBinaryFile(int pos,DWORD *pLen = 0,DWORD *pitch1 = 0,DWORD *pitch2 = 0);

	void SetMainTextureEventId(UINT eid) { mainTextureEid = eid; }
	UINT GetMainTextureEventId() { return mainTextureEid; }

	void SetMainTextureFormat(DXGI_FORMAT format) { mainTextureFormat = format; }
	DXGI_FORMAT GetMainTextureFormat() { return mainTextureFormat; }

	void SetMainTextureWidth(UINT w) { mainTextureWidth = w; }
	UINT GetMainTextureWidth() { return mainTextureWidth; }

	void SetMainTextureHeight(UINT h) { mainTextureHeight = h; }
	UINT GetMainTextureHeight() { return mainTextureHeight; }

	void SetUsedAsRenderTarget(D3D10DDI_HRESOURCE hres,BOOL bSet = TRUE);
	BOOL IsUsedAsRenderTarget(D3D10DDI_HRESOURCE hres);

	UINT64 ResaveBinaryData(UINT64 pos,BOOL bPitchRequired = FALSE, BOOL bAlign = FALSE);
	UINT64 ResaveBinaryDataEx(int index, BOOL bAlign = FALSE);

	char *ToViewDimDsv(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres);
	char *ToViewDimRtv(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres);
	char *ToViewDimUav(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres);
	char *ToViewDim(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres);
	char *ToViewDim1(D3D10DDIRESOURCE_TYPE type,D3D10DDI_HRESOURCE hres);

	template<class DXCREATERESOURCE>
	void SaveCreateResourceStruct(D3D10DDI_HRESOURCE hres,DXCREATERESOURCE &cres);
	D3D10DDIARG_CREATERESOURCE *GetCreateResourceStruct(D3D10DDI_HRESOURCE hres);

	template<class T> UINT GetHandleEventId(T handle)
	{
		void *addr = handle.pDrvPrivate;

		if (addr <= 0) __debugbreak();
		if (!addr) return -2;

		MapAddrToEid::iterator it = mapAddrToEid.find(addr);
		if (it == mapAddrToEid.end())
		{
			_ASSERT(FALSE);
			return -1;
		}
		
		return it->second;
	}

	template<class T> void AddHandleEventId(T handle,UINT EventId)
	{
		void *addr = handle.pDrvPrivate;

		if (!addr) return;

		mapAddrToEid[addr] = EventId;
	}

	std::set<Commands::CreateVertexShader *> GetShadersOfLayout(Commands::CreateElementLayout *layout);
	void SetShadersOfLayout(Commands::CreateElementLayout *layout,Commands::CreateVertexShader *);

	template<class T> void SetResourceOfView(T hview,D3D10DDI_HRESOURCE hres)
	{
		_ASSERT(ViewsResourcesMap.find(hview.pDrvPrivate) == ViewsResourcesMap.end());
		ViewsResourcesMap[hview.pDrvPrivate] = hres.pDrvPrivate;
	}

	template<class T> D3D10DDI_HRESOURCE GetResourceOfView(T hview)
	{
		if (!hview.pDrvPrivate || ViewsResourcesMap.find(hview.pDrvPrivate) == ViewsResourcesMap.end())
		{
			D3D10DDI_HRESOURCE hres = { 0 };
			return hres;
		}

		D3D10DDI_HRESOURCE hres = { ViewsResourcesMap[hview.pDrvPrivate] };
		return hres;
	}

	void IncreaseDxMode(GeneratorDxMode mode)
	{
		if (mode > m_generatorMode) m_generatorMode = mode;
	}

	GeneratorDxMode GetDxMode() { return m_generatorMode; }

	void AddClearTarget(UINT eventId,UINT cmdId);
	std::string GetClearAllString();

	void SetMainRTViewEventId(UINT eventId) { mainRTViewEid = eventId; }
	UINT GetMainRTViewEventId() { return mainRTViewEid; }

	BOOL NeedSetEidCommands() { return m_bAddSetEidCommands; }
	void DontNeedSetEidCommands() { m_bAddSetEidCommands = FALSE; }

protected:
	BOOL m_bAddSetEidCommands;

	UINT mainTextureEid;
	DXGI_FORMAT mainTextureFormat;
	UINT mainTextureWidth;
	UINT mainTextureHeight;

	typedef std::map<void *,int> MapAddrToEid;
	MapAddrToEid mapAddrToEid;

	typedef std::map<Commands::CreateElementLayout *, std::set<Commands::CreateVertexShader *> > ShaderLayoutMap;
	ShaderLayoutMap shaderLayoutMap;

	std::map<void *,void *> ViewsResourcesMap;
	std::set<void *> wasUsedAsRenderTarget;
	std::map<void *,D3D10DDIARG_CREATERESOURCE> resourcesMap;

	UINT mainRTViewEid;

	GeneratorDxMode m_generatorMode;

	std::string m_sClearAll;

	std::ifstream m_IndexFile;

	struct LoadedData
	{
		void *data;
		DWORD len;
		DWORD *params;
		DWORD count;

		UINT size;
	};
	static std::map<int,LoadedData> LoadedDataCache;
	static UINT m_cacheSize;
	static int m_newFileFormatFlag; // 1 == yes, -1 == no, 0 == unknown
};


template<class DXCREATERESOURCE>
void CDumpState::SaveCreateResourceStruct(D3D10DDI_HRESOURCE hres,DXCREATERESOURCE &cres)
{
	resourcesMap[hres.pDrvPrivate] = cres;
}

template<> 
void CDumpState::SaveCreateResourceStruct<D3D11DDIARG_CREATERESOURCE>(D3D10DDI_HRESOURCE hres,D3D11DDIARG_CREATERESOURCE &cres);

