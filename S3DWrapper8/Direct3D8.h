/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"       // main symbols
#include "wrapper.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CDirect3D8

class CDirect3D8: 
	public CWrapper<IDirect3D8, IDirect3D9>,
	public CComCoClass<CDirect3D8>
{
	BEGIN_COM_MAP(CDirect3D8)
		COM_INTERFACE_ENTRY(IDirect3D8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	IMPLEMENT_INTERNAL_RELEASE_STRONG();

	std::vector<D3DDISPLAYMODE> m_AdapterModes;
	int m_Adapter;

	void BuildAdaperModesList(int adapter);

public:
	CDirect3D8();

	// IDirect3D8
	STDMETHODIMP RegisterSoftwareDevice(void* pInitializeFunction);
    STDMETHODIMP_(UINT) GetAdapterCount();
    STDMETHODIMP GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier);
    STDMETHODIMP_(UINT) GetAdapterModeCount(UINT Adapter);
    STDMETHODIMP EnumAdapterModes(UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode);
    STDMETHODIMP GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode);
    STDMETHODIMP CheckDeviceType(UINT Adapter,D3DDEVTYPE CheckType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
    STDMETHODIMP CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
    STDMETHODIMP CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType);
    STDMETHODIMP CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
    STDMETHODIMP GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps);

	STDMETHODIMP_(HMONITOR) GetAdapterMonitor(UINT Adapter);
    STDMETHODIMP CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS8* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface);
};

