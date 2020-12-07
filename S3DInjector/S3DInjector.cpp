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
#include <madCHook.h>
#define INITGUID
#include <shlwapi.h>
#include "AntiCheat.h"
#include "ProductNames.h"
#include <dxgi.h>
#include <d3d11.h>
#include "../CommonUtils/System.h"
#include <string>
#include "..\ZLog\zlog.h"
#include "HookPolice.h"
#include "S3DInjector.h"
#include "ExplicitLibraryLoad.h"

#ifdef _DEBUG
#include <d3d9.h>
	#define	TRACE(s) OutputDebugString(_T(s))
#else
	#define	TRACE(s) 
#endif

// ToDo:  RESTORE_CODE_BEFORE_HOOK is removed, needs to be replaced by RestoreCode
DWORD g_HookingFlags = NO_SAFE_UNHOOKING;

HMODULE GetCallingModule( LPCVOID pCaller )
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

__declspec(dllexport) void WINAPI Disable3DDriver()
{
	FinalizeMadCHook();
}

CONST LONG MaximumCallGuardCountBeforeCompleteStackOverflow = 10;

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

#define g_CallingModule reinterpret_cast<LONG_PTR>(_ReturnAddress())

// ***************************************************************
bool	LoggingInitialized;
_TCHAR	DllPath[MAX_PATH];

// ***************************************************************
typedef DWORD (WINAPI *InitializeExchangeServer)();
typedef HRESULT (WINAPI *DirectDrawCreate)( GUID FAR *lpGUID, LPVOID FAR *lplpDD, IUnknown FAR *pUnkOuter );
typedef HRESULT (WINAPI *DirectDrawCreateEx)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );
typedef void* (WINAPI *Direct3DCreate)(UINT  sdkVersion);
typedef HRESULT (WINAPI *Direct3DCreateEx)(UINT  sdkVersion, PVOID* ppd3d9);
typedef Direct3DCreate (WINAPI *GetD3D9Creator)();
typedef Direct3DCreateEx (WINAPI *GetD3D9CreatorEx)();

// ***************************************************************
// DirectDraw

LONG g_CallGuardDDraw = 0;
struct CALL_GUARD_DDRAW
{
	CALL_GUARD_DDRAW()	{ g_CallGuardDDraw++; }
	~CALL_GUARD_DDRAW()	{ g_CallGuardDDraw--; }
};

HMODULE g_hModDDraw = 0;
HRESULT (WINAPI *DirectDrawCreateNext)( GUID FAR *lpGUID, LPVOID FAR *lplpDD, IUnknown FAR *pUnkOuter );
HRESULT (WINAPI *DirectDrawCreateExNext)( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter );
_TCHAR	DllDX7Path[MAX_PATH];

DEFINE_GUID( IID_IDirectDraw4,                  0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );
DEFINE_GUID( IID_IDirectDraw7,                  0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );

HRESULT WINAPI DirectDrawCreateCallback( GUID FAR * lpGuid, LPVOID  *lplpDD, IUnknown FAR *pUnkOuter )
{
	isHookPoliceWorking = false;
	TRACE("DirectDrawCreateCallback\n");
	CALL_GUARD_DDRAW guard;
	if(g_CallGuardDDraw > 1 && NoCheat(DllDX7Path) || g_hModDDraw == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDDraw <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return DirectDrawCreateNext(lpGuid, lplpDD, pUnkOuter);
	}

	if(!g_hModDDraw)
		g_hModDDraw = LoadLibraryEx(DllDX7Path, 0, 0);

	if(g_hModDDraw)
	{
		DirectDrawCreate p = (DirectDrawCreate)GetProcAddress(g_hModDDraw, "DirectDrawCreate");
		InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModDDraw, "InitializeExchangeServer");
		if(p && pGetRouter)
		{
			DWORD routerType = pGetRouter();
			if (routerType == 0 || routerType == 1)
				return p(lpGuid, lplpDD, pUnkOuter);
			else
			{
				HRESULT hResult = DirectDrawCreateNext(lpGuid, lplpDD, pUnkOuter);
				FreeLibrary(g_hModDDraw);
				g_hModDDraw = (HMODULE)-1;
				return hResult;
			}
		}
	}
	return DirectDrawCreateNext(lpGuid, lplpDD, pUnkOuter);
}

