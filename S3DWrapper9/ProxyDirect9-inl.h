#pragma once

#ifndef OUTPUT_LIBRARY

#include "Trace.h"
#include "GlobalData.h"
#include "ProxyDirect9.h"

#undef	PROXYMETHOD
#undef	PROXYMETHOD_
#undef	CALLORIGINAL
#undef	CALLORIGINALEX
#define PROXYMETHOD(a)			inline HRESULT	ProxyDirect9::a 
#define PROXYMETHOD_(type, a)	inline type		ProxyDirect9::a
#define CALLORIGINAL(a)			(m_pHookedInterface->*m_fpOriginal.a)
#define CALLORIGINALEX(a)		(((IDirect3D9Ex*)m_pHookedInterface)->*m_fpOriginal.a)

// IUnknown methods 
PROXYMETHOD(QueryInterface)(REFIID riid, void** ppvObj)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(QueryInterface)(riid, ppvObj);
}

PROXYMETHOD_(ULONG,AddRef)() 
{
	CHECKMODIFICATION();
	return CALLORIGINAL(AddRef)();
}

PROXYMETHOD_(ULONG,Release)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(Release)();
}

// IDirect3D9 methods
PROXYMETHOD(RegisterSoftwareDevice)(void* pInitializeFunction)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(RegisterSoftwareDevice)(pInitializeFunction);
}

PROXYMETHOD_(UINT, GetAdapterCount)()
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAdapterCount)();
}

PROXYMETHOD(GetAdapterIdentifier)(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAdapterIdentifier)(Adapter, Flags, pIdentifier);
}

PROXYMETHOD_(UINT, GetAdapterModeCount)(UINT Adapter, D3DFORMAT Format)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAdapterModeCount)(Adapter, Format);
}

PROXYMETHOD(EnumAdapterModes)(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(EnumAdapterModes)(Adapter, Format, Mode, pMode);
}

PROXYMETHOD(GetAdapterDisplayMode)(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAdapterDisplayMode)(Adapter, pMode);
}

PROXYMETHOD(CheckDeviceType)(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CheckDeviceType)(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

PROXYMETHOD(CheckDeviceFormat)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CheckDeviceFormat)(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

PROXYMETHOD(CheckDeviceMultiSampleType)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CheckDeviceMultiSampleType)(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

PROXYMETHOD(CheckDepthStencilMatch)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CheckDepthStencilMatch)(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

PROXYMETHOD(CheckDeviceFormatConversion)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CheckDeviceFormatConversion)(Adapter, DeviceType, SourceFormat, TargetFormat);
}

PROXYMETHOD(GetDeviceCaps)(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetDeviceCaps)(Adapter, DeviceType, pCaps);
}

PROXYMETHOD_(HMONITOR, GetAdapterMonitor)(UINT Adapter)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(GetAdapterMonitor)(Adapter);
}

PROXYMETHOD(CreateDevice)(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	CHECKMODIFICATION();
	return CALLORIGINAL(CreateDevice)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}

// IDirect3D9Ex methods
PROXYMETHOD_(UINT, GetAdapterModeCountEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetAdapterModeCountEx)(Adapter, pFilter);
}

PROXYMETHOD(EnumAdapterModesEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(EnumAdapterModesEx)(Adapter, pFilter, Mode, pMode);
}

PROXYMETHOD(GetAdapterDisplayModeEx)(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetAdapterDisplayModeEx)(Adapter, pMode, pRotation);
}

PROXYMETHOD(CreateDeviceEx)(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(CreateDeviceEx)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
}

PROXYMETHOD(GetAdapterLUID)(UINT Adapter, LUID * pLUID)
{
	CHECKMODIFICATION();
	return CALLORIGINALEX(GetAdapterLUID)(Adapter, pLUID);
}


#endif
