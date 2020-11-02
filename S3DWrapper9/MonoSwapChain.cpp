/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "StdAfx.h"
#include "BaseStereoRenderer.h"
#include "ProxySwapChain9-inl.h"

CMonoSwapChain::CMonoSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index)
{
	m_refCount = 0;
	m_Index = index;
	m_pD3D9Device = pDirect3DDevice9;
	ZeroMemory(&m_OriginalPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	ZeroMemory(&m_OriginalFullscreenDisplayMode, sizeof(D3DDISPLAYMODEEX));
	m_pFullscreenDisplayMode = NULL;
}

CMonoSwapChain::~CMonoSwapChain(void)
{
	typedef std::vector<CMonoSwapChain*> SwapChains;
	SwapChains& sc = m_pD3D9Device->m_SwapChains;   
	SwapChains::iterator it;
	for (it = sc.begin(); it != sc.end(); ++it)
	{
		if (*it == this) {
			size_t pos = it - sc.begin();
			sc.erase(it);
#ifndef FINAL_RELEASE
			DEBUG_MESSAGE(_T("SwapChain %d removed\n"), pos);
			if (m_pD3D9Device->m_Direct3DDevice)
			{
				int count = (int)m_pD3D9Device->m_Direct3DDevice.GetNumberOfSwapChains();
				if (SUCCEEDED(count))
				{
					for (int i = 0; i < count; i++)
					{
						CComPtr<IDirect3DSwapChain9> pTempSwapChain;
						if (FAILED(m_pD3D9Device->m_Direct3DDevice.GetSwapChain(i, &pTempSwapChain)))
						{
							DEBUG_MESSAGE(_T("\tSwapChain %d already removed\n"), i);
							continue;
						}
					}
				}
				DEBUG_MESSAGE(_T("Real SwapChain count: %d\n"), count);
			}
			DEBUG_MESSAGE(_T("Total SwapChain count: %d\n"), sc.size());
#endif
			break;
		}
	}
	if (m_SwapChain)
	{
		RouterTypeHookCallGuard<ProxyDevice9> RTdev(&m_pD3D9Device->m_Direct3DDevice);
		m_SwapChain.Release();
	}
}

void CMonoSwapChain::SetPresentationParameters(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode)
{
	m_OriginalPresentationParameters = *pPresentationParameters;
	// for XLive and RE5
	if (m_OriginalPresentationParameters.hDeviceWindow == NULL)
		m_OriginalPresentationParameters.hDeviceWindow = m_pD3D9Device->m_hFocusWindow;
	if (m_OriginalPresentationParameters.Windowed && 
		m_OriginalPresentationParameters.BackBufferWidth == 0 && m_OriginalPresentationParameters.BackBufferHeight == 0)
	{
		RECT rc;
		GetClientRect(m_OriginalPresentationParameters.hDeviceWindow, &rc);
		m_OriginalPresentationParameters.BackBufferWidth = rc.right - rc.left;
		m_OriginalPresentationParameters.BackBufferHeight = rc.bottom - rc.top;
	}
	if (pFullscreenDisplayMode)
	{
		m_OriginalFullscreenDisplayMode = *pFullscreenDisplayMode;
		m_pFullscreenDisplayMode = &m_OriginalFullscreenDisplayMode;
	}
	else
		m_pFullscreenDisplayMode = NULL;
}

HRESULT CMonoSwapChain::InitializeEx(UINT iSwapChain)
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSwapChain9> pSwapChain;
	NSCALL(m_pD3D9Device->m_Direct3DDevice.GetSwapChain(iSwapChain, &pSwapChain));
	hResult = Initialize(pSwapChain);
	return hResult;
}

HRESULT CMonoSwapChain::Initialize(IDirect3DSwapChain9* pSwapChain)
{
	HRESULT hResult = S_OK;

	//--- add renderer to list ---
	g_pSwapChainWrapperList.AddWrapper(pSwapChain, this);
	m_SwapChain.initialize(pSwapChain);

	return hResult;
}