HRESULT WINAPI DirectDrawCreateExCallback( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter )
{
	isHookPoliceWorking = false;
	TRACE("DirectDrawCreateExCallback\n");
	CALL_GUARD_DDRAW guard;
	if(g_CallGuardDDraw > 1 && NoCheat(DllDX7Path) || !InlineIsEqualGUID(iid, IID_IDirectDraw7) || g_hModDDraw == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDDraw <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return DirectDrawCreateExNext(lpGuid, lplpDD, iid, pUnkOuter);
	}

	if(!g_hModDDraw)
		g_hModDDraw = LoadLibraryEx(DllDX7Path, 0, 0);

	if(g_hModDDraw)
	{
		DirectDrawCreateEx p = (DirectDrawCreateEx)GetProcAddress(g_hModDDraw, "DirectDrawCreateEx");
		InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModDDraw, "InitializeExchangeServer");
		if(p && pGetRouter)
		{
			DWORD routerType = pGetRouter();
			if (routerType == 0 || routerType == 1)
				return p(lpGuid, lplpDD, iid, pUnkOuter);
			else
			{
				HRESULT hResult = DirectDrawCreateExNext(lpGuid, lplpDD, iid, pUnkOuter);
				FreeLibrary(g_hModDDraw);
				g_hModDDraw = (HMODULE)-1;
				return hResult;
			}
		}
	}
	return DirectDrawCreateExNext(lpGuid, lplpDD, iid, pUnkOuter);
}

// ***************************************************************
// Direct3D8

LONG g_CallGuardDX8 = 0;
struct CALL_GUARD_DX8
{
	CALL_GUARD_DX8()	{ g_CallGuardDX8++; }
	~CALL_GUARD_DX8()	{ g_CallGuardDX8--; }
};

HMODULE g_hModD3D8 = 0;
PVOID   (WINAPI *Direct3DCreate8Next)(DWORD sdkVersion);						 
_TCHAR	DllDX8Path[MAX_PATH];

PVOID WINAPI Direct3DCreate8Callback(DWORD sdkVersion)
{
	isHookPoliceWorking = false;
	TRACE("Direct3DCreate8Callback\n");
	CALL_GUARD_DX8 guard;
	if(g_CallGuardDX8 > 1 && NoCheat(DllDX8Path) || g_hModD3D8 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDX8 <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return (Direct3DCreate)Direct3DCreate8Next(sdkVersion);
	}

	if(!g_hModD3D8)
		g_hModD3D8 = LoadLibraryEx(DllDX8Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

	if(g_hModD3D8)
	{
		Direct3DCreate p = (Direct3DCreate)GetProcAddress(g_hModD3D8, "Direct3DCreate8");
		InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModD3D8, "InitializeExchangeServer");
		if(p && pGetRouter)
		{
			DWORD routerType = pGetRouter();
			if (routerType == 0 || routerType == 1)
				return p(sdkVersion);
			else
			{
				PVOID pd3d8 = (Direct3DCreate)Direct3DCreate8Next(sdkVersion);
				FreeLibrary(g_hModD3D8);
				g_hModD3D8 = (HMODULE)-1;
				return pd3d8;
			}
		}
	}
	return (Direct3DCreate)Direct3DCreate8Next(sdkVersion);
}

// ***************************************************************
// Direct3D9

LONG g_CallGuardDX9 = 0;
struct CALL_GUARD_DX9
{
	CALL_GUARD_DX9()	{ g_CallGuardDX9++; }
	~CALL_GUARD_DX9()	{ g_CallGuardDX9--; }
};

HMODULE g_hModD3D9 = 0;
_TCHAR	DllDX9Path[MAX_PATH];
PVOID   (WINAPI *Direct3DCreate9Next)(UINT sdkVersion);
PVOID   (WINAPI *Direct3DCreate9NextRouted)(UINT sdkVersion) = NULL;					//--- Vista SP1 fix ---
HRESULT (WINAPI *Direct3DCreate9ExNext)(UINT sdkVersion, PVOID* ppd3d9);
HRESULT (WINAPI *Direct3DCreate9ExNextRouted)(UINT sdkVersion, PVOID* ppd3d9);

