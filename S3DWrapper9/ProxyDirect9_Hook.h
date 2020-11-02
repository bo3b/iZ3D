#include "stdafx.h"
#include "Hook.h"

namespace IDirect3D9MethodNames
{
	// IUnknown methods
	PROXYSTDMETHOD(QueryInterface)(IDirect3D9* pDirect, REFIID riid, void** ppvObj);
	PROXYSTDMETHOD_(ULONG,AddRef)(IDirect3D9* pDirect);
	PROXYSTDMETHOD_(ULONG,Release)(IDirect3D9* pDirect);

	// IDirect3D9 methods
	PROXYSTDMETHOD(RegisterSoftwareDevice)(IDirect3D9* pDirect, void* pInitializeFunction);
	PROXYSTDMETHOD_(UINT, GetAdapterCount)(IDirect3D9* pDirect);
	PROXYSTDMETHOD(GetAdapterIdentifier)(IDirect3D9* pDirect, UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
	PROXYSTDMETHOD_(UINT, GetAdapterModeCount)(IDirect3D9* pDirect, UINT Adapter, D3DFORMAT Format);
	PROXYSTDMETHOD(EnumAdapterModes)(IDirect3D9* pDirect, UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
	PROXYSTDMETHOD(GetAdapterDisplayMode)(IDirect3D9* pDirect, UINT Adapter, D3DDISPLAYMODE* pMode);
	PROXYSTDMETHOD(CheckDeviceType)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
	PROXYSTDMETHOD(CheckDeviceFormat)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
	PROXYSTDMETHOD(CheckDeviceMultiSampleType)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
	PROXYSTDMETHOD(CheckDepthStencilMatch)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
	PROXYSTDMETHOD(CheckDeviceFormatConversion)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
	PROXYSTDMETHOD(GetDeviceCaps)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
	PROXYSTDMETHOD_(HMONITOR, GetAdapterMonitor)(IDirect3D9* pDirect, UINT Adapter);
	PROXYSTDMETHOD(CreateDevice)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

	// IDirect3D9Ex methods
	PROXYSTDMETHOD_(UINT, GetAdapterModeCountEx)(IDirect3D9Ex* pDirect, UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter);
	PROXYSTDMETHOD(EnumAdapterModesEx)(IDirect3D9Ex* pDirect, UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode);
	PROXYSTDMETHOD(GetAdapterDisplayModeEx)(IDirect3D9Ex* pDirect, UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);
	PROXYSTDMETHOD(CreateDeviceEx)(IDirect3D9Ex* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface);
	PROXYSTDMETHOD(GetAdapterLUID)(IDirect3D9Ex* pDirect, UINT Adapter, LUID * pLUID);
}
