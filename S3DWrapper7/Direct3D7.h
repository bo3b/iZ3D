/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "stdio.h"
#include <ddraw.h>
#include <d3d.h>
#include "WrapperTable.h"
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CDirectDraw7

class DECLSPEC_UUID("EF93DB2F-0096-4344-96D2-D9AB0D4C7C10")
	CDirect3D7: 
	public CWrapperTable,
	public IDirectDraw7,
	public IDirectDraw4,
	public IDirectDraw2,
	public IDirectDraw,
	public IDirect3D7,
	public IDirect3D3,
	public IDirect3D2,
	public IDirect3D,	// Direct3D 
	public CComCoClass<CDirect3D7>,
	public CComObjectRoot
{
	BEGIN_COM_MAP(CDirect3D7)
		COM_INTERFACE_ENTRY(IWrapperTable)
		COM_INTERFACE_ENTRY_FUNC_BLIND(0, DebugQueryInterface)
		COM_INTERFACE_ENTRY(IDirectDraw7)
		COM_INTERFACE_ENTRY(IDirectDraw4)
		COM_INTERFACE_ENTRY(IDirectDraw2)
		COM_INTERFACE_ENTRY(IDirectDraw)
		COM_INTERFACE_ENTRY(IDirect3D7)
		COM_INTERFACE_ENTRY(IDirect3D3)
		COM_INTERFACE_ENTRY(IDirect3D2)
		COM_INTERFACE_ENTRY(IDirect3D)
	END_COM_MAP()

public:
	CDirect3D7();
	void Init(IDirectDraw7 *pDDraw7);
	void Init(IDirectDraw *pDDraw);
	HRESULT InitializeD3D9();
	HRESULT InitializeD3D9Device();
	static HRESULT WINAPI DebugQueryInterface(void* pv, REFIID riid, LPVOID* ppv, DWORD dw);

	/*** IDirectDraw methods ***/
	STDMETHODIMP Compact();
	STDMETHODIMP CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter);
	STDMETHODIMP CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpDDColorArray, LPDIRECTDRAWPALETTE FAR *lplpDDPalette, IUnknown FAR *pUnkOuter );
	STDMETHODIMP CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPDIRECTDRAWSURFACE7 FAR *lplpDDSurface, IUnknown FAR *pUnkOuter);
	STDMETHODIMP CreateSurface(LPDDSURFACEDESC2 lpDDSurfaceDesc2,LPDIRECTDRAWSURFACE4 *lplpDDSurface,IUnknown *pUnkOuter);
	STDMETHODIMP CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE *lplpDDSurface,IUnknown *pUnkOuter);
	STDMETHODIMP DuplicateSurface(LPDIRECTDRAWSURFACE7 lpDDSurface, LPDIRECTDRAWSURFACE7 FAR *lplpDupDDSurface);
	STDMETHODIMP DuplicateSurface(LPDIRECTDRAWSURFACE4 lpDDSurface,LPDIRECTDRAWSURFACE4 *lplpDupDDSurface);
	STDMETHODIMP DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface,LPDIRECTDRAWSURFACE *lplpDupDDSurface);
	STDMETHODIMP EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback);
	STDMETHODIMP EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumModesCallback);
	STDMETHODIMP EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext, LPDDENUMSURFACESCALLBACK7 lpEnumSurfacesCallback);
	STDMETHODIMP EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC2 lpDDSD2, LPVOID lpContext, LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback);
	STDMETHODIMP EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC lpDDSD, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback);
	STDMETHODIMP FlipToGDISurface();
	STDMETHODIMP GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps);
	STDMETHODIMP GetDisplayMode(LPDDSURFACEDESC2 lpDDSurfaceDesc2);
	STDMETHODIMP GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc);
	STDMETHODIMP GetFourCCCodes(LPDWORD lpNumCodes,	LPDWORD lpCodes);
	STDMETHODIMP GetGDISurface(LPDIRECTDRAWSURFACE7 FAR *lplpGDIDDSSurface);
	STDMETHODIMP GetGDISurface(LPDIRECTDRAWSURFACE4 *lplpGDIDDSSurface);
	STDMETHODIMP GetGDISurface(LPDIRECTDRAWSURFACE *lplpGDIDDSSurface);
	STDMETHODIMP GetMonitorFrequency(LPDWORD lpdwFrequency);
	STDMETHODIMP GetScanLine(LPDWORD lpdwScanLine);
	STDMETHODIMP GetVerticalBlankStatus(LPBOOL lpbIsInVB);
	STDMETHODIMP Initialize(GUID FAR *lpGUID);
	STDMETHODIMP Initialize(REFCLSID GUID);
	STDMETHODIMP RestoreDisplayMode();
	STDMETHODIMP SetCooperativeLevel(HWND hWnd, DWORD dwFlags);
	STDMETHODIMP SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwRefreshRate, DWORD dwFlags);
	STDMETHODIMP SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP);
	STDMETHODIMP WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent);
	/*** Added in the v2 interface ***/
	STDMETHODIMP GetAvailableVidMem(LPDDSCAPS2 lpDDSCaps2, LPDWORD lpdwTotal, LPDWORD lpdwFree);
	STDMETHODIMP GetAvailableVidMem(LPDDSCAPS lpDDSCaps, LPDWORD lpdwTotal, LPDWORD lpdwFree);
	/*** Added in the V4 Interface ***/
	STDMETHODIMP GetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE7 * lpDDS);
	STDMETHODIMP GetSurfaceFromDC(HDC hdc, LPDIRECTDRAWSURFACE4 * lpDDS);
	STDMETHODIMP RestoreAllSurfaces();
	STDMETHODIMP TestCooperativeLevel();
	STDMETHODIMP GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2 lpdddi, DWORD dwFlags);
	STDMETHODIMP GetDeviceIdentifier(LPDDDEVICEIDENTIFIER lpdddi, DWORD dwFlags);
	STDMETHODIMP StartModeTest(LPSIZE   lpModesToTest, DWORD    dwNumEntries, DWORD    dwFlags);
	STDMETHODIMP EvaluateMode(DWORD  dwFlags, DWORD  *pSecondsUntilTimeout);

	STDMETHODIMP CreateLight(LPDIRECT3DLIGHT* lplpDirect3DLight, IUnknown* pUnkOuter);
	STDMETHODIMP CreateMaterial(LPDIRECT3DMATERIAL3* lplpDirect3DMaterial3, IUnknown* pUnkOuter);
	STDMETHODIMP CreateMaterial(LPDIRECT3DMATERIAL2* lplpDirect3DMaterial2, IUnknown* pUnkOuter);
	STDMETHODIMP CreateMaterial(LPDIRECT3DMATERIAL* lplpDirect3DMaterial, IUnknown* pUnkOuter);
	STDMETHODIMP CreateViewport(LPDIRECT3DVIEWPORT3* lplpD3DViewport3, IUnknown* pUnkOuter);
	STDMETHODIMP CreateViewport(LPDIRECT3DVIEWPORT2* lplpD3DViewport2, IUnknown* pUnkOuter);
	STDMETHODIMP CreateViewport(LPDIRECT3DVIEWPORT* lplpD3DViewport, IUnknown* pUnkOuter);
	STDMETHODIMP FindDevice(LPD3DFINDDEVICESEARCH lpD3DFDS, LPD3DFINDDEVICERESULT lpD3DFDR);

	/*** IDirect3D7 methods ***/
	STDMETHODIMP EnumDevices(LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, LPVOID lpUserArg);
	STDMETHODIMP EnumDevices(LPD3DENUMDEVICESCALLBACK lpEnumDevicesCallback, LPVOID lpUserArg);
	STDMETHODIMP CreateDevice(REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS, LPDIRECT3DDEVICE7 * lplpD3DDevice);
	STDMETHODIMP CreateDevice(REFCLSID rclsid,LPDIRECTDRAWSURFACE4 lpDDS,LPDIRECT3DDEVICE3 * lplpD3DDevice, LPUNKNOWN pUnkOuter);
	STDMETHODIMP CreateDevice(REFCLSID rclsid,LPDIRECTDRAWSURFACE lpDDS,LPDIRECT3DDEVICE2 * lplpD3DDevice);
	STDMETHODIMP CreateVertexBuffer(LPD3DVERTEXBUFFERDESC lpVBDesc, LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, DWORD dwFlags);
	STDMETHODIMP CreateVertexBuffer(LPD3DVERTEXBUFFERDESC lpVBDesc,LPDIRECT3DVERTEXBUFFER* lplpD3DVertexBuffer, DWORD dwFlags, LPUNKNOWN pUnkOuter);
	STDMETHODIMP EnumZBufferFormats(REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, LPVOID   lpContext);
	STDMETHODIMP EvictManagedTextures();

	CComPtr<IDirectDraw>		m_pDDraw;
	CComQIPtr<IDirectDraw2>		m_pDDraw2;
	CComQIPtr<IDirectDraw4>		m_pDDraw4;
	CComQIPtr<IDirect3D3>		m_pD3D3;
	CComPtr<IDirectDraw7>		m_pDDraw7;
	CComQIPtr<IDirect3D7>		m_pD3D7;
	CComPtr<IDirect3D9>			m_pD3D9;
	CComPtr<IDirect3DDevice9>	m_pD3D9Device;

	UINT		m_nAdapter;
	D3DDEVTYPE	m_DeviceType;
	HWND		m_hFocusWindow;
	DWORD		m_dwBehaviorFlags;
	bool		m_bDirectDrawMode;	 
	D3DPRESENT_PARAMETERS	m_PresentationParameters;
};
