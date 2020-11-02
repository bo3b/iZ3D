#include "stdafx.h"
#include "Direct3D9.h"
#include "ProxyDirect9-inl.h"
#include "ProxyDirect9-inl.h"
#include "ProxyDirect9_Hook.h"

using namespace IDirect3D9MethodNames;

//--- unmodified IDirect3D9 function.  Hooked in NGuard compatible hooking case ---

void CDirect3D9::HookIDirect3D9All()
{
#define HOOKINTERFACEENTRY(x) m_Direct3D9.HookInterfaceEntry(this, _ ##x ##_, Proxy_ ##x)

	//--- it's not necessary to hook unmodified functions ---
	//--- uncomment appropriate hook if you wish modify function ---
	//HOOKINTERFACEENTRY(QueryInterface);
	HOOKINTERFACEENTRY(AddRef);
	HOOKINTERFACEENTRY(Release);
	//HOOKINTERFACEENTRY(RegisterSoftwareDevice);
	HOOKINTERFACEENTRY(GetAdapterCount);
	//HOOKINTERFACEENTRY(GetAdapterIdentifier);
	//HOOKINTERFACEENTRY(GetAdapterModeCount);
	//HOOKINTERFACEENTRY(EnumAdapterModes);
	HOOKINTERFACEENTRY(GetAdapterDisplayMode);
	//HOOKINTERFACEENTRY(CheckDeviceType);
	//HOOKINTERFACEENTRY(CheckDeviceFormat);
	//HOOKINTERFACEENTRY(CheckDeviceMultiSampleType);
	//HOOKINTERFACEENTRY(CheckDepthStencilMatch);
	//HOOKINTERFACEENTRY(CheckDeviceFormatConversion);
	//HOOKINTERFACEENTRY(GetDeviceCaps);
	//HOOKINTERFACEENTRY(GetAdapterMonitor);
	HOOKINTERFACEENTRY(CreateDevice);

	if(m_Direct3D9.getExMode() != EXMODE_NONE)
	{
		// IDirect3D9Ex methods
		//HOOKINTERFACEENTRY(GetAdapterModeCountEx);
		//HOOKINTERFACEENTRY(EnumAdapterModesEx);
		//HOOKINTERFACEENTRY(GetAdapterDisplayModeEx);
		HOOKINTERFACEENTRY(CreateDeviceEx);
		//HOOKINTERFACEENTRY(GetAdapterLUID);
	}
}

void CDirect3D9::UnHookIDirect3D9All()
{
#define UNHOOKINTERFACEENTRY(x) m_Direct3D9.UnhookInterfaceEntry(this, _ ##x ##_)

	//--- it's not necessary to unhook unmodified functions ---
	//--- uncomment appropriate unhook if you wish modify function ---
	//UNHOOKINTERFACEENTRY(QueryInterface);
	UNHOOKINTERFACEENTRY(AddRef);
	UNHOOKINTERFACEENTRY(Release);
	//UNHOOKINTERFACEENTRY(RegisterSoftwareDevice);
	UNHOOKINTERFACEENTRY(GetAdapterCount);
	//UNHOOKINTERFACEENTRY(GetAdapterIdentifier);
	//UNHOOKINTERFACEENTRY(GetAdapterModeCount);
	//UNHOOKINTERFACEENTRY(EnumAdapterModes);
	UNHOOKINTERFACEENTRY(GetAdapterDisplayMode);
	//UNHOOKINTERFACEENTRY(CheckDeviceType);
	//UNHOOKINTERFACEENTRY(CheckDeviceFormat);
	//UNHOOKINTERFACEENTRY(CheckDeviceMultiSampleType);
	//UNHOOKINTERFACEENTRY(CheckDepthStencilMatch);
	//UNHOOKINTERFACEENTRY(CheckDeviceFormatConversion);
	//UNHOOKINTERFACEENTRY(GetDeviceCaps);
	//UNHOOKINTERFACEENTRY(GetAdapterMonitor);
	UNHOOKINTERFACEENTRY(CreateDevice);
	
	if(m_Direct3D9.getExMode() != EXMODE_NONE)
	{
		// IDirect3D9Ex methods
		//UNHOOKINTERFACEENTRY(GetAdapterModeCountEx);
		//UNHOOKINTERFACEENTRY(EnumAdapterModesEx);
		//UNHOOKINTERFACEENTRY(GetAdapterDisplayModeEx);
		UNHOOKINTERFACEENTRY(CreateDeviceEx);
		//UNHOOKINTERFACEENTRY(GetAdapterLUID);
	}
}