HANDLE g_hHookPolice = 0;
PVOID WINAPI Direct3DCreate9Callback(DWORD sdkVersion)
{
	isHookPoliceWorking = false;

	TRACE("Direct3DCreate9Callback\n");
	CALL_GUARD_DX9 guard;
	if(g_CallGuardDX9 > 1 && NoCheat(DllDX9Path) || g_hModD3D9 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDX9 <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return (Direct3DCreate)Direct3DCreate9Next(sdkVersion);
	}

	if(!g_hModD3D9)
	{
		g_hModD3D9 = LoadLibraryEx(DllDX9Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
		Direct3DCreate9NextRouted = Direct3DCreate9Next;
	}

	if(g_hModD3D9)
	{
		GetD3D9Creator pVistaSP1Router = (GetD3D9Creator)GetProcAddress(g_hModD3D9, "GetD3D9Creator");
		if(pVistaSP1Router)
			Direct3DCreate9NextRouted = pVistaSP1Router();

		InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModD3D9, "InitializeExchangeServer");
		if(pGetRouter)
		{
			DWORD routerType = pGetRouter();
			switch(routerType)
			{
			case 0:	
			case 1: //--- leave for compatibility ---			
				{
					//--- return pointer to our wrapper ---
					Direct3DCreate pCreate = (Direct3DCreate)GetProcAddress(g_hModD3D9, "Direct3DCreate9");
					if(pCreate)
						return pCreate(sdkVersion);
					else
					{
						FreeLibrary(g_hModD3D9);
						g_hModD3D9 = (HMODULE)-1;
					}
				}	
			default:
				{
					//--- create original IDirect3DDevice9 ---
					FreeLibrary(g_hModD3D9);
					g_hModD3D9 = (HMODULE)-1;
				}
			}
		}
	} 
	return (Direct3DCreate)Direct3DCreate9Next(sdkVersion);
}

HRESULT WINAPI Direct3DCreate9ExCallback(DWORD sdkVersion, PVOID* ppd3d9)
{
	isHookPoliceWorking = false;
	TRACE("Direct3DCreate9ExCallback\n");
	CALL_GUARD_DX9 guard;
	if(g_CallGuardDX9 > 1 && NoCheat(DllDX9Path) || g_hModD3D9 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDX9 <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return Direct3DCreate9ExNext(sdkVersion, ppd3d9);
	}

	if(!g_hModD3D9)
	{
		g_hModD3D9 = LoadLibraryEx(DllDX9Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
		Direct3DCreate9ExNextRouted = Direct3DCreate9ExNext;
	}

	if(g_hModD3D9)
	{
		GetD3D9CreatorEx pVistaSP1ExRouter = (GetD3D9CreatorEx)GetProcAddress(g_hModD3D9, "GetD3D9CreatorEx");
		if(pVistaSP1ExRouter)
			Direct3DCreate9ExNextRouted = pVistaSP1ExRouter();

		InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModD3D9, "InitializeExchangeServer");
		if(pGetRouter)
		{
			DWORD routerType = pGetRouter();
			switch(routerType)
			{
			case 0:	
			case 1: //--- leave for compatibility ---
				{
					//--- return pointer to our wrapper ---
					Direct3DCreateEx pCreate = (Direct3DCreateEx)GetProcAddress(g_hModD3D9, "Direct3DCreate9Ex");
					if(pCreate)
						return pCreate(sdkVersion, ppd3d9);
					else
					{
						FreeLibrary(g_hModD3D9);
						g_hModD3D9 = (HMODULE)-1;
					}
				}	
			default:
				{
					//--- create original routed IDirect3DDevice9Ex ---
					FreeLibrary(g_hModD3D9);
					g_hModD3D9 = (HMODULE)-1;
				}
			}
		}
	}
	return Direct3DCreate9ExNext(sdkVersion, ppd3d9);
}

// ***************************************************************
// Direct3D10

LONG g_CallGuardDX10 = 0;
struct CALL_GUARD_DX10
{
	CALL_GUARD_DX10()	{ g_CallGuardDX10++; }
	~CALL_GUARD_DX10()	{ g_CallGuardDX10--; }
};

