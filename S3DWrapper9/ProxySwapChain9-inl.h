#pragma once

#ifndef OUTPUT_LIBRARY

#include "Trace.h"
#include "GlobalData.h"
#include "ProxySwapChain9.h"

#undef	PROXYMETHOD
#undef	PROXYMETHOD_
#undef	CALLORIGINAL
#undef	CALLORIGINALEX
#define PROXYMETHOD(a)			inline HRESULT  ProxySwapChain9::a 
#define PROXYMETHOD_(type, a)	inline type ProxySwapChain9::a
#define CALLORIGINAL(a)			(m_pHookedInterface->*m_fpOriginal.a)
#define CALLORIGINALEX(a)		(((IDirect3DSwapChain9Ex*)m_pHookedInterface)->*m_fpOriginal.a)

// IUnknown methods 
PROXYMETHOD(QueryInterface)(REFIID riid, void** ppvObj)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(QueryInterface)(riid, ppvObj);
}

PROXYMETHOD_(ULONG,AddRef)() 
{
	CHECKMODIFICATION();
	return CALLORIGINAL(AddRef)();
}

PROXYMETHOD_(ULONG,Release)()
{
	CHECKMODIFICATION();
 	return CALLORIGINAL(Release)();
}

// IDirect3D9 methods
PROXYMETHOD(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Present)(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags);
}

PROXYMETHOD(GetFrontBufferData)(IDirect3DSurface9* pDestSurface)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetFrontBufferData)(pDestSurface);
}

PROXYMETHOD(GetBackBuffer)(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetBackBuffer)(iBackBuffer,Type,ppBackBuffer);
}

PROXYMETHOD(GetRasterStatus)(D3DRASTER_STATUS* pRasterStatus)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetRasterStatus)(pRasterStatus);
}

PROXYMETHOD(GetDisplayMode)(D3DDISPLAYMODE* pMode)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDisplayMode)(pMode);
}

PROXYMETHOD(GetDevice)(IDirect3DDevice9** ppDevice)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDevice)(ppDevice);
}

PROXYMETHOD(GetPresentParameters)(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetPresentParameters)(pPresentationParameters);
}

// IDirect3DSwapChain9Ex methods
PROXYMETHOD(GetLastPresentCount)(UINT* pLastPresentCount)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetLastPresentCount)(pLastPresentCount);
}

PROXYMETHOD(GetPresentStats)(D3DPRESENTSTATS* pPresentationStatistics)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetPresentStats)(pPresentationStatistics);
}

PROXYMETHOD(GetDisplayModeEx)(D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetDisplayModeEx)(pMode,pRotation);
}

#endif