void CMonoSwapChain::Clear()
{
	g_pSwapChainWrapperList.RemoveWrapper(this);
}

// IUnknown
HRESULT CMonoSwapChain::QueryInterface( 
	REFIID riid,
	void** ppvObject)
{
	if( ppvObject == NULL ) {
		return E_POINTER;
	} 
	else if(InlineIsEqualGUID(riid, IID_IDirect3DSwapChain9)) {
		//DEBUG_TRACE3( __FUNCTIONW__ _T("(IDirect3DSwapChain9)\n") );
		*ppvObject = static_cast<IDirect3DSwapChain9*>( this );
		AddRef();
		return S_OK;
	}
	else if(InlineIsEqualGUID(riid, IID_IDirect3DSwapChain9Ex) && m_SwapChain) {
		//DEBUG_TRACE3( __FUNCTIONW__ _T("(IDirect3DSwapChain9Ex)\n")); 
		*ppvObject = static_cast<IDirect3DSwapChain9Ex*>( this );
		AddRef();
		return S_OK;
	}
	else 
	{
		//DEBUG_TRACE3( __FUNCTIONW__ _T("(other)\n"));
		return m_SwapChain.QueryInterface(riid, ppvObject);
	}
}

ULONG CMonoSwapChain::AddRef()
{
	//DEBUG_TRACE3(_T("CMonoSwapChain::AddRef(), %d\n"), m_refCount);
	return InterlockedIncrement(& m_refCount);
}

ULONG CMonoSwapChain::Release()
{
	//DEBUG_TRACE3(_T("CMonoSwapChain::Release(), %d\n"), m_refCount);
	ULONG ret = InterlockedDecrement(& m_refCount);
	if( ret == 0 )
	{
		Clear();
		delete this;
	}
	return ret;
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetFrontBufferData(IDirect3DSurface9* pDestSurface)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetFrontBufferData(pDestSurface);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetRasterStatus(pRasterStatus);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetDisplayMode(D3DDISPLAYMODE* pMode)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetDisplayMode(pMode);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetDevice(IDirect3DDevice9** ppDevice)
{
	if(gInfo.RouterType != ROUTER_TYPE_HOOK)
		*ppDevice = m_pD3D9Device;
	else
		*ppDevice = m_pD3D9Device->m_Direct3DDevice;

	m_pD3D9Device->AddRef();
	DEBUG_TRACE3(_T("MainSwapChain->GetDevice(pChain = %p, ppDevice = %p);\n"), 
		m_SwapChain.GetReal(), Indirect(ppDevice) );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	*pPresentationParameters = m_OriginalPresentationParameters;
	DEBUG_TRACE2(_T("MainSwapChain->GetPresentParameters(pPresentationParameters = %s);\n"), 
		GetPresentationParametersString(pPresentationParameters) );
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetBackBuffer(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetLastPresentCount(UINT* pLastPresentCount)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetLastPresentCount(pLastPresentCount);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetPresentStats(pPresentationStatistics);
}

HRESULT STDMETHODCALLTYPE CMonoSwapChain::GetDisplayModeEx(D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
{
	DEBUG_TRACE3( __FUNCTIONW__ _T("\n") );
	return m_SwapChain.GetDisplayModeEx(pMode, pRotation);
}

#pragma optimize( "y", off ) // for Steam Community

HRESULT STDMETHODCALLTYPE CMonoSwapChain::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
{
	DEBUG_TRACE3(_T("IDirect3DSwapChain9::Present(This = %p, pSourceRect = %s, pDestRect = %s, hDestWindowOverride = %p, pDirtyRegion = %p, dwFlags = %s)\n"),
		m_SwapChain.GetReal(), GetRectString(pSourceRect), GetRectString(pDestRect), hDestWindowOverride, pDirtyRegion, GetPresentFlagsString(dwFlags));
	HRESULT hResult = m_SwapChain.Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	return hResult;
}

#pragma optimize( "y", on ) // for Steam Community