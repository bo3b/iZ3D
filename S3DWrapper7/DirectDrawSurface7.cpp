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
#include "stdafx.h"
#include "DirectDrawSurface7.h"
#include "DirectDrawClipper.h"
#include <algorithm>

CDirectDrawSurface7::CDirectDrawSurface7( )
: m_pDirect3D7(NULL), m_pClipper(NULL)
{
}

STDMETHODIMP CDirectDrawSurface7::Initialize( LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	CComQIPtr<IWrapperTable> pWrapperTable = lpDD;	
	Init(pWrapperTable);
	m_pDirect3D7 = (CDirect3D7*)pWrapperTable.p;
	memcpy(&m_DDSurfaceDesc2, lpDDSurfaceDesc, sizeof(DDSURFACEDESC2));
	m_pDirect3D7->AddWrapper((IWrapper*)this);
	bool bCreateDDSurf = true;
	HRESULT hResult = S_OK;
	UINT Width = 0;
	if( lpDDSurfaceDesc->dwFlags & DDSD_WIDTH )
		Width = lpDDSurfaceDesc->dwWidth;
	UINT Height = 0;
	if( lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT )
		Height = lpDDSurfaceDesc->dwHeight;
	if( lpDDSurfaceDesc->dwFlags & DDSD_CAPS )
	{
		D3DPOOL Pool = D3DPOOL_DEFAULT;
		if (lpDDSurfaceDesc->ddsCaps.dwCaps2 == DDSCAPS2_TEXTUREMANAGE)
		{
			Pool = D3DPOOL_MANAGED;
		} else if (lpDDSurfaceDesc->ddsCaps.dwCaps2 == DDSCAPS_SYSTEMMEMORY)
		{
			Pool = D3DPOOL_SYSTEMMEM;
		}
		if ( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE )
		{
			DEBUG_TRACE3("\tPrimary buffer\n");
			// swapchain primarybuffer
			bCreateDDSurf = false; 
		}
		else if ( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER )
		{
			DEBUG_TRACE3("\tBack buffer\n");
			// swapchain backbuffer
			bCreateDDSurf = false; 
		}
		else if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_3DDEVICE ) // DDSCAPS_OFFSCREENPLAIN
		{
			DEBUG_TRACE3("\tRenderTarget\n");
			//hResult = m_pDirect3D7->m_pD3D9Device->CreateRenderTarget(Width, Height, D3DFMT)
		}
		else if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ZBUFFER )
		{
			DEBUG_TRACE3("\tZBuffer\n");
			//hResult = m_pDirect3D7->m_pD3D9Device->CreateDepthStencilSurface()
		}
		else if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_TEXTURE )
		{
			DEBUG_TRACE3("\tTexture\n");
			UINT TextureStage = 0;
			if (lpDDSurfaceDesc->dwFlags & DDSD_TEXTURESTAGE)
				TextureStage = lpDDSurfaceDesc->dwTextureStage;
			if (m_pDirect3D7->m_pD3D9Device)
			{
				hResult = m_pDirect3D7->m_pD3D9Device->CreateTexture(Width, Height, 1, 0, D3DFMT_X8R8G8B8,
					Pool, &m_pTexture, NULL);
				if (SUCCEEDED(hResult))
				{
					hResult = m_pTexture->GetSurfaceLevel(0, &m_pSurface);
				}
			}
		}
	}
	if (bCreateDDSurf)
		hResult = m_pDirect3D7->m_pDDraw7->CreateSurface(lpDDSurfaceDesc, &m_pSurface7, NULL);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::Initialize( LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}


HRESULT WINAPI CDirectDrawSurface7::DebugQueryInterface( void* pv, REFIID riid, LPVOID* ppv, DWORD dw )
{
	if (InlineIsEqualGUID(riid, IID_IDirectDrawSurface7)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDrawSurface7\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDrawSurface4)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDrawSurface4\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDrawSurface3)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDrawSurface3\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDrawSurface2)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDrawSurface2\n");
	} else if (InlineIsEqualGUID(riid, IID_IDirectDrawSurface)) {
		DEBUG_MESSAGE("QueryInterface: IDirectDrawSurface\n");
	} else {
		DEBUG_MESSAGE("QueryInterface: Unknown\n");
	}
	*ppv = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP CDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	DEBUG_TRACE3("%s 7\n", __FUNCTION__);
	lpDDSAttachedSurface->AddRef();
	m_AttachedSurfaces.push_back((CDirectDrawSurface7*)lpDDSAttachedSurface);
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE4 lpDDSAttachedSurface )
{
	DEBUG_TRACE3("%s 4\n", __FUNCTION__);
	lpDDSAttachedSurface->AddRef();
	// TODO: Do query
	m_AttachedSurfaces.push_back((CDirectDrawSurface7*)lpDDSAttachedSurface);
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE3 lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::AddAttachedSurface( LPDIRECTDRAWSURFACE lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::AddOverlayDirtyRect( LPRECT lpRect )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	if( m_DDSurfaceDesc2.dwFlags & DDSD_CAPS)
	{
		DEBUG_TRACE3("%s\n", __FUNCTION__);
		if (m_DDSurfaceDesc2.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE ||
			m_DDSurfaceDesc2.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
		{
			// present image from primary backbuffer
			if (m_pDirect3D7->m_pD3D9Device)
				m_pDirect3D7->m_pD3D9Device->Present(NULL, NULL, NULL, NULL);
			return S_OK;
		}
	}
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	DEBUG_TRACE3("%s 4\n", __FUNCTION__);
	return Blt( lpDestRect, (LPDIRECTDRAWSURFACE7)lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx );
}

STDMETHODIMP CDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE3 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	DEBUG_TRACE3("%s 3\n", __FUNCTION__);
	return Blt( lpDestRect, (LPDIRECTDRAWSURFACE7)lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx );
}

STDMETHODIMP CDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	DEBUG_TRACE3("%s 2\n", __FUNCTION__);
	return Blt( lpDestRect, (LPDIRECTDRAWSURFACE7)lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx );
}

