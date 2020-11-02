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
// Direct3D8.cpp : Implementation of CDx8wrpApp and DLL registration.

#include "stdafx.h"
#include "Direct3D7.h"
#include "DirectDrawSurface7.h"
#include "DirectDrawClipper.h"
#include "DirectDrawPalette.h"
#include "Direct3DDevice7.h"
#include "Direct3DVertexBuffer7.h"
#include <algorithm>

CDirect3D7::CDirect3D7()
: m_nAdapter(0), m_DeviceType(D3DDEVTYPE_HAL), m_hFocusWindow(NULL), m_bDirectDrawMode(false),
 m_dwBehaviorFlags(D3DCREATE_MIXED_VERTEXPROCESSING)
{
	ZeroMemory(&m_PresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	m_PresentationParameters.Windowed = TRUE;
	m_PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
}

void CDirect3D7::Init( IDirectDraw7 *pDDraw7 )
{
	m_pDDraw7 = pDDraw7;	//ddraw.dll
	m_pD3D7 = pDDraw7;		//d3dim700.dll
	m_pDDraw4 = pDDraw7;
	m_pDDraw2 = pDDraw7;
	m_pDDraw7.QueryInterface(&m_pDDraw);
}

void CDirect3D7::Init( IDirectDraw *pDDraw )
{
	m_pDDraw = pDDraw;		//ddraw.dll
	m_pDDraw2 = pDDraw;
	m_pDDraw4 = pDDraw;
	m_pDDraw.QueryInterface(&m_pDDraw7);
	m_pD3D3 = pDDraw;		//d3dim.dll
}

HRESULT CDirect3D7::InitializeD3D9()
{
	if (m_pD3D9)
		return S_OK;
	m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	return m_pD3D9 ? S_OK : E_FAIL;
}

HRESULT CDirect3D7::InitializeD3D9Device()
{
	if (m_pD3D9Device)
		return S_OK;
	if (FAILED(InitializeD3D9()))
		return E_FAIL;
	HRESULT hr = m_pD3D9->CreateDevice(m_nAdapter, m_DeviceType, m_hFocusWindow, m_dwBehaviorFlags, 
		&m_PresentationParameters, &m_pD3D9Device);
	return hr;
}

HRESULT WINAPI CDirect3D7::DebugQueryInterface( void* pv, REFIID riid, LPVOID* ppv, DWORD dw )
{
	if (InlineIsEqualGUID(riid, IID_IDirectDraw7)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDraw7\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDraw4)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDraw4\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDraw2)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDraw2\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDraw)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDraw\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirect3D7)) {
		DEBUG_MESSAGE("QueryInterface: IDirect3D7 (D3D7)\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirect3D3)) {
		DEBUG_MESSAGE("QueryInterface: IDirect3D3 (D3D6)\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirect3D2)) {
		DEBUG_MESSAGE("QueryInterface: IDirect3D2 (D3D5)\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirect3D)) {
		DEBUG_MESSAGE("QueryInterface: IDirect3D (D3D2)\n");
	} else {
		DEBUG_MESSAGE("QueryInterface: Unknown\n");
	}
	*ppv = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CDirect3D7::Compact()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = S_OK;
	hResult = CDirectDrawClipper::CreateInstance(lplpDDClipper);
	hResult = (*lplpDDClipper)->Initialize((IDirectDraw*)this, dwFlags);
	return hResult;
}

STDMETHODIMP CDirect3D7::CreatePalette( DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE FAR *lplpDDPalette, IUnknown FAR *pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateSurface( LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPDIRECTDRAWSURFACE7 FAR *lplpDDSurface, IUnknown FAR *pUnkOuter )
{
	DEBUG_TRACE3("%s 7\n", __FUNCTION__);
	if (lpDDSurfaceDesc2->dwSize != sizeof(DDSURFACEDESC2))
		return DDERR_INVALIDPARAMS;

	HRESULT hResult = S_OK;
	if( lpDDSurfaceDesc2->dwFlags & DDSD_CAPS &&  lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
	{
		if( lpDDSurfaceDesc2->dwFlags & DDSD_BACKBUFFERCOUNT)
			m_PresentationParameters.BackBufferCount = lpDDSurfaceDesc2->dwBackBufferCount;
		//if( lpDDSurfaceDesc2->ddsCaps.dwCaps & DDSCAPS_FLIP)
		//	m_PresentationParameters.SwapEffect = D3DSWAPEFFECT_FLIP;
	}
	hResult = CDirectDrawSurface7::CreateInstance(lplpDDSurface);
	hResult = (*lplpDDSurface)->Initialize((IDirectDraw*)this, lpDDSurfaceDesc2);
	return hResult;
}

STDMETHODIMP CDirect3D7::CreateSurface( LPDDSURFACEDESC2 lpDDSurfaceDesc2,LPDIRECTDRAWSURFACE4 *lplpDDSurface,IUnknown *pUnkOuter )
{
	DEBUG_TRACE3("%s 4\n", __FUNCTION__);
	HRESULT hResult = S_OK;
	CComPtr<IDirectDrawSurface7> pSurf;
	hResult = CreateSurface(lpDDSurfaceDesc2, &pSurf, pUnkOuter);
	if (SUCCEEDED(hResult))
		pSurf->QueryInterface(IID_IDirectDrawSurface4, (void**)lplpDDSurface);
	return hResult;
}

STDMETHODIMP CDirect3D7::CreateSurface( LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE *lplpDDSurface,IUnknown *pUnkOuter )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = S_OK;
	CComPtr<IDirectDrawSurface7> pSurf;
	DDSURFACEDESC2 Desc2;
	memcpy(&Desc2, lpDDSurfaceDesc, sizeof DDSURFACEDESC);
	Desc2.dwSize = sizeof DDSURFACEDESC2;
	Desc2.dwTextureStage = 0;
	Desc2.ddsCaps.dwCaps2 = 0;
	Desc2.ddsCaps.dwCaps3 = 0;
	Desc2.ddsCaps.dwCaps4 = 0;
	hResult = CreateSurface(&Desc2, &pSurf, pUnkOuter);
	if (SUCCEEDED(hResult))
		pSurf->QueryInterface(IID_IDirectDrawSurface, (void**)lplpDDSurface);
	return hResult;
}

STDMETHODIMP CDirect3D7::DuplicateSurface( LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 FAR *lplpDupDDSurface )
{
	DEBUG_MESSAGE("Not implemented: %s 7\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::DuplicateSurface( LPDIRECTDRAWSURFACE4 lpDDSurface,LPDIRECTDRAWSURFACE4 *lplpDupDDSurface )
{
	DEBUG_MESSAGE("Not implemented: %s 4\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::DuplicateSurface( LPDIRECTDRAWSURFACE lpDDSurface,LPDIRECTDRAWSURFACE *lplpDupDDSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::EnumDisplayModes( DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback )
{
	DEBUG_TRACE3("%s 2\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw7->EnumDisplayModes(dwFlags, lpDDSurfaceDesc2, lpContext, lpEnumModesCallback);
	return hResult;
}

STDMETHODIMP CDirect3D7::EnumDisplayModes( DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumModesCallback )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
	return hResult;
}

STDMETHODIMP CDirect3D7::EnumSurfaces( DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback )
{
	DEBUG_TRACE3("%s 7\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw7->EnumSurfaces(dwFlags, lpDDSD2, lpContext, lpEnumSurfacesCallback);
	return hResult;
}

STDMETHODIMP CDirect3D7::EnumSurfaces( DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::EnumSurfaces( DWORD dwFlags, LPDDSURFACEDESC lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::FlipToGDISurface()
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (m_pD3D9Device)
		m_pD3D9Device->Present(NULL, NULL, NULL, NULL);
	return S_OK;
}

STDMETHODIMP CDirect3D7::GetCaps( LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw4->GetCaps(lpDDDriverCaps, lpDDHELCaps);
	return hResult;
	/*
	ZeroMemory(lpDDDriverCaps, sizeof(DDCAPS));
	lpDDDriverCaps->dwSize = sizeof(DDCAPS);
	lpDDDriverCaps->dwCaps = 0x85d27fc1;
	lpDDDriverCaps->dwCaps2	= 0xa062b270 | DDCAPS2_CANRENDERWINDOWED;
	lpDDDriverCaps->dwCKeyCaps = 0x00000210;
	lpDDDriverCaps->dwFXCaps = 0x3aad54e0;
	lpDDDriverCaps->dwVidMemTotal = 0xcf11f000;
	lpDDDriverCaps->dwVidMemFree = 0xce855000;
	lpDDDriverCaps->dwMaxVisibleOverlays = 0x00000001;
	lpDDDriverCaps->dwNumFourCCCodes = 0x00000011;
	lpDDDriverCaps->dwRops[6] = 0x00001000;
	lpDDDriverCaps->ddsOldCaps.dwCaps = 0x304672fc;
	lpDDDriverCaps->dwMinOverlayStretch	= 0x00000001;
	lpDDDriverCaps->dwMaxOverlayStretch	= 0x00004e20;
	lpDDDriverCaps->dwSVBCaps = 0x00000040;
	memcpy(lpDDDriverCaps->dwSVBRops, lpDDDriverCaps->dwRops, sizeof(lpDDDriverCaps->dwRops));
	lpDDDriverCaps->dwVSBCaps = lpDDDriverCaps->dwSVBCaps;
	memcpy(lpDDDriverCaps->dwVSBRops, lpDDDriverCaps->dwRops, sizeof(lpDDDriverCaps->dwRops));
	lpDDDriverCaps->dwNLVBCaps = lpDDDriverCaps->dwCaps;
	lpDDDriverCaps->dwNLVBCaps2 = 0xa062b270;
	lpDDDriverCaps->dwNLVBCKeyCaps = lpDDDriverCaps->dwCKeyCaps;
	lpDDDriverCaps->dwNLVBFXCaps = lpDDDriverCaps->dwFXCaps;
	memcpy(lpDDDriverCaps->dwNLVBRops, lpDDDriverCaps->dwRops, sizeof(lpDDDriverCaps->dwRops));
	lpDDDriverCaps->ddsCaps.dwCaps = lpDDDriverCaps->ddsOldCaps.dwCaps;
	lpDDDriverCaps->ddsCaps.dwCaps2 = 0x00000200;

	ZeroMemory(lpDDHELCaps, sizeof(DDCAPS));
	lpDDHELCaps->dwSize = sizeof(DDCAPS);
	lpDDHELCaps->dwCaps = 0xf4c08241;
	lpDDHELCaps->dwCaps2 = 0x00000001; // DDCAPS2_CANRENDERWINDOWED
	lpDDHELCaps->dwCKeyCaps = 0x00000200;
	lpDDHELCaps->dwFXCaps = 0x0003fce3;
	lpDDHELCaps->dwPalCaps = 0x00000347;
	lpDDHELCaps->dwZBufferBitDepths = 0x00000400;
	lpDDHELCaps->dwRops[0x0] = 0x00000001;
	lpDDHELCaps->dwRops[0x6] = 0x00001000;
	lpDDHELCaps->dwRops[0x7] = 0x80000000;
	lpDDHELCaps->ddsOldCaps.dwCaps = 0x00421350;
	lpDDHELCaps->dwSVBCaps = lpDDHELCaps->dwCaps;
	lpDDHELCaps->dwSVBCKeyCaps = lpDDHELCaps->dwCKeyCaps;
	lpDDHELCaps->dwSVBFXCaps = lpDDHELCaps->dwFXCaps;
	memcpy(lpDDHELCaps->dwSVBRops, lpDDHELCaps->dwRops, sizeof(lpDDHELCaps->dwRops));
	lpDDHELCaps->dwVSBCaps = lpDDHELCaps->dwCaps;
	lpDDHELCaps->dwVSBCKeyCaps = lpDDHELCaps->dwCKeyCaps;
	lpDDHELCaps->dwVSBFXCaps = lpDDHELCaps->dwFXCaps;
	memcpy(lpDDHELCaps->dwVSBRops, lpDDHELCaps->dwRops, sizeof(lpDDHELCaps->dwRops));
	lpDDHELCaps->dwSSBCaps = lpDDHELCaps->dwCaps;
	lpDDHELCaps->dwSSBCKeyCaps = lpDDHELCaps->dwCKeyCaps;
	lpDDHELCaps->dwSSBFXCaps = lpDDHELCaps->dwFXCaps;
	memcpy(lpDDHELCaps->dwSSBRops, lpDDHELCaps->dwRops, sizeof(lpDDHELCaps->dwRops));
	lpDDHELCaps->dwNLVBCaps = lpDDHELCaps->dwCaps;
	lpDDHELCaps->dwNLVBCaps2 = lpDDHELCaps->dwCaps2;
	lpDDHELCaps->dwNLVBCKeyCaps = lpDDHELCaps->dwCKeyCaps;
	lpDDHELCaps->dwNLVBFXCaps = lpDDHELCaps->dwFXCaps;
	memcpy(lpDDHELCaps->dwNLVBRops, lpDDHELCaps->dwRops, sizeof(lpDDHELCaps->dwRops));
	lpDDHELCaps->ddsCaps.dwCaps = lpDDHELCaps->ddsOldCaps.dwCaps;
	lpDDHELCaps->ddsCaps.dwCaps2 = lpDDHELCaps->dwCKeyCaps;
	

	return S_OK;*/
}

STDMETHODIMP CDirect3D7::GetDisplayMode( LPDDSURFACEDESC2 lpDDSurfaceDesc2 )
{
	DEBUG_TRACE3("%s 2\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw7->GetDisplayMode(lpDDSurfaceDesc2);
	return hResult;
}

STDMETHODIMP CDirect3D7::GetDisplayMode( LPDDSURFACEDESC lpDDSurfaceDesc )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw->GetDisplayMode(lpDDSurfaceDesc);

	// fix after SetDisplayMode check
	lpDDSurfaceDesc->dwWidth = m_PresentationParameters.BackBufferWidth;
	lpDDSurfaceDesc->dwHeight = m_PresentationParameters.BackBufferHeight;
	return hResult;
}

STDMETHODIMP CDirect3D7::GetFourCCCodes( LPDWORD lpNumCodes, LPDWORD lpCodes )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetGDISurface( LPDIRECTDRAWSURFACE7 FAR *lplpGDIDDSSurface )
{
	DEBUG_MESSAGE("Not implemented: %s 7\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetGDISurface( LPDIRECTDRAWSURFACE4 *lplpGDIDDSSurface )
{
	DEBUG_MESSAGE("Not implemented: %s 4\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetGDISurface( LPDIRECTDRAWSURFACE *lplpGDIDDSSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetMonitorFrequency( LPDWORD lpdwFrequency )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetScanLine( LPDWORD lpdwScanLine )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw->GetScanLine(lpdwScanLine);
	return hResult;
}

STDMETHODIMP CDirect3D7::GetVerticalBlankStatus( LPBOOL lpbIsInVB )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pDDraw->GetVerticalBlankStatus(lpbIsInVB);
	return hResult;
}

STDMETHODIMP CDirect3D7::Initialize( GUID FAR *lpGUID )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::Initialize( REFCLSID GUID )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::RestoreDisplayMode()
{
	// TODO: Call D3D9Device Reset
	return S_OK;
}

STDMETHODIMP CDirect3D7::SetCooperativeLevel( HWND hWnd, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (dwFlags & DDSCL_SETDEVICEWINDOW)
		m_PresentationParameters.hDeviceWindow = hWnd;
	else if (dwFlags & DDSCL_SETFOCUSWINDOW)
		m_hFocusWindow = hWnd;
	else
	{
		m_PresentationParameters.hDeviceWindow = hWnd;
		m_hFocusWindow = hWnd;
	}
	if (dwFlags & DDSCL_CREATEDEVICEWINDOW)
		__noop;
	if (dwFlags & DDSCL_NORMAL)
		__noop;
	if (dwFlags & DDSCL_ALLOWMODEX)
		__noop;
	if (dwFlags & DDSCL_ALLOWREBOOT)
		__noop;
	if (dwFlags & DDSCL_FPUPRESERVE)
		m_dwBehaviorFlags |= D3DCREATE_FPU_PRESERVE;
	if (dwFlags & DDSCL_FPUSETUP)
		__noop;
	if (dwFlags & DDSCL_MULTITHREADED)
		m_dwBehaviorFlags |= D3DCREATE_MULTITHREADED;
	if (dwFlags & DDSCL_NOWINDOWCHANGES)
		m_dwBehaviorFlags |= D3DCREATE_NOWINDOWCHANGES;
	if (dwFlags & DDSCL_EXCLUSIVE)
		__noop;
	if (dwFlags & DDSCL_FULLSCREEN)
		m_PresentationParameters.Windowed = FALSE;
	dwFlags &= ~(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX);
	dwFlags |= DDSCL_NORMAL;
	HRESULT hResult = m_pDDraw7->SetCooperativeLevel(hWnd, dwFlags);
	return S_OK;
}

STDMETHODIMP CDirect3D7::SetDisplayMode( DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	m_PresentationParameters.BackBufferWidth = dwWidth;
	m_PresentationParameters.BackBufferHeight = dwHeight;
	switch(dwBPP)
	{
	case 32:
		m_PresentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		break;
	case 16:
		m_PresentationParameters.BackBufferFormat = D3DFMT_R5G6B5;
		break;
	}
	m_PresentationParameters.FullScreen_RefreshRateInHz = dwRefreshRate;
	return S_OK;
}

// IDirect3D2
STDMETHODIMP CDirect3D7::SetDisplayMode( DWORD dwWidth, DWORD dwHeight, DWORD dwBPP )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	m_PresentationParameters.BackBufferWidth = dwWidth;
	m_PresentationParameters.BackBufferHeight = dwHeight;
	switch(dwBPP)
	{
	case 32:
		m_PresentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		break;
	case 16:
		m_PresentationParameters.BackBufferFormat = D3DFMT_R5G6B5;
		break;
	}
	return S_OK;
}

STDMETHODIMP CDirect3D7::WaitForVerticalBlank( DWORD dwFlags, HANDLE hEvent )
{
	HRESULT hResult = m_pDDraw->WaitForVerticalBlank(dwFlags, hEvent);
	return hResult;
}

STDMETHODIMP CDirect3D7::GetAvailableVidMem( LPDDSCAPS2 lpDDSCaps2, LPDWORD lpdwTotal, LPDWORD lpdwFree )
{
	HRESULT hResult = m_pDDraw4->GetAvailableVidMem(lpDDSCaps2, lpdwTotal, lpdwFree);
	return hResult;
}

STDMETHODIMP CDirect3D7::GetAvailableVidMem( LPDDSCAPS lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree )
{
	HRESULT hResult = m_pDDraw2->GetAvailableVidMem(lpDDSCaps, lpdwTotal, lpdwFree);
	return hResult;
}

STDMETHODIMP CDirect3D7::GetSurfaceFromDC( HDC hdc, LPDIRECTDRAWSURFACE7 * lpDDS )
{
	DEBUG_MESSAGE("Not implemented: %s 7\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetSurfaceFromDC( HDC hdc, LPDIRECTDRAWSURFACE4 * lpDDS )
{
	DEBUG_MESSAGE("Not implemented: %s 4\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::RestoreAllSurfaces()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::TestCooperativeLevel()
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pD3D9Device ? m_pD3D9Device->TestCooperativeLevel() : S_OK;
	return hResult;
}

STDMETHODIMP CDirect3D7::GetDeviceIdentifier( LPDDDEVICEIDENTIFIER2 lpdddi, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::GetDeviceIdentifier( LPDDDEVICEIDENTIFIER lpdddi, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::StartModeTest( LPSIZE lpModesToTest, DWORD dwNumEntries, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::EvaluateMode( DWORD dwFlags, DWORD *pSecondsUntilTimeout )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::EnumDevices( LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, LPVOID lpUserArg )
{
	DEBUG_TRACE3("%s 7\n", __FUNCTION__);
	HRESULT hResult = m_pD3D7->EnumDevices(lpEnumDevicesCallback, lpUserArg);
	return hResult;
}

STDMETHODIMP CDirect3D7::EnumDevices( LPD3DENUMDEVICESCALLBACK lpEnumDevicesCallback, LPVOID lpUserArg )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateDevice( REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS, LPDIRECT3DDEVICE7 * lplpD3DDevice )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = S_OK;
	hResult = CDirect3DDevice7::CreateInstance(lplpD3DDevice);
	CDirect3DDevice7* pDev = dynamic_cast<CDirect3DDevice7*>(*lplpD3DDevice);
	hResult = pDev->Initialize(this, rclsid, lpDDS);
	return S_OK;
}

STDMETHODIMP CDirect3D7::CreateDevice( REFCLSID rclsid,LPDIRECTDRAWSURFACE4 lpDDS,LPDIRECT3DDEVICE3 * lplpD3DDevice, LPUNKNOWN pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s 4\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateDevice( REFCLSID rclsid,LPDIRECTDRAWSURFACE lpDDS,LPDIRECT3DDEVICE2 * lplpD3DDevice )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateVertexBuffer( LPD3DVERTEXBUFFERDESC lpVBDesc, LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = S_OK;
	IWrapper* pWrapper = NULL;
	hResult = CDirect3DVertexBuffer7::CreateInstance(&pWrapper);
	CDirect3DVertexBuffer7* pVB = (CDirect3DVertexBuffer7*)pWrapper;
	hResult = pVB->Initialize(this, lpVBDesc);
	*lplpD3DVertexBuffer = pVB;
	return hResult;
}

STDMETHODIMP CDirect3D7::CreateVertexBuffer( LPD3DVERTEXBUFFERDESC lpVBDesc,LPDIRECT3DVERTEXBUFFER* lplpD3DVertexBuffer, DWORD dwFlags, LPUNKNOWN pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::EnumZBufferFormats( REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID lpContext )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult;
	if (m_pD3D7)
		hResult = m_pD3D7->EnumZBufferFormats(riidDevice, lpEnumCallback, lpContext);
	else
		hResult = m_pD3D3->EnumZBufferFormats(riidDevice, lpEnumCallback, lpContext);
	return hResult;
}

STDMETHODIMP CDirect3D7::EvictManagedTextures()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateLight( LPDIRECT3DLIGHT* lplpDirect3DLight, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateMaterial( LPDIRECT3DMATERIAL3* lplpDirect3DMaterial3, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s 3\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateMaterial( LPDIRECT3DMATERIAL2* lplpDirect3DMaterial2, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateMaterial( LPDIRECT3DMATERIAL* lplpDirect3DMaterial, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateViewport( LPDIRECT3DVIEWPORT3* lplpD3DViewport3, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s 3\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateViewport( LPDIRECT3DVIEWPORT2* lplpD3DViewport2, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::CreateViewport( LPDIRECT3DVIEWPORT* lplpD3DViewport, IUnknown* pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3D7::FindDevice( LPD3DFINDDEVICESEARCH lpD3DFDS, LPD3DFINDDEVICERESULT lpD3DFDR )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}
