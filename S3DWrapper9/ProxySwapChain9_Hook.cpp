#include "stdafx.h"
#include "MonoRenderer.h"
#include "BaseSwapChain.h"
#include "ProxySwapChain9-inl.h"
#include "ProxySwapChain9_Hook.h"
#include "ProxyMulti_Hook.h"

using namespace IDirect3DSwapChain9MethodNames;

//--- unmodified IDirect3DSwapChain9 function.  Hooked in NGuard compatible hooking case ---

void CBaseSwapChain::HookIDirect3DSwapChain9All()
{

#define HOOKINTERFACEENTRY(x) m_SwapChain.HookInterfaceEntry(this, _ ##x ##_, Proxy_ ##x)

	//--- it's not necessary to hook unmodified functions ---
	//--- uncomment appropriate hook if you wish modify function ---
	//HOOKINTERFACEENTRY(QueryInterface);
	HOOKINTERFACEENTRY(AddRef);
	
	//--- use multi hook  instead of this ---
	HOOKINTERFACEENTRY(Release);	
	/*
	void** p = *(void***)m_SwapChain.m_pHookedInterface;												
	if(IsCodeHookedByProxy(p[_Release_], Proxy_Surface_And_SwapChain_Release))
		m_SwapChain.m_fpOriginal.func[_Release_] = UniqueGetCode(p[_Release_]);
	else
	{
		m_SwapChain.HookInterfaceEntry(this, _Release_, Proxy_Surface_And_SwapChain_Release);
		*(PVOID*)(&Original_Surface_And_SwapChain_Release) = UniqueGetCode(p[_Release_]);
	}
	*/
	// IDirect3DSwapChain9 methods
	HOOKINTERFACEENTRY(Present);
	//HOOKINTERFACEENTRY(GetFrontBufferData);
	HOOKINTERFACEENTRY(GetBackBuffer);
	//HOOKINTERFACEENTRY(GetRasterStatus);
	HOOKINTERFACEENTRY(GetDisplayMode);
	//HOOKINTERFACEENTRY(GetDevice);
	//HOOKINTERFACEENTRY(GetPresentParameters);

	// IDirect3DSwapChain9Ex methods
	if(m_SwapChain.getExMode() != EXMODE_NONE)
	{
		//HOOKINTERFACEENTRY(GetLastPresentCount);
		//HOOKINTERFACEENTRY(GetPresentStats);
		HOOKINTERFACEENTRY(GetDisplayModeEx);
	}
}

void CBaseSwapChain::UnHookIDirect3DSwapChain9All()
{
	if (!m_SwapChain)
		return;

#define UNHOOKINTERFACEENTRY(x) m_SwapChain.UnhookInterfaceEntry(this, _ ##x ##_)

	//--- it's not necessary to unhook unmodified functions ---
	//--- uncomment appropriate unhook if you wish modify function ---
	//UNHOOKINTERFACEENTRY(QueryInterface);
	UNHOOKINTERFACEENTRY(AddRef);
	UNHOOKINTERFACEENTRY(Release);

	// IDirect3DSwapChain9 methods
	UNHOOKINTERFACEENTRY(Present);
	//UNHOOKINTERFACEENTRY(GetFrontBufferData);
	UNHOOKINTERFACEENTRY(GetBackBuffer);
	//UNHOOKINTERFACEENTRY(GetRasterStatus);
	UNHOOKINTERFACEENTRY(GetDisplayMode);
	//UNHOOKINTERFACEENTRY(GetDevice);
	//UNHOOKINTERFACEENTRY(GetPresentParameters);

	// IDirect3DSwapChain9Ex methods
	if(m_SwapChain.getExMode() != EXMODE_NONE)
	{
		//UNHOOKINTERFACEENTRY(GetLastPresentCount);
		//UNHOOKINTERFACEENTRY(GetPresentStats);
		UNHOOKINTERFACEENTRY(GetDisplayModeEx);
	}
}