STDMETHODIMP CDirectDrawSurface7::Blt( LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	return Blt( lpDestRect, (LPDIRECTDRAWSURFACE7)lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx );
}

STDMETHODIMP CDirectDrawSurface7::BltBatch( LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE4 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE3 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE2 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTransD )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::BltFast( DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	std::vector<CDirectDrawSurface7*>::const_iterator it;
	it = std::find(m_AttachedSurfaces.begin(), m_AttachedSurfaces.end(), 
		(CDirectDrawSurface7*)lpDDSAttachedSurface);
	if (it != m_AttachedSurfaces.end())
	{
		(*it)->Release();
		m_AttachedSurfaces.erase(it);
		return S_OK;
	}
	return DDERR_SURFACENOTATTACHED;
}

STDMETHODIMP CDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE4 lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE3 lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::DeleteAttachedSurface( DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumAttachedSurfaces( LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpfnCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpfnCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::EnumOverlayZOrders( DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	if( m_DDSurfaceDesc2.dwFlags & DDSD_CAPS && m_DDSurfaceDesc2.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE )
	{
		DEBUG_TRACE3("%s\n", __FUNCTION__);
		// lpDDSurfaceTargetOverride == NULL
		// Do it in Blit
		//if (m_pDirect3D7->m_pD3D9Device)
		//	m_pDirect3D7->m_pD3D9Device->Present(NULL, NULL, NULL, NULL);
		return S_OK;
	}
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE4 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	DEBUG_TRACE3("%s 4\n", __FUNCTION__);
	return Flip( (LPDIRECTDRAWSURFACE7)lpDDSurfaceTargetOverride, dwFlags );
}

STDMETHODIMP CDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE3 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	DEBUG_TRACE3("%s 3\n", __FUNCTION__);
	return Flip( (LPDIRECTDRAWSURFACE7)lpDDSurfaceTargetOverride, dwFlags );
}

STDMETHODIMP CDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE2 lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	DEBUG_TRACE3("%s 2\n", __FUNCTION__);
	return Flip( (LPDIRECTDRAWSURFACE7)lpDDSurfaceTargetOverride, dwFlags );
}

STDMETHODIMP CDirectDrawSurface7::Flip( LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	return Flip( (LPDIRECTDRAWSURFACE7)lpDDSurfaceTargetOverride, dwFlags );
}

STDMETHODIMP CDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS2 lpDDSCaps, LPDIRECTDRAWSURFACE7 FAR *lplpDDAttachedSurface )
{	
	if( m_DDSurfaceDesc2.dwFlags & DDSD_CAPS && m_DDSurfaceDesc2.ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE )
	{
		if( lpDDSCaps->dwCaps = DDSCAPS_BACKBUFFER )
		{
			DEBUG_TRACE3("%s 7\n", __FUNCTION__);
			HRESULT hResult = S_OK;
			hResult = CDirectDrawSurface7::CreateInstance(lplpDDAttachedSurface);
			DDSURFACEDESC2 desc = m_DDSurfaceDesc2;
			desc.ddsCaps = *lpDDSCaps;
			hResult = (*lplpDDAttachedSurface)->Initialize((IDirectDraw*)m_pDirect3D7, &desc);
			return hResult;
		}
	}
	DEBUG_MESSAGE("Not implemented: %s 7\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS2 lpDDSCaps,LPDIRECTDRAWSURFACE4 *lplpDDAttachedSurface )
{
	DEBUG_TRACE3("%s 4\n", __FUNCTION__);
	return GetAttachedSurface(lpDDSCaps, (LPDIRECTDRAWSURFACE7*) lplpDDAttachedSurface);
}

STDMETHODIMP CDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE3 *lplpDDAttachedSurface )
{
	DEBUG_TRACE3("%s 3\n", __FUNCTION__);
	DDSCAPS2 DDSCaps;
	DDSCaps.dwCaps = lpDDSCaps->dwCaps;
	DDSCaps.dwCaps2 = 0;
	DDSCaps.dwCaps3 = 0;
	DDSCaps.dwCaps4 = 0;
	return GetAttachedSurface(&DDSCaps, (LPDIRECTDRAWSURFACE7*) lplpDDAttachedSurface);
}

STDMETHODIMP CDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE2 *lplpDDAttachedSurface )
{
	DEBUG_TRACE3("%s 2\n", __FUNCTION__);
	DDSCAPS2 DDSCaps;
	DDSCaps.dwCaps = lpDDSCaps->dwCaps;
	DDSCaps.dwCaps2 = 0;
	DDSCaps.dwCaps3 = 0;
	DDSCaps.dwCaps4 = 0;
	return GetAttachedSurface(&DDSCaps, (LPDIRECTDRAWSURFACE7*) lplpDDAttachedSurface);
}

