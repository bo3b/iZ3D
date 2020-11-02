/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <vector>
#include "ProxyDevice9.h"
#include "ProxySwapChain9.h"

class CMonoRenderer;

class DECLSPEC_UUID("91BB00CA-29A0-45cc-A1DC-06E811423B98")
CMonoSwapChain : public IDirect3DSwapChain9Ex
{
protected:
	long								m_refCount;
	UINT								m_Index;
	CMonoRenderer*						m_pD3D9Device;

public:
	ProxySwapChain9						m_SwapChain;
	void								setExMode(PROXY_EXMODE exMode) { m_SwapChain.setExMode(exMode); }

	D3DPRESENT_PARAMETERS				m_OriginalPresentationParameters;
	D3DDISPLAYMODEEX					m_OriginalFullscreenDisplayMode;
	D3DDISPLAYMODEEX*					m_pFullscreenDisplayMode;

	HWND	GetAppWindow()
	{
		return m_OriginalPresentationParameters.hDeviceWindow;
	}

	CMonoSwapChain(CMonoRenderer*	pDirect3DDevice9, UINT index);
	virtual ~CMonoSwapChain(void);
	void SetPresentationParameters(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode);
	HRESULT InitializeEx(UINT iSwapChain);
	virtual HRESULT Initialize(IDirect3DSwapChain9* pSwapChain);
	virtual void	Clear();

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG,AddRef)(THIS);
	STDMETHOD_(ULONG,Release)(THIS);

	/*** IDirect3DSwapChain9 methods ***/
	STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	STDMETHOD(GetFrontBufferData)(THIS_ IDirect3DSurface9* pDestSurface);
	STDMETHOD(GetBackBuffer)(THIS_ UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	STDMETHOD(GetRasterStatus)(THIS_ D3DRASTER_STATUS* pRasterStatus);
	STDMETHOD(GetDisplayMode)(THIS_ D3DDISPLAYMODE* pMode);
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	STDMETHOD(GetPresentParameters)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters);

	/*** IDirect3DSwapChain9Ex methods ***/
	STDMETHOD(GetLastPresentCount)(THIS_ UINT* pLastPresentCount);
	STDMETHOD(GetPresentStats)(THIS_ D3DPRESENTSTATS* pPresentationStatistics);
	STDMETHOD(GetDisplayModeEx)(THIS_ D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation);
};