HMODULE g_hModD3D10 = 0;
_TCHAR	DllDX10Path[MAX_PATH];
CHAR	DriverDllDX10Path[MAX_PATH];
CHAR	DriverDllDX11Path[MAX_PATH];
typedef HRESULT (APIENTRY *PFND3D10DDI_OPENADAPTER)(void*);
HRESULT (WINAPI *OpenAdapter10Next)(void*);

HRESULT WINAPI OpenAdapter10Callback(void* pOpenData )
{
	isHookPoliceWorking = false;
	CALL_GUARD_DX10 guard;
	if(g_CallGuardDX10 > 1 && NoCheat(DllDX10Path) || g_hModD3D10 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDX10 <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return OpenAdapter10Next(pOpenData);
	}
	TRACE("OpenAdapter10Callback\n");

	if(!g_hModD3D10)
		g_hModD3D10 = LoadLibraryEx(DllDX10Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

	if(g_hModD3D10)
	{
		PFND3D10DDI_OPENADAPTER p = (PFND3D10DDI_OPENADAPTER)GetProcAddress(g_hModD3D10, "OpenAdapter10");
		if(p)
			return p(pOpenData);
		else
		{
			FreeLibrary(g_hModD3D10);
			g_hModD3D10 = (HMODULE)-1;
		}
	} 
	return OpenAdapter10Next(pOpenData);
}


LONG g_CallGuardDX10_2 = 0;
struct CALL_GUARD_DX10_2
{
	CALL_GUARD_DX10_2()	{ g_CallGuardDX10_2++; }
	~CALL_GUARD_DX10_2()	{ g_CallGuardDX10_2--; }
};

HRESULT (WINAPI *OpenAdapter10_2Next)(void*);

HRESULT WINAPI OpenAdapter10_2Callback(void* pOpenData )
{
	isHookPoliceWorking = false;
	CALL_GUARD_DX10_2 guard;
	if(g_CallGuardDX10_2 > 1 && NoCheat(DllDX10Path) || g_hModD3D10 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDX10_2 <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return OpenAdapter10_2Next(pOpenData);
	}
	TRACE("OpenAdapter10_2Callback\n");

	if(!g_hModD3D10)
		g_hModD3D10 = LoadLibraryEx(DllDX10Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

	if(g_hModD3D10)
	{
		PFND3D10DDI_OPENADAPTER p = (PFND3D10DDI_OPENADAPTER)GetProcAddress(g_hModD3D10, "OpenAdapter10_2");
		if(p)
			return p(pOpenData);
		else
		{
			FreeLibrary(g_hModD3D10);
			g_hModD3D10 = (HMODULE)-1;
		}
	} 
	return OpenAdapter10_2Next(pOpenData);
}

HRESULT (WINAPI *D3D10CreateDeviceAndSwapChainNext)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device **);
HRESULT WINAPI D3D10CreateDeviceAndSwapChainCallback(IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software,
	UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain,  ID3D10Device **ppDevice)
{
	return E_FAIL;
}

HRESULT (WINAPI *D3D10CreateDeviceAndSwapChain1Next)(IDXGIAdapter *, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D10Device1 **);
HRESULT WINAPI D3D10CreateDeviceAndSwapChain1Callback(IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software,
													  UINT Flags, D3D10_FEATURE_LEVEL1 HardwareLevel, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain,  ID3D10Device1 **ppDevice)
{
	return E_FAIL;
}

HRESULT (WINAPI *D3D11CreateDeviceAndSwapChainNext)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, CONST D3D_FEATURE_LEVEL *, UINT, UINT, 
													CONST DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
HRESULT WINAPI D3D11CreateDeviceAndSwapChainCallback(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
													  UINT Flags, CONST D3D_FEATURE_LEVEL *pFeatureLevels, UINT FeatureLevels, 
													  UINT SDKVersion, CONST DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain,
													  ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel, ID3D11DeviceContext **ppImmediateContext)
{
	return E_FAIL;
}

LONG g_CallGuardDXGI = 0;
struct CALL_GUARD_DXGI
{
	CALL_GUARD_DXGI()	{ g_CallGuardDXGI++; }
	~CALL_GUARD_DXGI()	{ g_CallGuardDXGI--; }
};

typedef HRESULT (STDMETHODCALLTYPE *PFNCREATEDXGIFACTORY)(REFIID riid, void **ppFactory);
typedef HRESULT (STDMETHODCALLTYPE *PFNMYCREATEDXGIFACTORY)(REFIID riid, void **ppFactory, HMODULE hCallingModule);

void DisableD3D1x()
{
	CHAR sysPath[MAX_PATH];
	GetSystemDirectoryA(sysPath, MAX_PATH);

	// disable D3D10
	if (!D3D10CreateDeviceAndSwapChainNext)
	{
		CHAR d3d10_dll_full_path[MAX_PATH];
		sprintf_s(d3d10_dll_full_path, "%s\\d3d10.dll", sysPath);
		HookAPI(d3d10_dll_full_path, "D3D10CreateDeviceAndSwapChain", D3D10CreateDeviceAndSwapChainCallback, (PVOID*) &D3D10CreateDeviceAndSwapChainNext, g_HookingFlags);
	}
	// disable D3D10.1
	if (!D3D10CreateDeviceAndSwapChain1Next)
	{
		CHAR d3d10_1_dll_full_path[MAX_PATH];
		sprintf_s(d3d10_1_dll_full_path, "%s\\d3d10_1.dll", sysPath);
		HookAPI(d3d10_1_dll_full_path, "D3D10CreateDeviceAndSwapChain1", D3D10CreateDeviceAndSwapChain1Callback, (PVOID*) &D3D10CreateDeviceAndSwapChain1Next, g_HookingFlags);
	}
	// disable D3D11
	if (!D3D11CreateDeviceAndSwapChainNext)
	{
		CHAR d3d11_dll_full_path[MAX_PATH];
		sprintf_s(d3d11_dll_full_path, "%s\\d3d11.dll", sysPath);
		HookAPI(d3d11_dll_full_path, "D3D11CreateDeviceAndSwapChain", D3D11CreateDeviceAndSwapChainCallback, (PVOID*) &D3D11CreateDeviceAndSwapChainNext, g_HookingFlags);
	}
}

HRESULT D3D1xDllDoesntExist( REFIID riid, void ** &ppFactory, PFNCREATEDXGIFACTORY nextFunction, bool returnFailWhenDisable )
{
	UnhookAPI((PVOID*) &OpenAdapter10Next);
	UnhookAPI((PVOID*) &OpenAdapter10_2Next);

	if (g_hModD3D10)
		FreeLibrary(g_hModD3D10);
	g_hModD3D10 = (HMODULE)-1;

	DisableD3D1x();

	if (returnFailWhenDisable)
	{
		*ppFactory = NULL;
		return DXGI_ERROR_UNSUPPORTED;
	}
	return nextFunction(riid, ppFactory);
}

STDMETHODIMP CreateDXGIFactoryX( REFIID riid, void ** &ppFactory, PFNCREATEDXGIFACTORY nextFunction, PCHAR functionName, HMODULE hCallingModule, bool returnFailWhenDisable = false )
{
	TRACE("CreateDXGIFactoryXCallback\n");

	if(!PathFileExists(DllDX10Path)) // if our dll doesn't exist
		return D3D1xDllDoesntExist(riid, ppFactory, nextFunction, returnFailWhenDisable);
	else if (hCallingModule == GetModuleHandle(_T("ieframe.dll")) ||
		hCallingModule == GetModuleHandle(_T("mshtml.dll"))) // skip IE 9 Direct2D rendering engine
	{
		UnhookAPI((PVOID*) &OpenAdapter10Next);
		UnhookAPI((PVOID*) &OpenAdapter10_2Next);

		return nextFunction(riid, ppFactory);
	}

	if(!g_hModD3D10)
		g_hModD3D10 = LoadLibraryEx(DllDX10Path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);

	if(!g_hModD3D10)  // if can't load our library
		return D3D1xDllDoesntExist(riid, ppFactory, nextFunction, returnFailWhenDisable);

	bool isD3D10DriverDisabled = false;
	InitializeExchangeServer pGetRouter = (InitializeExchangeServer)GetProcAddress(g_hModD3D10, "InitializeExchangeServer");
	if(pGetRouter)
	{
		DWORD routerType = pGetRouter();
		PFNMYCREATEDXGIFACTORY p = (PFNMYCREATEDXGIFACTORY)GetProcAddress(g_hModD3D10, functionName);
		if((routerType == 0 || routerType == 1) && p)
		{
			InitializeExchangeServer IsD3D10DriverDisabled = (InitializeExchangeServer)GetProcAddress(g_hModD3D10, "StartDDAServer");
			if(IsD3D10DriverDisabled && IsD3D10DriverDisabled())
				isD3D10DriverDisabled = true;
			else
			{
				if (DriverDllDX10Path[0] != '\0' && OpenAdapter10Next == NULL)
					HookAPI(DriverDllDX10Path, "OpenAdapter10", OpenAdapter10Callback, (PVOID*) &OpenAdapter10Next, NO_SAFE_UNHOOKING);
				if (DriverDllDX11Path[0] != '\0' && OpenAdapter10_2Next == NULL)
					HookAPI(DriverDllDX11Path, "OpenAdapter10_2", OpenAdapter10_2Callback, (PVOID*) &OpenAdapter10_2Next, NO_SAFE_UNHOOKING);

				return p(riid, ppFactory, hCallingModule);
			}
		}
	}

	UnhookAPI((PVOID*) &OpenAdapter10Next);
	UnhookAPI((PVOID*) &OpenAdapter10_2Next);

	if (g_hModD3D10)
		FreeLibrary(g_hModD3D10);
	g_hModD3D10 = (HMODULE)-1;

	if(isD3D10DriverDisabled)
	{
		DisableD3D1x();

		if (returnFailWhenDisable)
		{
			*ppFactory = NULL;
			return DXGI_ERROR_UNSUPPORTED;
		}
	}

	return nextFunction(riid, ppFactory);
}

HRESULT (STDMETHODCALLTYPE *CreateDXGIFactoryNext)(REFIID riid, void **ppFactory);

STDMETHODIMP CreateDXGIFactoryCallback(REFIID riid, void **ppFactory)
{
	isHookPoliceWorking = false;
	CALL_GUARD_DXGI guard;
	if(g_CallGuardDXGI > 1 && NoCheat(DllDX10Path) || g_hModD3D10 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDXGI <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return CreateDXGIFactoryNext(riid, ppFactory);
	}

	TRACE("CreateDXGIFactoryCallback\n");
	HMODULE hCallingModule = GetCallingModule((LPCVOID) g_CallingModule);
	return CreateDXGIFactoryX(riid, ppFactory, CreateDXGIFactoryNext, "CreateDXGIFactory", hCallingModule);
}

HRESULT (STDMETHODCALLTYPE *CreateDXGIFactory1Next)(REFIID riid, void **ppFactory);

STDMETHODIMP CreateDXGIFactory1Callback(REFIID riid, void **ppFactory)
{
	isHookPoliceWorking = false;
	CALL_GUARD_DXGI guard;
	if(g_CallGuardDXGI > 1 && NoCheat(DllDX10Path) || g_hModD3D10 == ((HMODULE)-1))
	{
		_ASSERT(g_CallGuardDXGI <= MaximumCallGuardCountBeforeCompleteStackOverflow);
		return CreateDXGIFactory1Next(riid, ppFactory);
	}

	TRACE("CreateDXGIFactory1Callback\n");
	HMODULE hCallingModule = GetCallingModule((LPCVOID) g_CallingModule);
	return CreateDXGIFactoryX(riid, ppFactory, CreateDXGIFactory1Next, "CreateDXGIFactory1", hCallingModule, true);
}

#define D3D10_VISTA_MODULE_NAME "DriverD3D10VistaModule"
#define D3D10_WIN7_MODULE_NAME	"DriverD3D10Win7Module"

void GetDriverModuleName(LPSTR ModuleName, LPCSTR Key)
{
	ModuleName[0] = '\0';
#ifndef WIN64
	LPCTSTR key = _T("SOFTWARE\\iZ3D\\iZ3D Driver\\Win32");
#else
	LPCTSTR key = _T("SOFTWARE\\iZ3D\\iZ3D Driver\\Win64");
#endif
	HKEY hDriver;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hDriver) == ERROR_SUCCESS)
	{
		DWORD Size = MAX_PATH * sizeof(CHAR);
		if (RegQueryValueExA(hDriver, Key, NULL, NULL, (BYTE*)ModuleName, &Size))
			Size = 0;
		ModuleName[Size / sizeof(CHAR)] = 0;
		RegCloseKey(hDriver);
	}
}