namespace IDirect3DSwapChain9MethodNames
{
	// IUnknown methods
	PROXYSTDMETHOD(QueryInterface)(IDirect3DSwapChain9* pSwapChain, REFIID riid, void** ppvObj)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->QueryInterface(riid, ppvObj);
			else
				return pWrapper->m_SwapChain.QueryInterface(riid, ppvObj);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, QueryInterface);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, QueryInterface)(riid, ppvObj);
			return hResult;
		}
	}

	PROXYSTDMETHOD_(ULONG,AddRef)(IDirect3DSwapChain9* pSwapChain)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (pWrapper->m_SwapChain.m_RouterHookGuard > 0)
				InterlockedIncrement(&pWrapper->m_SwapChain.m_InternalReferenceCount);
			ULONG rc = pWrapper->m_SwapChain.AddRef();
			DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc, pWrapper->m_SwapChain.m_InternalReferenceCount, pWrapper->m_SwapChain.m_RouterHookGuard);
			return rc - pWrapper->m_SwapChain.m_InternalReferenceCount;
		}
		else
		{
			DECLAREPOINTER(pSwapChain, AddRef);
			ULONG uCount = CALLFORPOINTER(pSwapChain, AddRef)();
			return uCount;
		}
	}

	PROXYSTDMETHOD_(ULONG,Release)(IDirect3DSwapChain9* pSwapChain)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (pWrapper->m_SwapChain.m_RouterHookGuard > 0)
				InterlockedDecrement(&pWrapper->m_SwapChain.m_InternalReferenceCount);
			
			pWrapper->m_SwapChain.AddRef();
			ULONG rc = pWrapper->m_SwapChain.Release();
			LONG internalCount = std::max<LONG>(0, pWrapper->m_SwapChain.m_InternalReferenceCount);
			DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc-1, pWrapper->m_SwapChain.m_InternalReferenceCount, pWrapper->m_SwapChain.m_RouterHookGuard);
			if ((LONG)rc <= pWrapper->m_SwapChain.m_InternalReferenceCount + 1)
			{
				//--- guard from recursion wrapper releasing ---
				pWrapper->m_SwapChain.m_InternalReferenceCount = -1;
				rc = pWrapper->Release();
				internalCount = 0;
			}
			else
				rc = pWrapper->m_SwapChain.Release();
			return rc - internalCount;
		}
		else
		{
			DECLAREPOINTER(pSwapChain, Release);
			ULONG uCount = CALLFORPOINTER(pSwapChain, Release)();
			return uCount;
		}
	}

	// IDirect3DSwapChain9 methods
	PROXYSTDMETHOD(Present)(IDirect3DSwapChain9* pSwapChain, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
			else
				return pWrapper->m_SwapChain.Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, Present);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, Present)(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetFrontBufferData)(IDirect3DSwapChain9* pSwapChain, IDirect3DSurface9* pDestSurface)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetFrontBufferData(pDestSurface);
			else
				return pWrapper->m_SwapChain.GetFrontBufferData(pDestSurface);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetFrontBufferData);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetFrontBufferData)(pDestSurface);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetBackBuffer)(IDirect3DSwapChain9* pSwapChain, UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
			else
				return pWrapper->m_SwapChain.GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetBackBuffer);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetBackBuffer)(iBackBuffer, Type, ppBackBuffer);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetRasterStatus)(IDirect3DSwapChain9* pSwapChain, D3DRASTER_STATUS* pRasterStatus)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetRasterStatus(pRasterStatus);
			else
				return pWrapper->m_SwapChain.GetRasterStatus(pRasterStatus);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetRasterStatus);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetRasterStatus)(pRasterStatus);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetDisplayMode)(IDirect3DSwapChain9* pSwapChain, D3DDISPLAYMODE* pMode)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetDisplayMode(pMode);
			else
				return pWrapper->m_SwapChain.GetDisplayMode(pMode);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetDisplayMode);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetDisplayMode)(pMode);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetDevice)(IDirect3DSwapChain9* pSwapChain, IDirect3DDevice9** ppDevice)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetDevice(ppDevice);
			else
				return pWrapper->m_SwapChain.GetDevice(ppDevice);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetDevice);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetDevice)(ppDevice);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetPresentParameters)(IDirect3DSwapChain9* pSwapChain, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetPresentParameters(pPresentationParameters);
			else
				return pWrapper->m_SwapChain.GetPresentParameters(pPresentationParameters);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetPresentParameters);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetPresentParameters)(pPresentationParameters);
			return hResult;
		}
	}

	// IDirect3DSwapChain9Ex methods
	PROXYSTDMETHOD(GetLastPresentCount)(IDirect3DSwapChain9Ex* pSwapChain, UINT* pLastPresentCount)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetLastPresentCount(pLastPresentCount);
			else
				return pWrapper->m_SwapChain.GetLastPresentCount(pLastPresentCount);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetLastPresentCount);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetLastPresentCount)(pLastPresentCount);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetPresentStats)(IDirect3DSwapChain9Ex* pSwapChain, D3DPRESENTSTATS* pPresentationStatistics)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetPresentStats(pPresentationStatistics);
			else
				return pWrapper->m_SwapChain.GetPresentStats(pPresentationStatistics);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetPresentStats);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetPresentStats)(pPresentationStatistics);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetDisplayModeEx)(IDirect3DSwapChain9Ex* pSwapChain, D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
	{
		CBaseSwapChain* pWrapper = FindSwapChainWrapper(pSwapChain);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetDisplayModeEx(pMode, pRotation);
			else
				return pWrapper->m_SwapChain.GetDisplayModeEx(pMode, pRotation);
		}
		else
		{
			DECLAREPOINTER(pSwapChain, GetDisplayModeEx);
			HRESULT hResult = CALLFORPOINTER(pSwapChain, GetDisplayModeEx)(pMode, pRotation);
			return hResult;
		}
	}
}
