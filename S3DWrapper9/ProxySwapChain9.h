/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "D3DInterfaceMethods.h"
#include "ProxyBase.h"

namespace IDirect3DSwapChain9MethodNames
{
	#define PROXYSWAPCHAINMETHODTYPE(a)			typedef HRESULT	 (__stdcall IDirect3DSwapChain9::*a##_t)
	#define PROXYSWAPCHAINMETHODTYPE_(type, a)	typedef type     (__stdcall IDirect3DSwapChain9::*a##_t)

	// IUnknown methods
	PROXYSWAPCHAINMETHODTYPE(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYSWAPCHAINMETHODTYPE_(ULONG,AddRef)();
	PROXYSWAPCHAINMETHODTYPE_(ULONG,Release)();

	// IDirect3DSwapChain9 methods
	PROXYSWAPCHAINMETHODTYPE(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	PROXYSWAPCHAINMETHODTYPE(GetFrontBufferData)(IDirect3DSurface9* pDestSurface);
	PROXYSWAPCHAINMETHODTYPE(GetBackBuffer)(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	PROXYSWAPCHAINMETHODTYPE(GetRasterStatus)(D3DRASTER_STATUS* pRasterStatus);
	PROXYSWAPCHAINMETHODTYPE(GetDisplayMode)(D3DDISPLAYMODE* pMode);
	PROXYSWAPCHAINMETHODTYPE(GetDevice)(IDirect3DDevice9** ppDevice);
	PROXYSWAPCHAINMETHODTYPE(GetPresentParameters)(D3DPRESENT_PARAMETERS* pPresentationParameters);
	
	#define PROXYSWAPCHAINEXMETHODTYPE(a)			typedef HRESULT	 (__stdcall IDirect3DSwapChain9Ex::*a##_t)
	#define PROXYSWAPCHAINEXMETHODTYPE_(type, a)	typedef type     (__stdcall IDirect3DSwapChain9Ex::*a##_t)

	// IDirect3DSwapChain9Ex methods
	PROXYSWAPCHAINEXMETHODTYPE(GetLastPresentCount)(UINT* pLastPresentCount);
	PROXYSWAPCHAINEXMETHODTYPE(GetPresentStats)(D3DPRESENTSTATS* pPresentationStatistics);
	PROXYSWAPCHAINEXMETHODTYPE(GetDisplayModeEx)(D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);

	#define PROXYSWAPCHAINMETHOD(a)			a##_t a
	#define PROXYSWAPCHAINMETHOD_(type, a)	a##_t a

	union FunctionList
	{
		static const int MAX_METHODS    = _MaxMethods_;
		static const int MAX_METHODS_EX = _MaxMethodsEx_;

		FunctionList()
		{
			ZeroMemory(func, sizeof(func));
		}
		struct
		{
			// IUnknown methods
			PROXYSWAPCHAINMETHOD(QueryInterface);
			PROXYSWAPCHAINMETHOD_(ULONG,AddRef);
			PROXYSWAPCHAINMETHOD_(ULONG,Release);

			// IDirect3DSwapChain9 methods
			PROXYSWAPCHAINMETHOD(Present);
			PROXYSWAPCHAINMETHOD(GetFrontBufferData);
			PROXYSWAPCHAINMETHOD(GetBackBuffer);
			PROXYSWAPCHAINMETHOD(GetRasterStatus);
			PROXYSWAPCHAINMETHOD(GetDisplayMode);
			PROXYSWAPCHAINMETHOD(GetDevice);
			PROXYSWAPCHAINMETHOD(GetPresentParameters);

			// IDirect3DSwapChain9Ex methods
			PROXYSWAPCHAINMETHOD(GetLastPresentCount);
			PROXYSWAPCHAINMETHOD(GetPresentStats);
			PROXYSWAPCHAINMETHOD(GetDisplayModeEx);
		};

		static const TCHAR* GetFunctionName(int id)
		{
			return GetSwapChain9MethodName(id);
		}

		void* func[MAX_METHODS_EX];
	};
}

class ProxySwapChain9 : public ProxyBase<IDirect3DSwapChain9, IDirect3DSwapChain9MethodNames::FunctionList>
{
public:

	#undef	PROXYSWAPCHAINMETHOD
	#undef	PROXYSWAPCHAINMETHOD_
	#define PROXYSWAPCHAINMETHOD(a)			HRESULT a 
	#define PROXYSWAPCHAINMETHOD_(type, a)	type    a
	
	// IUnknown methods
	PROXYSWAPCHAINMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYSWAPCHAINMETHOD_(ULONG,AddRef)();
	PROXYSWAPCHAINMETHOD_(ULONG,Release)();

	// IDirect3DSwapChain9 methods
	PROXYSWAPCHAINMETHOD(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	PROXYSWAPCHAINMETHOD(GetFrontBufferData)(IDirect3DSurface9* pDestSurface);
	PROXYSWAPCHAINMETHOD(GetBackBuffer)(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	PROXYSWAPCHAINMETHOD(GetRasterStatus)(D3DRASTER_STATUS* pRasterStatus);
	PROXYSWAPCHAINMETHOD(GetDisplayMode)(D3DDISPLAYMODE* pMode);
	PROXYSWAPCHAINMETHOD(GetDevice)(IDirect3DDevice9** ppDevice);
	PROXYSWAPCHAINMETHOD(GetPresentParameters)(D3DPRESENT_PARAMETERS* pPresentationParameters);

	// IDirect3DSwapChain9Ex methods
	PROXYSWAPCHAINMETHOD(GetLastPresentCount)(UINT* pLastPresentCount);
	PROXYSWAPCHAINMETHOD(GetPresentStats)(D3DPRESENTSTATS* pPresentationStatistics);
	PROXYSWAPCHAINMETHOD(GetDisplayModeEx)(D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);
};