void InitializeLogging()
{
	TCHAR  ExeName[MAX_PATH];
	GetModuleFileName(NULL, ExeName, _countof(ExeName));
	TCHAR* s = _tcsrchr(ExeName, '\\');
	if(s) 
		_tcscpy_s(ExeName, _countof(ExeName), s+1);

	// Clear logging directory
	TCHAR LogDirectory[MAX_PATH];
	_tcscpy(LogDirectory, DllPath);
	PathAppend(LogDirectory, _T("Logs"));
	if (PathFileExists(LogDirectory))
	{
		PathAppend(LogDirectory, ExeName);
		TCHAR* c = _tcsrchr(LogDirectory, '.');
		if (c != NULL)
			c[0] = '\0';
		if (PathFileExists(LogDirectory))
			ClearDirectory(LogDirectory);
	}
}

ExplicitLibaries*	g_pExpLib = 0;
IngectorSettings	g_iSettings;
HHOOK				g_hHookUpdate = 0;

LRESULT CALLBACK HookUpdateProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	RenewHook((PVOID*)Direct3DCreate9Next);
	RenewHook((PVOID*) &CreateDXGIFactoryNext);
	RenewHook((PVOID*) &CreateDXGIFactory1Next);
	RenewHook((PVOID*) &OpenAdapter10Next);
	RenewHook((PVOID*) &OpenAdapter10_2Next);
	return CallNextHookEx(g_hHookUpdate, nCode, wParam, lParam);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			ReadProfile(&g_iSettings);
			if(g_iSettings.m_RouterType == 2)
				return TRUE;
			
			if(g_iSettings.m_RenewHookMode)
			{
				if(g_iSettings.m_RenewHookMode == 2)
					g_hHookUpdate = SetWindowsHookEx(WH_CBT, HookUpdateProc, 0, GetCurrentThreadId());
// Default is now Don't Renew.				
//				g_HookingFlags &= ~DONT_RENEW_OVERWRITTEN_HOOK;
				g_HookingFlags &= RENEW_OVERWRITTEN_HOOKS;
			}
			
			DisableThreadLibraryCalls(hModule);
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
			//_CrtSetBreakAlloc(82291);
			// InitializeMadCHook is needed only if you're using the static madCHook.lib
	
			InitializeMadCHook();
			CHAR sysPath[MAX_PATH];
			CHAR ddraw_dll_full_path[MAX_PATH];
			CHAR d3d8_dll_full_path[MAX_PATH];
			CHAR d3d9_dll_full_path[MAX_PATH];
			CHAR dxgi_dll_full_path[MAX_PATH];
			GetSystemDirectoryA(sysPath, MAX_PATH);
			sprintf_s(ddraw_dll_full_path, "%s\\ddraw.dll",  sysPath);
			sprintf_s(d3d8_dll_full_path, "%s\\d3d8.dll",  sysPath);
			sprintf_s(d3d9_dll_full_path, "%s\\d3d9.dll",  sysPath);
			sprintf_s(dxgi_dll_full_path, "%s\\dxgi.dll",  sysPath);

			GetModuleFileName(hModule, DllPath, sizeof(DllPath));
			_tcsrchr(DllPath, _T('\\'))[1] = '\0';
			_tcscpy_s(DllDX7Path, MAX_PATH, DllPath);
			_tcscpy_s(DllDX8Path, MAX_PATH, DllPath);
			_tcscpy_s(DllDX9Path, MAX_PATH, DllPath);
			_tcscpy_s(DllDX10Path, MAX_PATH, DllPath);
			CollectHooks();
