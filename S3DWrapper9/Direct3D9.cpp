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
#include "Direct3D9.h"
#include "StereoRenderer.h"
#include "WideStereoRenderer.h"
#include "Trace.h"
#include "..\S3DAPI\ReadData.h"
#include "ProxyDirect9-inl.h"
#include "ProxyDirect9_Hook.h"
#include "CommandDumper.h"

CDirect3D9::~CDirect3D9()
{
	if(gInfo.RouterType == ROUTER_TYPE_HOOK)
	{
		UnHookIDirect3D9All();
		DEBUG_MESSAGE(_T("Count of hooked functions after IDirect3D9 wrapper realising - %d\n"), g_UniqueHooks.size());
	}
	g_pDirectWrapperList.RemoveWrapper(this);
}

#pragma optimize( "y", off ) // for Steam Community

STDMETHODIMP CDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
	if(CalledFromApp())
	{
		if( ppvObj == NULL )
			return E_POINTER;

		if(InlineIsEqualGUID(riid, IID_IUnknown)) 
		{
			this->AddRef();
			* ppvObj = this;
			return S_OK;
		}
		if(InlineIsEqualGUID(riid, IID_IDirect3D9)) 
		{
			this->AddRef();
			* ppvObj = this;
			return S_OK;
		}
		if(InlineIsEqualGUID(riid, IID_IDirect3D9Ex) && m_Direct3D9.getExMode() == EXMODE_EX) 
		{
			this->AddRef();
			* ppvObj = this;
			return S_OK;
		}
	}
	return m_Direct3D9.QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) CDirect3D9::AddRef()
{
	return InterlockedIncrement(& m_RefCount);
}

STDMETHODIMP_(ULONG) CDirect3D9::Release()
{
	ULONG refCount = InterlockedDecrement(& m_RefCount);
	if(refCount == 0)
		delete this;
	return refCount;
}

#pragma optimize( "y", on )

void CDirect3D9::DoInitialize(IDirect3D9* pReal, PROXY_EXMODE exMode)
{
	m_Direct3D9.setExMode(exMode);
	
	//--- Steam perhaps hooks Direct3DCreate9(), we MUST use QueryInterface for getting original IDirect3D9 ---
	if(exMode == EXMODE_NONE)
	{
		CComPtr<IDirect3D9> pReal_Original;
		pReal->QueryInterface(IID_IDirect3D9, (void**)&pReal_Original);
		m_Direct3D9.initialize(pReal_Original);
	}
	else
	{
		CComPtr<IDirect3D9Ex> pRealEx_Original;
		pReal->QueryInterface(IID_IDirect3D9Ex, (void**)&pRealEx_Original);
		m_Direct3D9.initialize(pRealEx_Original);
	}
	
	g_pDirectWrapperList.AddWrapper(m_Direct3D9.GetReal(), this);
	if(gInfo.RouterType == ROUTER_TYPE_HOOK)
		HookIDirect3D9All();
}

void CDirect3D9::PrepareDevice( UINT Adapter )
{
	D3DADAPTER_IDENTIFIER9 ident;
	if (SUCCEEDED(m_Direct3D9.GetAdapterIdentifier(GetAdapterNumber(Adapter), 0, &ident)) && GINFO_DEBUG)
	{    
		DEBUG_MESSAGE(_T("Adapter info\n"));
		DEBUG_MESSAGE(_T("Driver: %S\n"), ident.Driver);
		DEBUG_MESSAGE(_T("Description: %S\n"), ident.Description);
		DEBUG_MESSAGE(_T("Device name: %S\n"), ident.DeviceName);
		DEBUG_MESSAGE(_T("Driver version: %d.%d.%d.%d\n"), 
			HIWORD(ident.DriverVersion.HighPart),	LOWORD(ident.DriverVersion.HighPart), 
			HIWORD(ident.DriverVersion.LowPart),	LOWORD(ident.DriverVersion.LowPart));
		DEBUG_MESSAGE(_T("VendorId: 0x%X\n"), ident.VendorId);
		DEBUG_MESSAGE(_T("DeviceId: 0x%X\n"), ident.DeviceId);
		DEBUG_MESSAGE(_T("SubSysId: 0x%X\n"), ident.SubSysId);
		DEBUG_MESSAGE(_T("Revision: 0x%X\n"), ident.Revision);
	}

	ReadCurrentProfile(ident.VendorId);
}

HMODULE CDirect3D9::GetCallingModule( LPCVOID pCaller )
{
	HMODULE hModule = NULL;
	MEMORY_BASIC_INFORMATION mbi;
	if ( VirtualQuery(pCaller, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION) )
	{
		// the allocation base is the beginning of a PE file 
		hModule = (HMODULE) mbi.AllocationBase;
	}
	return hModule;
}

