/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "D3DInterfaceMethods.h"
#include "ProxyBase.h"

namespace IDirect3D9MethodNames
{
	#define PROXYDIRECTMETHODTYPE(a)			typedef HRESULT	 (__stdcall IDirect3D9::*a##_t)
	#define PROXYDIRECTMETHODTYPE_(type, a)		typedef type     (__stdcall IDirect3D9::*a##_t)

	// IUnknown methods
	PROXYDIRECTMETHODTYPE(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYDIRECTMETHODTYPE_(ULONG,AddRef)();
	PROXYDIRECTMETHODTYPE_(ULONG,Release)();

	// IDirect3D9 methods
	PROXYDIRECTMETHODTYPE(RegisterSoftwareDevice)(void* pInitializeFunction);
	PROXYDIRECTMETHODTYPE_(UINT, GetAdapterCount)();
	PROXYDIRECTMETHODTYPE(GetAdapterIdentifier)(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
	PROXYDIRECTMETHODTYPE_(UINT, GetAdapterModeCount)(UINT Adapter, D3DFORMAT Format);
	PROXYDIRECTMETHODTYPE(EnumAdapterModes)(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
	PROXYDIRECTMETHODTYPE(GetAdapterDisplayMode)(UINT Adapter, D3DDISPLAYMODE* pMode);
	PROXYDIRECTMETHODTYPE(CheckDeviceType)(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
	PROXYDIRECTMETHODTYPE(CheckDeviceFormat)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
	PROXYDIRECTMETHODTYPE(CheckDeviceMultiSampleType)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
	PROXYDIRECTMETHODTYPE(CheckDepthStencilMatch)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
	PROXYDIRECTMETHODTYPE(CheckDeviceFormatConversion)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
	PROXYDIRECTMETHODTYPE(GetDeviceCaps)(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
	PROXYDIRECTMETHODTYPE_(HMONITOR, GetAdapterMonitor)(UINT Adapter);
	PROXYDIRECTMETHODTYPE(CreateDevice)( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
	
	#define PROXYDIRECTEXMETHODTYPE(a)			typedef HRESULT	 (__stdcall IDirect3D9Ex::*a##_t)
	#define PROXYDIRECTEXMETHODTYPE_(type, a)	typedef type     (__stdcall IDirect3D9Ex::*a##_t)

	// IDirect3D9Ex methods
	PROXYDIRECTEXMETHODTYPE_(UINT, GetAdapterModeCountEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter);
	PROXYDIRECTEXMETHODTYPE(EnumAdapterModesEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode);
	PROXYDIRECTEXMETHODTYPE(GetAdapterDisplayModeEx)(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);
	PROXYDIRECTEXMETHODTYPE(CreateDeviceEx)(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface);
	PROXYDIRECTEXMETHODTYPE(GetAdapterLUID)(UINT Adapter, LUID * pLUID);

	#define PROXYDIRECTMETHOD(a)			a##_t a
	#define PROXYDIRECTMETHOD_(type, a)		a##_t a

	union FunctionList
	{
		static const int MAX_METHODS    = _MaxMethods_;
		static const int MAX_METHODS_EX = _MaxMethodsEx_;

		FunctionList()
		{
			ZeroMemory(func, sizeof(func));
		}

		struct
		{
			// IUnknown methods
			PROXYDIRECTMETHOD(QueryInterface);
			PROXYDIRECTMETHOD_(ULONG,AddRef);
			PROXYDIRECTMETHOD_(ULONG,Release);

			// IDirect3D9 methods
			PROXYDIRECTMETHOD(RegisterSoftwareDevice);
			PROXYDIRECTMETHOD_(UINT, GetAdapterCount);
			PROXYDIRECTMETHOD(GetAdapterIdentifier);
			PROXYDIRECTMETHOD_(UINT, GetAdapterModeCount);
			PROXYDIRECTMETHOD(EnumAdapterModes);
			PROXYDIRECTMETHOD(GetAdapterDisplayMode);
			PROXYDIRECTMETHOD(CheckDeviceType);
			PROXYDIRECTMETHOD(CheckDeviceFormat);
			PROXYDIRECTMETHOD(CheckDeviceMultiSampleType);
			PROXYDIRECTMETHOD(CheckDepthStencilMatch);
			PROXYDIRECTMETHOD(CheckDeviceFormatConversion);
			PROXYDIRECTMETHOD(GetDeviceCaps);
			PROXYDIRECTMETHOD_(HMONITOR, GetAdapterMonitor);
			PROXYDIRECTMETHOD(CreateDevice);

			// IDirect3D9Ex methods
			PROXYDIRECTMETHOD_(UINT, GetAdapterModeCountEx);
			PROXYDIRECTMETHOD(EnumAdapterModesEx);
			PROXYDIRECTMETHOD(GetAdapterDisplayModeEx);
			PROXYDIRECTMETHOD(CreateDeviceEx);
			PROXYDIRECTMETHOD(GetAdapterLUID);
		};

		static const TCHAR* GetFunctionName(int id)
		{
			return GetDirect3D9MethodName(id);
		}

		void* func[MAX_METHODS_EX];
	};
}

class ProxyDirect9 : public ProxyBase<IDirect3D9, IDirect3D9MethodNames::FunctionList>
{
public:
	operator IDirect3D9Ex*() const { return (IDirect3D9Ex*)(IDirect3D9*)m_pHookedInterface; }

public:
	#undef	PROXYDIRECTMETHOD
	#undef	PROXYDIRECTMETHOD_
	#define PROXYDIRECTMETHOD(a)		HRESULT a 
	#define PROXYDIRECTMETHOD_(type, a)	type    a
	
	// IUnknown methods
	PROXYDIRECTMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	PROXYDIRECTMETHOD_(ULONG,AddRef)();
	PROXYDIRECTMETHOD_(ULONG,Release)();

	// IDirect3D9 methods
	PROXYDIRECTMETHOD(RegisterSoftwareDevice)(void* pInitializeFunction);
	PROXYDIRECTMETHOD_(UINT, GetAdapterCount)();
	PROXYDIRECTMETHOD(GetAdapterIdentifier)(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
	PROXYDIRECTMETHOD_(UINT, GetAdapterModeCount)(UINT Adapter, D3DFORMAT Format);
	PROXYDIRECTMETHOD(EnumAdapterModes)(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
	PROXYDIRECTMETHOD(GetAdapterDisplayMode)(UINT Adapter, D3DDISPLAYMODE* pMode);
	PROXYDIRECTMETHOD(CheckDeviceType)(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
	PROXYDIRECTMETHOD(CheckDeviceFormat)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
	PROXYDIRECTMETHOD(CheckDeviceMultiSampleType)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
	PROXYDIRECTMETHOD(CheckDepthStencilMatch)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
	PROXYDIRECTMETHOD(CheckDeviceFormatConversion)(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat);
	PROXYDIRECTMETHOD(GetDeviceCaps)(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
	PROXYDIRECTMETHOD_(HMONITOR, GetAdapterMonitor)(UINT Adapter);
	PROXYDIRECTMETHOD(CreateDevice)( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

	// IDirect3D9Ex methods
	PROXYDIRECTMETHOD_(UINT, GetAdapterModeCountEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter);
	PROXYDIRECTMETHOD(EnumAdapterModesEx)(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode);
	PROXYDIRECTMETHOD(GetAdapterDisplayModeEx)(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);
	PROXYDIRECTMETHOD(CreateDeviceEx)(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface);
	PROXYDIRECTMETHOD(GetAdapterLUID)(UINT Adapter, LUID * pLUID);
};