namespace IDirect3D9MethodNames
{
	// IUnknown methods 
	PROXYSTDMETHOD(QueryInterface)(IDirect3D9* pDirect, REFIID riid, void** ppvObj)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->QueryInterface(riid, ppvObj);
			else
				return pWrapper->m_Direct3D9.QueryInterface(riid, ppvObj);
		}
		else
		{
			DECLAREPOINTER(pDirect, QueryInterface);
			HRESULT hResult = CALLFORPOINTER(pDirect, QueryInterface)(riid, ppvObj);
			return hResult;
		}
	}

	PROXYSTDMETHOD_(ULONG,AddRef)(IDirect3D9* pDirect) 
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (pWrapper->m_Direct3D9.m_RouterHookGuard > 0)
				InterlockedIncrement(&pWrapper->m_Direct3D9.m_InternalReferenceCount);
			ULONG rc = pWrapper->m_Direct3D9.AddRef();
			//DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc, pWrapper->m_Direct3D9.m_InternalReferenceCount, pWrapper->m_Direct3D9.m_RouterHookGuard);
			return rc - pWrapper->m_Direct3D9.m_InternalReferenceCount;
		}
		else
		{
			DECLAREPOINTER(pDirect, AddRef);
			ULONG uCount = CALLFORPOINTER(pDirect, AddRef)();
			return uCount;
		}
	}

	PROXYSTDMETHOD_(ULONG,Release)(IDirect3D9* pDirect)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (pWrapper->m_Direct3D9.m_RouterHookGuard > 0)
				InterlockedDecrement(&pWrapper->m_Direct3D9.m_InternalReferenceCount);

			pWrapper->m_Direct3D9.AddRef();
			ULONG rc = pWrapper->m_Direct3D9.Release();
			LONG internalCount = std::max<LONG>(0, pWrapper->m_Direct3D9.m_InternalReferenceCount);
			//	DEBUG_TRACE3(_T("%s: RefCount = %2i, InternalRefCount = %2i  IsCallInternal = %2i\n"), _T(__FUNCTION__), rc-1, pWrapper->m_Direct3D9.m_InternalReferenceCount, pWrapper->m_Direct3D9.m_RouterHookGuard);
			if ((LONG)rc <= pWrapper->m_Direct3D9.m_InternalReferenceCount + 1)
			{
				//--- guard from recursion wrapper releasing ---
				pWrapper->m_Direct3D9.m_InternalReferenceCount = -1;
				rc = pWrapper->Release();
				internalCount = 0;
			}
			else
				rc = pWrapper->m_Direct3D9.Release();
			return rc - internalCount;
		}
		else
		{
			DECLAREPOINTER(pDirect, Release);
			ULONG uCount = CALLFORPOINTER(pDirect, Release)();
			return uCount;
		}
	}

	// IDirect3D9 methods
	PROXYSTDMETHOD(RegisterSoftwareDevice)(IDirect3D9* pDirect, void* pInitializeFunction)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->RegisterSoftwareDevice(pInitializeFunction);
			else
				return pWrapper->m_Direct3D9.RegisterSoftwareDevice(pInitializeFunction);
		}
		else
		{
			DECLAREPOINTER(pDirect, RegisterSoftwareDevice);
			UINT uCount = CALLFORPOINTER(pDirect, RegisterSoftwareDevice)(pInitializeFunction);
			return uCount;
		}
	}

	PROXYSTDMETHOD_(UINT, GetAdapterCount)(IDirect3D9* pDirect)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterCount();
			else
				return pWrapper->m_Direct3D9.GetAdapterCount();
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterCount);
			UINT uCount = CALLFORPOINTER(pDirect, GetAdapterCount)();
			return uCount;
		}
	}

	PROXYSTDMETHOD(GetAdapterIdentifier)(IDirect3D9* pDirect, UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
			else
				return pWrapper->m_Direct3D9.GetAdapterIdentifier(Adapter, Flags, pIdentifier);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterIdentifier);
			HRESULT hResult = CALLFORPOINTER(pDirect, GetAdapterIdentifier)(Adapter, Flags, pIdentifier);
			return hResult;
		}
	}

	PROXYSTDMETHOD_(UINT, GetAdapterModeCount)(IDirect3D9* pDirect, UINT Adapter, D3DFORMAT Format)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterModeCount(Adapter, Format);
			else
				return pWrapper->m_Direct3D9.GetAdapterModeCount(Adapter, Format);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterModeCount);
			UINT uCount = CALLFORPOINTER(pDirect, GetAdapterModeCount)(Adapter, Format);
			return uCount;
		}
	}

	PROXYSTDMETHOD(EnumAdapterModes)(IDirect3D9* pDirect, UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->EnumAdapterModes(Adapter, Format, Mode, pMode);
			else
				return pWrapper->m_Direct3D9.EnumAdapterModes(Adapter, Format, Mode, pMode);
		}
		else
		{
			DECLAREPOINTER(pDirect, EnumAdapterModes);
			HRESULT hResult = CALLFORPOINTER(pDirect, EnumAdapterModes)(Adapter, Format, Mode, pMode);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetAdapterDisplayMode)(IDirect3D9* pDirect, UINT Adapter, D3DDISPLAYMODE* pMode)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterDisplayMode(Adapter, pMode);
			else
				return pWrapper->m_Direct3D9.GetAdapterDisplayMode(Adapter, pMode);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterDisplayMode);
			HRESULT hResult = CALLFORPOINTER(pDirect, GetAdapterDisplayMode)(Adapter, pMode);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CheckDeviceType)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
			else
				return pWrapper->m_Direct3D9.CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
		}
		else
		{
			DECLAREPOINTER(pDirect, CheckDeviceType);
			HRESULT hResult = CALLFORPOINTER(pDirect, CheckDeviceType)(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CheckDeviceFormat)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
			else
				return pWrapper->m_Direct3D9.CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
		}
		else
		{
			DECLAREPOINTER(pDirect, CheckDeviceFormat);
			HRESULT hResult = CALLFORPOINTER(pDirect, CheckDeviceFormat)(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CheckDeviceMultiSampleType)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
			else
				return pWrapper->m_Direct3D9.CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
		}
		else
		{
			DECLAREPOINTER(pDirect, CheckDeviceMultiSampleType);
			HRESULT hResult = CALLFORPOINTER(pDirect, CheckDeviceMultiSampleType)(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CheckDepthStencilMatch)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
			else
			return pWrapper->m_Direct3D9.CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
		}
		else
		{
			DECLAREPOINTER(pDirect, CheckDepthStencilMatch);
			HRESULT hResult = CALLFORPOINTER(pDirect, CheckDepthStencilMatch)(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CheckDeviceFormatConversion)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
			else
			return pWrapper->m_Direct3D9.CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
		}
		else
		{
			DECLAREPOINTER(pDirect, CheckDeviceFormatConversion);
			HRESULT hResult = CALLFORPOINTER(pDirect, CheckDeviceFormatConversion)(Adapter, DeviceType, SourceFormat, TargetFormat);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetDeviceCaps)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetDeviceCaps(Adapter, DeviceType, pCaps);
			else
			return pWrapper->m_Direct3D9.GetDeviceCaps(Adapter, DeviceType, pCaps);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetDeviceCaps);
			HRESULT hResult = CALLFORPOINTER(pDirect, GetDeviceCaps)(Adapter, DeviceType, pCaps);
			return hResult;
		}
	}

	PROXYSTDMETHOD_(HMONITOR, GetAdapterMonitor)(IDirect3D9* pDirect, UINT Adapter)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterMonitor(Adapter);
			else
			return pWrapper->m_Direct3D9.GetAdapterMonitor(Adapter);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterMonitor);
			HMONITOR hMonitor = CALLFORPOINTER(pDirect, GetAdapterMonitor)(Adapter);
			return hMonitor;
		}
	}

	PROXYSTDMETHOD(CreateDevice)(IDirect3D9* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
			else
				return pWrapper->m_Direct3D9.CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		}
		else
		{
			DECLAREPOINTER(pDirect, CreateDevice);
			HRESULT hResult = CALLFORPOINTER(pDirect, CreateDevice)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
			return hResult;
		}
	}

	// IDirect3D9Ex methods
	PROXYSTDMETHOD_(UINT, GetAdapterModeCountEx)(IDirect3D9Ex* pDirect, UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterModeCountEx(Adapter, pFilter);
			else
			return pWrapper->m_Direct3D9.GetAdapterModeCountEx(Adapter, pFilter);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterModeCountEx);
			UINT uCount = CALLFORPOINTER(pDirect, GetAdapterModeCountEx)(Adapter, pFilter);
			return uCount;
		}
	}

	PROXYSTDMETHOD(EnumAdapterModesEx)(IDirect3D9Ex* pDirect, UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
			else
			return pWrapper->m_Direct3D9.EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
		}
		else
		{
			DECLAREPOINTER(pDirect, EnumAdapterModesEx);
			HRESULT hResult = CALLFORPOINTER(pDirect, EnumAdapterModesEx)(Adapter, pFilter, Mode, pMode);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetAdapterDisplayModeEx)(IDirect3D9Ex* pDirect, UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
			else
			return pWrapper->m_Direct3D9.GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterDisplayModeEx);
			HRESULT hResult = CALLFORPOINTER(pDirect, GetAdapterDisplayModeEx)(Adapter, pMode, pRotation);
			return hResult;
		}
	}

	PROXYSTDMETHOD(CreateDeviceEx)(IDirect3D9Ex* pDirect, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
			else
			return pWrapper->m_Direct3D9.CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
		}
		else
		{
			DECLAREPOINTER(pDirect, CreateDeviceEx);
			HRESULT hResult = CALLFORPOINTER(pDirect, CreateDeviceEx)(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
			return hResult;
		}
	}

	PROXYSTDMETHOD(GetAdapterLUID)(IDirect3D9Ex* pDirect, UINT Adapter, LUID * pLUID)
	{
		CDirect3D9* pWrapper = FindDirectWrapper(pDirect);
		if (pWrapper)
		{
			if (CalledFromApp())
				return pWrapper->GetAdapterLUID(Adapter, pLUID);
			else
			return pWrapper->m_Direct3D9.GetAdapterLUID(Adapter, pLUID);
		}
		else
		{
			DECLAREPOINTER(pDirect, GetAdapterLUID);
			HRESULT hResult = CALLFORPOINTER(pDirect, GetAdapterLUID)(Adapter, pLUID);
			return hResult;
		}
	}
}