STDMETHODIMP CDirect3D9::CreateDevice( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface )
{
	HRESULT hResult = S_OK;
	if (gInfo.UseMonoDeviceWrapper)
	{
		CMonoRenderer *pCMonoRenderer = DNew CMonoRenderer();
		CComPtr<IDirect3DDevice9> pMonoRenderer = pCMonoRenderer;
		NSCALL_TRACE1(pCMonoRenderer->Initialize(
			this, m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), DeviceType,
			hFocusWindow, BehaviorFlags, pPresentationParameters),
			DEBUG_MESSAGE(_T("Initialize(pRealDirect3D9 = %p, Adapter = %d, DeviceType = %s, ")
			_T("hFocusWindow = %p, BehaviorFlags = %s, pPresentationParameters = %s)"),
			m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), GetDeviceTypeString(DeviceType),
			hFocusWindow, GetBehaviorFlagsString(BehaviorFlags), 
			GetPresentationParametersString(pPresentationParameters)));
		if(FAILED(hResult))
		{
			*ppReturnedDeviceInterface = 0;
			return hResult;
		}
		IDirect3DDevice9 *pReturnedDeviceInterface = *ppReturnedDeviceInterface;
		{
			DUMP_CMD_ALL(CreateDevice,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,pReturnedDeviceInterface);
		}
		NSCALL(pMonoRenderer.QueryInterface(ppReturnedDeviceInterface));
		return hResult;
	}

	// if add D3DDEVTYPE_REF Steam Community crash on some apps
	if(DeviceType == D3DDEVTYPE_NULLREF || DeviceType == D3DDEVTYPE_SW)
	{
		DEBUG_MESSAGE(_T("Skipping D3DDEVTYPE_NULLREF & D3DDEVTYPE_REF device.\n"));
		return m_Direct3D9.CreateDevice(GetAdapterNumber(Adapter), DeviceType, hFocusWindow,
			BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	}

	PrepareDevice(Adapter);
	HMODULE hCallingModule = GetCallingModule((LPCVOID)g_CallingModule);
	UINT D3DVersion = 9;
	if (hCallingModule)
	{
#ifndef FINAL_RELEASE
		TCHAR moduleName[MAX_PATH];
		GetModuleFileName(hCallingModule, moduleName, sizeof(moduleName));
		DEBUG_MESSAGE(_T("Calling Module: %s\n"), moduleName);
#endif

		if (hCallingModule == GetModuleHandle(_T("S3DWrapperD3D8.dll")))
			D3DVersion = 8;
		else if (hCallingModule == GetModuleHandle(_T("S3DWrapperD3D7.dll")))
			D3DVersion = 7;
	}

	CComPtr<IDirect3DDevice9> pStereoRenderer;
	if (!gInfo.WideRenderTarget) {
		pStereoRenderer = DNew CStereoRenderer();
	} else {
		pStereoRenderer = DNew CWideStereoRenderer();
	}
	NSPTR(pStereoRenderer);
	CBaseStereoRenderer *pCStereoRenderer;
	pCStereoRenderer = (CBaseStereoRenderer *)(IDirect3DDevice9 *)pStereoRenderer;
	
	NSCALL_TRACE1(pCStereoRenderer->Initialize(
		this, m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), DeviceType,
		hFocusWindow, BehaviorFlags, pPresentationParameters),
		DEBUG_MESSAGE(_T("Initialize(pRealDirect3D9 = %p, Adapter = %d, DeviceType = %s, ")
		_T("hFocusWindow = %p, BehaviorFlags = %s, pPresentationParameters = %s)"),
		m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), GetDeviceTypeString(DeviceType),
		hFocusWindow, GetBehaviorFlagsString(BehaviorFlags), 
		GetPresentationParametersString(pPresentationParameters)));
	if(FAILED(hResult))
	{
		*ppReturnedDeviceInterface = 0;
		return hResult;
	}
	if(gInfo.RouterType == ROUTER_TYPE_HOOK)
	{
		pCStereoRenderer->AddRef();
		*ppReturnedDeviceInterface = pCStereoRenderer->m_Direct3DDevice.GetReal();
	}
	else
	{
		NSCALL(pStereoRenderer.QueryInterface(ppReturnedDeviceInterface));
	}
	pCStereoRenderer->SetDirect3DVersion(D3DVersion);

	IDirect3DDevice9 *pReturnedDeviceInterface = *ppReturnedDeviceInterface;
	{
		DUMP_CMD_ALL(CreateDevice,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,pReturnedDeviceInterface);
	}	
#ifndef FINAL_RELEASE	
	for(DWORD RenderTargetIndex = 0;RenderTargetIndex < 4;RenderTargetIndex++)
	{
		CComPtr<IDirect3DSurface9> pRenderTarget;
		if (SUCCEEDED(pReturnedDeviceInterface->GetRenderTarget(RenderTargetIndex,&pRenderTarget)))
		{
			if (pRenderTarget)
			{
				DUMP_CMD_ALL(GetRenderTarget,RenderTargetIndex,pRenderTarget);
			}
		}
	}

	CComPtr<IDirect3DSurface9> pZStencilSurface;
	if (SUCCEEDED(pReturnedDeviceInterface->GetDepthStencilSurface(&pZStencilSurface)))
	{
		if (pZStencilSurface)
		{
			DUMP_CMD_ALL(GetDepthStencilSurface,pZStencilSurface);
		}
	}