#ifndef FINAL_RELEASE
			InitializeLogging();
#endif
#ifndef WIN64
			// crash with Steam Community
			PathAppend(DllDX7Path, S3DWRAPPERD3D7_DLL_NAME);
			if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(DllDX7Path))
			{
				HookAPI(ddraw_dll_full_path, "DirectDrawCreate", DirectDrawCreateCallback, (PVOID*) &DirectDrawCreateNext, g_HookingFlags);
				HookAPI(ddraw_dll_full_path, "DirectDrawCreateEx", DirectDrawCreateExCallback, (PVOID*) &DirectDrawCreateExNext, g_HookingFlags);
			}
			PathAppend(DllDX8Path, S3DWRAPPERD3D8_DLL_NAME);
			if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(DllDX8Path))
				HookAPI(d3d8_dll_full_path, "Direct3DCreate8", Direct3DCreate8Callback, (PVOID*) &Direct3DCreate8Next, g_HookingFlags);
#endif
			PathAppend(DllDX9Path, S3DWRAPPERD3D9_DLL_NAME);
			if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(DllDX9Path))
			{
				BOOL isHooked = HookAPI(d3d9_dll_full_path, "Direct3DCreate9", Direct3DCreate9Callback, (PVOID*) &Direct3DCreate9Next, g_HookingFlags);
				isHooked = HookAPI(d3d9_dll_full_path, "Direct3DCreate9Ex", Direct3DCreate9ExCallback, (PVOID*) &Direct3DCreate9ExNext, g_HookingFlags);
			}
			TCHAR exeName[MAX_PATH];
			GetModuleFileName(NULL, exeName, _countof(exeName));
			_tcslwr(exeName);
			if (_tcsstr(exeName, _T("\\dwm.exe")) == NULL) // exclusion for Windows 7
			{
				PathAppend(DllDX10Path, S3DWRAPPERD3D10_DLL_NAME);

				CHAR d3d10_driver[MAX_PATH];
				GetDriverModuleName(d3d10_driver, D3D10_VISTA_MODULE_NAME);
				CHAR d3d11_driver[MAX_PATH];
				GetDriverModuleName(d3d11_driver, D3D10_WIN7_MODULE_NAME);

				if (d3d10_driver[0] != '\0' || d3d11_driver[0] != '\0')
				{
					sprintf_s(DriverDllDX10Path, "%s\\%s",  sysPath, d3d10_driver);
					sprintf_s(DriverDllDX11Path, "%s\\%s",  sysPath, d3d11_driver);
					HookAPI(dxgi_dll_full_path, "CreateDXGIFactory", CreateDXGIFactoryCallback, (PVOID*) &CreateDXGIFactoryNext, g_HookingFlags);
					HookAPI(dxgi_dll_full_path, "CreateDXGIFactory1", CreateDXGIFactory1Callback, (PVOID*) &CreateDXGIFactory1Next, g_HookingFlags);

					// for reason, if application call D3D10CreateDevice without calling before CreateDXGIFactory
					if (DriverDllDX10Path[0] != '\0')
						HookAPI(DriverDllDX10Path, "OpenAdapter10", OpenAdapter10Callback, (PVOID*) &OpenAdapter10Next, NO_SAFE_UNHOOKING);
					if (DriverDllDX11Path[0] != '\0')
						HookAPI(DriverDllDX11Path, "OpenAdapter10_2", OpenAdapter10_2Callback, (PVOID*) &OpenAdapter10_2Next, NO_SAFE_UNHOOKING);
				}
			}
			FlushHooks();
			// load libraries explicitly
			g_pExpLib = new ExplicitLibaries( hModule );
			break;
		} 
	case DLL_PROCESS_DETACH:
		{
			if(g_iSettings.m_RenewHookMode == 2)
				UnhookWindowsHookEx(g_hHookUpdate);
			
			if(g_iSettings.m_RouterType == 2)
				return TRUE;

			// free libraries explicitly
			delete g_pExpLib;
			if(g_hHookPolice)
			{
				if(WAIT_TIMEOUT == WaitForSingleObject(g_hHookPolice, 100))
					TerminateThread(g_hHookPolice, 0);
				CloseHandle(g_hHookPolice);
				g_hHookPolice = 0;
			}
			FinalizeMadCHook();
			// do not unload wrapper ddl's.  this prevent crash when uninject occurred while app worked
			_RPT0(0, "S3DInjector memory leaks:\n");
			break;
		}
	}

	return TRUE;
}