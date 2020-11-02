/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once
#include "ProxyDirect9.h"

inline UINT GetAdapterNumber(UINT Adapter)
{
	// TODO: Correct number if reduce count
	return Adapter;
}

inline UINT GetOrigAdapterNumber(UINT Adapter)
{
	// TODO: Correct number if reduce count
	return Adapter;
}

class CDirect3D9: public IDirect3D9Ex
{
public:
	CDirect3D9() : m_RefCount(0) {}
	~CDirect3D9();
	
	//--- RouterType = ROUTER_TYPE_HOOK case functions ----
	void HookIDirect3D9All();
	void UnHookIDirect3D9All();
	
	ULONG	m_RefCount;
	
	//--- Use this pointers for original IDirect3D9Ex function calling ---
	ProxyDirect9 m_Direct3D9;

	/*** IUnknown methods ***/
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	/*** IDirect3D methods ***/
	STDMETHODIMP RegisterSoftwareDevice(void* pInitializeFunction) 
	{
		return m_Direct3D9.RegisterSoftwareDevice(pInitializeFunction);
	}
	STDMETHODIMP_(UINT) GetAdapterCount() 
	{
		// TODO: reduce number by front panel count
		if(gInfo.HideAdditionalAdapters)	
			return 1;
		UINT cnt = m_Direct3D9.GetAdapterCount();
		DEBUG_TRACE2(_T("IDirect3D9::GetAdapterCount() = %d\n"), cnt);
		return cnt;
	}
	STDMETHODIMP GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) 
	{
		return m_Direct3D9.GetAdapterIdentifier(GetAdapterNumber(Adapter), Flags, pIdentifier);
	}
	STDMETHODIMP_(UINT) GetAdapterModeCount(UINT Adapter,D3DFORMAT Format) 
	{
		return m_Direct3D9.GetAdapterModeCount(GetAdapterNumber(Adapter), Format);
	}
	STDMETHODIMP EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) 
	{
		HRESULT hr = m_Direct3D9.EnumAdapterModes(GetAdapterNumber(Adapter), Format, Mode, pMode);
		return hr;
	}
	STDMETHODIMP GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode) 
	{
		HRESULT hResult;
		if (gInfo.SpanModeOn)
		{
			NSCALL_TRACE1(m_Direct3D9.GetAdapterDisplayMode(GetAdapterNumber(Adapter), pMode), 
				DEBUG_MESSAGE(_T("GetAdapterDisplayMode(Adapter = %d, pMode: %s)"), 
				GetAdapterNumber(Adapter), GetDisplayModeString(pMode)));
			pMode->Width /= 2;
			DEBUG_TRACE1(_T("Changed to: %s\n"), GetDisplayModeString(pMode));
		} else
		{
			NSCALL_TRACE2(m_Direct3D9.GetAdapterDisplayMode(GetAdapterNumber(Adapter), pMode), 
				DEBUG_MESSAGE(_T("GetAdapterDisplayMode(Adapter = %d, pMode: %s)"), 
				GetAdapterNumber(Adapter), GetDisplayModeString(pMode)));
		}
		return hResult;
	}
	STDMETHODIMP CheckDeviceType(UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) 
	{
		return m_Direct3D9.CheckDeviceType(GetAdapterNumber(Adapter), DevType, AdapterFormat, BackBufferFormat, bWindowed);
	}
	STDMETHODIMP CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) 
	{
		return m_Direct3D9.CheckDeviceFormat(GetAdapterNumber(Adapter), DeviceType, AdapterFormat, Usage, RType, CheckFormat);
	}
	STDMETHODIMP CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) 
	{
		return m_Direct3D9.CheckDeviceMultiSampleType(GetAdapterNumber(Adapter), DeviceType, SurfaceFormat, Windowed, 
			MultiSampleType, pQualityLevels);
	}
	STDMETHODIMP CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) 
	{
		return m_Direct3D9.CheckDepthStencilMatch(GetAdapterNumber(Adapter), DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
	}
	STDMETHODIMP CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) 
	{
		return m_Direct3D9.CheckDeviceFormatConversion(GetAdapterNumber(Adapter), DeviceType, SourceFormat, TargetFormat);
	}
	STDMETHODIMP GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) 
	{
		HRESULT hResult = S_OK;
		NSCALL_TRACE2(m_Direct3D9.GetDeviceCaps(GetAdapterNumber(Adapter), DeviceType, pCaps),
			DEBUG_MESSAGE(_T("GetDeviceCaps(Adapter = %d, DeviceType = %s, pCaps = %p)"), 
			GetAdapterNumber(Adapter), GetDeviceTypeString(DeviceType), pCaps));
		return hResult;
	}
	STDMETHODIMP_(HMONITOR) GetAdapterMonitor(UINT Adapter) 
	{
		return m_Direct3D9.GetAdapterMonitor(GetAdapterNumber(Adapter));
	}

	void DoInitialize( IDirect3D9* pReal, PROXY_EXMODE exMode );
	void PrepareDevice( UINT Adapter );
	HMODULE GetCallingModule( LPCVOID pCaller );

	STDMETHODIMP CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
		DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
		IDirect3DDevice9** ppReturnedDeviceInterface);

	/*** IDirect3D9Ex methods ***/
	STDMETHODIMP_(UINT) GetAdapterModeCountEx( UINT Adapter,CONST D3DDISPLAYMODEFILTER* pFilter )
	{
		return m_Direct3D9.GetAdapterModeCountEx(GetAdapterNumber(Adapter), pFilter);
	}
	STDMETHODIMP EnumAdapterModesEx( UINT Adapter,CONST D3DDISPLAYMODEFILTER* pFilter,UINT Mode,D3DDISPLAYMODEEX* pMode)
	{
		return m_Direct3D9.EnumAdapterModesEx(GetAdapterNumber(Adapter), pFilter, Mode, pMode);
	}
	STDMETHODIMP GetAdapterDisplayModeEx( UINT Adapter,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
	{
		return m_Direct3D9.GetAdapterDisplayModeEx(GetAdapterNumber(Adapter), pMode, pRotation);
	}
	STDMETHODIMP CreateDeviceEx( UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,
		D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode,
		IDirect3DDevice9Ex** ppReturnedDeviceInterface);
	STDMETHODIMP GetAdapterLUID( UINT Adapter,LUID * pLUID)
	{
		return m_Direct3D9.GetAdapterLUID(GetAdapterNumber(Adapter), pLUID);
	}
};