#endif // FINAL_RELEASE
	
	return hResult;
}

STDMETHODIMP CDirect3D9::CreateDeviceEx( UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface )
{
	HRESULT hResult = S_OK;
	if (gInfo.UseMonoDeviceWrapper)
	{
		CMonoRenderer *pCMonoRenderer = DNew CMonoRenderer();
		 CComPtr<IDirect3DDevice9Ex> pMonoRenderer = pCMonoRenderer;
		NSCALL_TRACE1(pCMonoRenderer->InitializeEx(
			this, (IDirect3D9Ex*)m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), DeviceType,
			hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode),
			DEBUG_MESSAGE(_T("InitializeEx(pRealDirect3D9 = %p, Adapter = %d, DeviceType = %s, ")
			_T("hFocusWindow = %p, BehaviorFlags = %s, pPresentationParameters = %s, pFullscreenDisplayMode = %p)"),
			m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), GetDeviceTypeString(DeviceType),
			hFocusWindow, GetBehaviorFlagsString(BehaviorFlags), 
			GetPresentationParametersString(pPresentationParameters), pFullscreenDisplayMode));
		if(FAILED(hResult))
		{
			*ppReturnedDeviceInterface = 0;
			return hResult;
		}
		IDirect3DDevice9 *pReturnedDeviceInterface = *ppReturnedDeviceInterface;
		{
			DUMP_CMD_ALL(CreateDeviceEx,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,pFullscreenDisplayMode,pReturnedDeviceInterface);
		}
		NSCALL(pMonoRenderer.QueryInterface(ppReturnedDeviceInterface));
		return hResult;
	}

	// if add D3DDEVTYPE_REF Steam Community crash on some apps
	if(DeviceType == D3DDEVTYPE_NULLREF || DeviceType == D3DDEVTYPE_SW)
	{
		DEBUG_MESSAGE(_T("Skipping D3DDEVTYPE_NULLREF & D3DDEVTYPE_REF device.\n"));
		return m_Direct3D9.CreateDeviceEx(GetAdapterNumber(Adapter), DeviceType, hFocusWindow,
			BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
	}

	PrepareDevice(Adapter);
	HMODULE hCallingModule = GetCallingModule((LPCVOID)g_CallingModule);
	UINT D3DVersion = 9;
	if (hCallingModule)
	{
		TCHAR moduleName[MAX_PATH];
		GetModuleFileName(hCallingModule, moduleName, sizeof(moduleName));
		DEBUG_MESSAGE(_T("Calling Module: %s\n"), moduleName);

		if (hCallingModule == GetModuleHandle(_T("S3DWrapperD3D8.dll")))
			D3DVersion = 8;
		else if (hCallingModule == GetModuleHandle(_T("S3DWrapperD3D7.dll")))
			D3DVersion = 7;
	}

	CComPtr<IDirect3DDevice9Ex> pStereoRenderer;
	if (!gInfo.WideRenderTarget) {
		pStereoRenderer = DNew CStereoRenderer();
	} else {
		pStereoRenderer = DNew CWideStereoRenderer();
	}
	NSPTR(pStereoRenderer);
	CBaseStereoRenderer *pCStereoRenderer;
	pCStereoRenderer = (CBaseStereoRenderer *)(IDirect3DDevice9Ex *)pStereoRenderer;
	NSCALL_TRACE1(pCStereoRenderer->InitializeEx(
		this, (IDirect3D9Ex*)m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), DeviceType,
		hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode),
		DEBUG_MESSAGE(_T("InitializeEx(pRealDirect3D9 = %p, Adapter = %d, DeviceType = %s, ")
		_T("hFocusWindow = %p, BehaviorFlags = %s, pPresentationParameters = %s, pFullscreenDisplayMode = %p)"),
		m_Direct3D9.GetReal(), GetAdapterNumber(Adapter), GetDeviceTypeString(DeviceType),
		hFocusWindow, GetBehaviorFlagsString(BehaviorFlags), 
		GetPresentationParametersString(pPresentationParameters), pFullscreenDisplayMode));
	if(FAILED(hResult))
	{
		*ppReturnedDeviceInterface = 0;
		return hResult;
	}
	IDirect3DDevice9 *pReturnedDeviceInterface = *ppReturnedDeviceInterface;
	{
		DUMP_CMD_ALL(CreateDeviceEx,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,pFullscreenDisplayMode,pReturnedDeviceInterface);
	}
	if(gInfo.RouterType == ROUTER_TYPE_HOOK)
	{
		pCStereoRenderer->AddRef();
		*ppReturnedDeviceInterface = (IDirect3DDevice9Ex*)pCStereoRenderer->m_Direct3DDevice.GetReal();
	}
	else
	{
		NSCALL(pStereoRenderer.QueryInterface(ppReturnedDeviceInterface));
	}
	pCStereoRenderer->SetDirect3DVersion(D3DVersion);
	return hResult;
}
