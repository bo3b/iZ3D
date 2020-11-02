#include "stdafx.h"
#include "Hook.h"

//--- unmodified IDirect3DSwapChain9 function.  Hooked in NGuard compatible hooking case ---
namespace IDirect3DSwapChain9MethodNames
{
	// IUnknown methods
	PROXYSTDMETHOD(QueryInterface)(IDirect3DSwapChain9* pSwapChain, REFIID riid, void** ppvObj);
	PROXYSTDMETHOD_(ULONG,AddRef)(IDirect3DSwapChain9* pSwapChain);
	PROXYSTDMETHOD_(ULONG,Release)(IDirect3DSwapChain9* pSwapChain);

	// IDirect3DSwapChain9 methods
	PROXYSTDMETHOD(Present)(IDirect3DSwapChain9* pSwapChain, CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	PROXYSTDMETHOD(GetFrontBufferData)(IDirect3DSwapChain9* pSwapChain, IDirect3DSurface9* pDestSurface);
	PROXYSTDMETHOD(GetBackBuffer)(IDirect3DSwapChain9* pSwapChain, UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	PROXYSTDMETHOD(GetRasterStatus)(IDirect3DSwapChain9* pSwapChain, D3DRASTER_STATUS* pRasterStatus);
	PROXYSTDMETHOD(GetDisplayMode)(IDirect3DSwapChain9* pSwapChain, D3DDISPLAYMODE* pMode);
	PROXYSTDMETHOD(GetDevice)(IDirect3DSwapChain9* pSwapChain, IDirect3DDevice9** ppDevice);
	PROXYSTDMETHOD(GetPresentParameters)(IDirect3DSwapChain9* pSwapChain, D3DPRESENT_PARAMETERS* pPresentationParameters);

	// IDirect3DSwapChain9Ex methods
	PROXYSTDMETHOD(GetLastPresentCount)(IDirect3DSwapChain9Ex* pSwapChain, UINT* pLastPresentCount);
	PROXYSTDMETHOD(GetPresentStats)(IDirect3DSwapChain9Ex* pSwapChain, D3DPRESENTSTATS* pPresentationStatistics);
	PROXYSTDMETHOD(GetDisplayModeEx)(IDirect3DSwapChain9Ex* pSwapChain, D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);
}