STDMETHODIMP CDirectDrawSurface7::GetAttachedSurface( LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE *lplpDDAttachedSurface )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	DDSCAPS2 DDSCaps;
	DDSCaps.dwCaps = lpDDSCaps->dwCaps;
	DDSCaps.dwCaps2 = 0;
	DDSCaps.dwCaps3 = 0;
	DDSCaps.dwCaps4 = 0;
	return GetAttachedSurface(&DDSCaps, (LPDIRECTDRAWSURFACE7*) lplpDDAttachedSurface);
}

STDMETHODIMP CDirectDrawSurface7::GetBltStatus( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetCaps( LPDDSCAPS2 lpDDSCaps )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetCaps( LPDDSCAPS lpDDSCaps )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetClipper( LPDIRECTDRAWCLIPPER FAR *lplpDDClipper )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (m_pClipper)
		m_pClipper->QueryInterface(IID_IDirectDrawClipper, (void**)lplpDDClipper);
	else
		*lplpDDClipper = NULL;
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::GetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetDC( HDC FAR *lphDC )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->GetDC(lphDC);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::GetFlipStatus( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetOverlayPosition( LPLONG lplX, LPLONG lplY )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetPalette( LPDIRECTDRAWPALETTE FAR *lplpDDPalette )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetPixelFormat( LPDDPIXELFORMAT lpDDPixelFormat )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lpDDPixelFormat = m_DDSurfaceDesc2.ddpfPixelFormat;
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::GetSurfaceDesc( LPDDSURFACEDESC2 lpDDSurfaceDesc )
{
	DEBUG_TRACE3("\t%s\n", __FUNCTION__);
	*lpDDSurfaceDesc = m_DDSurfaceDesc2;
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::GetSurfaceDesc( LPDDSURFACEDESC lpDDSurfaceDesc )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::IsLost()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Lock( LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	D3DLOCKED_RECT lockedRect;
	HRESULT hResult = m_pSurface->LockRect(&lockedRect, lpDestRect, dwFlags);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::Lock( LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::ReleaseDC( HDC hDC )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->ReleaseDC(hDC);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::Restore()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetClipper( LPDIRECTDRAWCLIPPER lpDDClipper )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	m_pClipper = (CDirectDrawClipper*)lpDDClipper;
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::SetColorKey( DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetOverlayPosition( LONG lX, LONG lY )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetPalette( LPDIRECTDRAWPALETTE lpDDPalette )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::Unlock( LPRECT lpRect )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->UnlockRect();
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::Unlock( LPVOID lpSurfaceData )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE4 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE3 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE2 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlay( LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayDisplay( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSReference )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE4 lpDDSReference )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE3 lpDDSReference )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSReference )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::UpdateOverlayZOrder( DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::GetDDInterface( LPVOID FAR *lplpDD )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lplpDD = m_pDirect3D7;
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::PageLock( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::PageUnlock( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetSurfaceDesc( LPDDSURFACEDESC2 lpddsd2, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetSurfaceDesc( LPDDSURFACEDESC lpddsd, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetPrivateData( REFGUID guidTag, LPVOID lpData, DWORD cbSize, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->SetPrivateData(guidTag, lpData, cbSize, dwFlags);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::GetPrivateData( REFGUID guidTag, LPVOID lpBuffer, LPDWORD lpcbBufferSize )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->GetPrivateData(guidTag, lpBuffer, lpcbBufferSize);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::FreePrivateData( REFGUID guidTag )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pSurface->FreePrivateData(guidTag);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::GetUniquenessValue( LPDWORD lpValue )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::ChangeUniquenessValue()
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawSurface7::SetPriority( DWORD dwPriority )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pTexture->SetPriority(dwPriority);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::GetPriority( LPDWORD lpdwPriority )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lpdwPriority = m_pSurface->GetPriority();
	return S_OK;
}

STDMETHODIMP CDirectDrawSurface7::SetLOD( DWORD dwMaxLOD )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pTexture->SetLOD(dwMaxLOD);
	return hResult;
}

STDMETHODIMP CDirectDrawSurface7::GetLOD( LPDWORD lpdwMaxLOD )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lpdwMaxLOD = m_pTexture->GetLOD();
	return S_OK;
}
