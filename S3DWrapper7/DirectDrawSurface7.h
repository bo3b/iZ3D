/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Direct3D7.h"
#include "Wrapper.h"
#include <vector>

class CDirectDrawClipper;
class CDirect3DDevice7;

class DECLSPEC_UUID("BBCEB070-E624-4575-A7EF-182EBD65AFC8")
	CDirectDrawSurface7: 
	public CWrapper,
	public IDirectDrawSurface7,
	public IDirectDrawSurface4,
	public IDirectDrawSurface3,
	public IDirectDrawSurface2,
	public IDirectDrawSurface,
	public CComCoClass<CDirectDrawSurface7>
{
private:
	friend CDirect3DDevice7;
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;

	BEGIN_COM_MAP(CDirectDrawSurface7)
		COM_INTERFACE_ENTRY(IWrapper)
		//COM_INTERFACE_ENTRY_FUNC_BLIND(0, DebugQueryInterface)
		COM_INTERFACE_ENTRY(IDirectDrawSurface7)
		COM_INTERFACE_ENTRY(IDirectDrawSurface4)
		COM_INTERFACE_ENTRY(IDirectDrawSurface3)
		COM_INTERFACE_ENTRY(IDirectDrawSurface2)
		COM_INTERFACE_ENTRY(IDirectDrawSurface)
	END_COM_MAP()

public:
	CDirectDrawSurface7();
	static HRESULT WINAPI DebugQueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD dw);

	/*** IDirectDrawSurface methods ***/
	STDMETHODIMP Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpDDSurfaceDesc);
	STDMETHODIMP Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc);
	STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
	STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE4 lpDDSAttachedSurface);
	STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE3 lpDDSAttachedSurface);
	STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface);
	STDMETHODIMP AddAttachedSurface(LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
	STDMETHODIMP AddOverlayDirtyRect(LPRECT lpRect);
	STDMETHODIMP Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);
	STDMETHODIMP Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE4 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);
	STDMETHODIMP Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE3 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);
	STDMETHODIMP Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);
	STDMETHODIMP Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);
	STDMETHODIMP BltBatch(LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags);
	STDMETHODIMP BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface,
		LPRECT lpSrcRect, DWORD dwTrans);
	STDMETHODIMP BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE4 lpDDSrcSurface,
		LPRECT lpSrcRect, DWORD dwTrans);
	STDMETHODIMP BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE3 lpDDSrcSurface,
		LPRECT lpSrcRect, DWORD dwTrans);
	STDMETHODIMP BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE2 lpDDSrcSurface,
		LPRECT lpSrcRect, DWORD dwTransD);
	STDMETHODIMP BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface,
		LPRECT lpSrcRect, DWORD dwTrans);
	STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
	STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE4 lpDDSAttachedSurface);
	STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE3 lpDDSAttachedSurface);
	STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface);
	STDMETHODIMP DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface);
	STDMETHODIMP EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback);
	STDMETHODIMP EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback);
	STDMETHODIMP EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback);
	STDMETHODIMP EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpfnCallback);
	STDMETHODIMP EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpfnCallback);
	STDMETHODIMP EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback);
	STDMETHODIMP Flip(LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, DWORD dwFlags);
	STDMETHODIMP Flip(LPDIRECTDRAWSURFACE4 lpDDSurfaceTargetOverride, DWORD dwFlags);
	STDMETHODIMP Flip(LPDIRECTDRAWSURFACE3 lpDDSurfaceTargetOverride, DWORD dwFlags);
	STDMETHODIMP Flip(LPDIRECTDRAWSURFACE2 lpDDSurfaceTargetOverride, DWORD dwFlags);
	STDMETHODIMP Flip(LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags);
	STDMETHODIMP GetAttachedSurface(LPDDSCAPS2 lpDDSCaps, LPDIRECTDRAWSURFACE7 FAR *lplpDDAttachedSurface);
	STDMETHODIMP GetAttachedSurface(LPDDSCAPS2 lpDDSCaps,LPDIRECTDRAWSURFACE4 *lplpDDAttachedSurface);
	STDMETHODIMP GetAttachedSurface(LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE3 *lplpDDAttachedSurface);
	STDMETHODIMP GetAttachedSurface(LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE2 *lplpDDAttachedSurface);
	STDMETHODIMP GetAttachedSurface(LPDDSCAPS lpDDSCaps,LPDIRECTDRAWSURFACE *lplpDDAttachedSurface);
	STDMETHODIMP GetBltStatus(DWORD dwFlags);
	STDMETHODIMP GetCaps(LPDDSCAPS2 lpDDSCaps);
	STDMETHODIMP GetCaps(LPDDSCAPS lpDDSCaps);
	STDMETHODIMP GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper );
	STDMETHODIMP GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey);
	STDMETHODIMP GetDC(HDC FAR *lphDC);
	STDMETHODIMP GetFlipStatus(DWORD dwFlags);
	STDMETHODIMP GetOverlayPosition(LPLONG lplX, LPLONG lplY);
	STDMETHODIMP GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette);
	STDMETHODIMP GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat);
	STDMETHODIMP GetSurfaceDesc(LPDDSURFACEDESC2 lpDDSurfaceDesc);
	STDMETHODIMP GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc);
	STDMETHODIMP IsLost();
	STDMETHODIMP Lock(LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent);
	STDMETHODIMP Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent);
	STDMETHODIMP ReleaseDC(HDC hDC);
	STDMETHODIMP Restore();
	STDMETHODIMP SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper);
	STDMETHODIMP SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey);
	STDMETHODIMP SetOverlayPosition(LONG lX, LONG lY);
	STDMETHODIMP SetPalette(LPDIRECTDRAWPALETTE lpDDPalette);
	STDMETHODIMP Unlock(LPRECT lpRect);
	STDMETHODIMP Unlock(LPVOID lpSurfaceData);
	STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpDDDestSurface, LPRECT lpDestRect, 
		DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx);
	STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE4 lpDDDestSurface, LPRECT lpDestRect, 
		DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx);
	STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE3 lpDDDestSurface, LPRECT lpDestRect, 
		DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx);
	STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE2 lpDDDestSurface, LPRECT lpDestRect, 
		DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx);
	STDMETHODIMP UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, 
		DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx);
	STDMETHODIMP UpdateOverlayDisplay(DWORD dwFlags);
	STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpDDSReference);
	STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE4 lpDDSReference);
	STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE3 lpDDSReference);
	STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSReference);
	STDMETHODIMP UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference);
	/*** Added in the v2 interface ***/
	STDMETHODIMP GetDDInterface(LPVOID FAR *lplpDD);
	STDMETHODIMP PageLock(DWORD dwFlags);
	STDMETHODIMP PageUnlock(DWORD dwFlags);
	/*** Added in the v3 interface ***/
	STDMETHODIMP SetSurfaceDesc(LPDDSURFACEDESC2 lpddsd2, DWORD dwFlags);
	STDMETHODIMP SetSurfaceDesc(LPDDSURFACEDESC lpddsd, DWORD dwFlags);
	/*** Added in the v4 interface ***/
	STDMETHODIMP SetPrivateData(REFGUID guidTag, LPVOID  lpData, DWORD   cbSize, DWORD   dwFlags);
	STDMETHODIMP GetPrivateData(REFGUID guidTag, LPVOID  lpBuffer, LPDWORD lpcbBufferSize);
	STDMETHODIMP FreePrivateData(REFGUID guidTag);
	STDMETHODIMP GetUniquenessValue(LPDWORD lpValue);
	STDMETHODIMP ChangeUniquenessValue();
	/*** Moved Texture7 methods here ***/
	STDMETHODIMP SetPriority(DWORD dwPriority);
	STDMETHODIMP GetPriority(LPDWORD lpdwPriority);
	STDMETHODIMP SetLOD(DWORD dwMaxLOD);
	STDMETHODIMP GetLOD(LPDWORD lpdwMaxLOD);

private:
	CDirect3D7*		m_pDirect3D7;
	DDSURFACEDESC2	m_DDSurfaceDesc2;

	CComPtr<IDirect3DSurface9>	m_pSurface;
	std::vector<CDirectDrawSurface7*>	m_AttachedSurfaces;
	CComPtr<IDirect3DTexture9>	m_pTexture;
	CDirectDrawClipper*	m_pClipper;

	CComPtr<IDirectDrawSurface7>	m_pSurface7;
	CComPtr<IDirectDrawSurface4>	m_pSurface4;
	CComPtr<IDirectDrawSurface3>	m_pSurface3;
	CComPtr<IDirectDrawSurface2>	m_pSurface2;
	CComPtr<IDirectDrawSurface2>	m_pSurface1;
};
