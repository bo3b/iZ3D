//

#include "stdafx.h"
#include "AdapterFunctions.h"
#include "D3DDeviceWrapper.h"

static union{
	D3D10DDI_ADAPTERFUNCS	OriginalAdapterFuncs;
	D3D10_2DDI_ADAPTERFUNCS	OriginalAdapterFuncs2;
};

#define GET_ORIG()		OriginalAdapterFuncs
#define GET_ORIG2()		OriginalAdapterFuncs2

/*ID3D10Device *GetD3DDevicePtr(D3D10DDI_HRTDEVICE hRTDevice)
{
	UINT_PTR Offset;
	// some magic will happen here
#ifndef WIN64
	Offset = 0x24;
#else
	Offset = 0x40;
#endif
	void* pD3DDev = (void*)((UINT_PTR)hRTDevice.handle - Offset);
	__try
	{
		if (!IsBadReadPtr(pD3DDev, sizeof(void*)))
		{
			void** p = *(void***)pD3DDev;	
			if (!IsBadReadPtr(p, sizeof(void*)))
			{
				UINT_PTR firstMethod = (UINT_PTR)p[0];
				// only if release, should be in d3d10core.dll
				if (CheckCodeInDll(_T("d3d10core.dll"), firstMethod) || 
					CheckCodeInDll(_T("d3d10_1core.dll"), firstMethod))
					return (ID3D10Device*) pD3DDev;
			}
		}

		//try debug
#ifndef WIN64
		Offset = 0x11D0;
#else
		Offset = 0x1B60;
#endif
		pD3DDev = (void*)((UINT_PTR)hRTDevice.handle - Offset);
		if (!IsBadReadPtr(pD3DDev, sizeof(void*)))
		{
			void** p = *(void***)pD3DDev;	
			if (!IsBadReadPtr(p, sizeof(void*)))
			{
				UINT_PTR firstMethod = (UINT_PTR)p[0];
				if (CheckCodeInDll(_T("d3d10sdklayers.dll"), firstMethod))
					return (ID3D10Device*) pD3DDev;
			}
		}

		//try debug
#ifndef WIN64
		Offset = 0x1190;
#else
		Offset = 0x1B20;
#endif
		pD3DDev = (void*)((UINT_PTR)hRTDevice.handle - Offset);
		if (!IsBadReadPtr(pD3DDev, sizeof(void*)))
		{
			void** p = *(void***)pD3DDev;	
			if (!IsBadReadPtr(p, sizeof(void*)))
			{
				UINT_PTR firstMethod = (UINT_PTR)p[0];
				if (CheckCodeInDll(_T("d3d10sdklayers.dll"), firstMethod))
					return (ID3D10Device*) pD3DDev;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	DEBUG_MESSAGE(_T("Can't get D3D10 Device address!\n"));
	return NULL;
}*/

#define D3D10_0_w_DDI_MINOR_VERSION 3	// Vista SP2 with platform update
#define D3D10_1_w_DDI_MINOR_VERSION 4	// Vista SP2 with platform update
#define D3D11_0_w_DDI_MINOR_VERSION 9

TD3DVersion GetD3DVersionFromInterface( UINT Interface )
{
	int DDIMajorVersion = Interface >> 16;
	int DDIMinorVersion = Interface & 0xFFFF;
	if ( DDIMajorVersion == D3D10_DDI_MAJOR_VERSION )
	{
		switch(DDIMinorVersion)
		{
		case D3D10_0_DDI_MINOR_VERSION:
		case D3D10_0_w_DDI_MINOR_VERSION:
		case D3D10_0_x_DDI_MINOR_VERSION:
		case D3D10_0_7_DDI_MINOR_VERSION:
			return TD3DVersion_10_0;

		case D3D10_1_DDI_MINOR_VERSION:
		case D3D10_1_w_DDI_MINOR_VERSION:
		case D3D10_1_x_DDI_MINOR_VERSION:
		case D3D10_1_7_DDI_MINOR_VERSION:
			return TD3DVersion_10_1;

		case D3D10on9_DDI_MINOR_VERSION:
			return TD3DVersion_Unknown;
		}
	}
	else if ( DDIMajorVersion == D3D11_DDI_MAJOR_VERSION )
	{
		switch(DDIMinorVersion)
		{
		case D3D11_0_w_DDI_MINOR_VERSION:
		case D3D11_0_DDI_MINOR_VERSION:
		case D3D11_0_7_DDI_MINOR_VERSION:
			return TD3DVersion_11_0;
		}
	}
	return TD3DVersion_Unknown;
}

bool IsAcceptableD3DInterfaceVersion( UINT Interface )
{
	TD3DVersion ver = GetD3DVersionFromInterface( Interface );
	if (ver == TD3DVersion_10_0)
		return true;
	else if (ver == TD3DVersion_10_1)
		return true;
	else if (ver == TD3DVersion_11_0)
		return true;
	return false;
}

PFND3D10DDI_SETERROR_CB SetErrorCB_Orig = NULL;
void (APIENTRY  SetErrorCB)( D3D10DDI_HRTCORELAYER hRTCoreLayer, HRESULT hr )
{
	if (hr != DXGI_DDI_ERR_WASSTILLDRAWING)
	{
		HRESULT hrRemoved = D3DERR_DEVICEREMOVED;
#ifndef FINAL_RELEASE
		if (hr != hrRemoved) {
			DEBUG_MESSAGE(_T("Video Driver error occur, hr = %s(0x%X)\n"), DXGetErrorString(hr), hr);
		} else {
			DEBUG_MESSAGE(_T("Video Driver error occur, hr = D3DERR_DEVICEREMOVED\n"));
		}
		DEBUG_MESSAGE(_T("Previous command: %S\n"), CommandIDToString(Commands::Command::LastCommandId_));
#endif
		_ASSERT(hr != hrRemoved);
	}
	SetErrorCB_Orig(hRTCoreLayer, hr);
}

SIZE_T (APIENTRY  CalcPrivateDeviceSize)( D3D10DDI_HADAPTER  hAdapter, CONST D3D10DDIARG_CALCPRIVATEDEVICESIZE*  pData )
{
	SIZE_T size = GET_ORIG().pfnCalcPrivateDeviceSize(hAdapter, pData);
	size_t wrapperSize = gInfo.UseMonoDeviceWrapper ? sizeof(D3DMonoDeviceWrapper) : sizeof(D3DDeviceWrapper);
	size += wrapperSize;
	return size;
}

HRESULT (APIENTRY  CreateDevice)( D3D10DDI_HADAPTER  hAdapter, D3D10DDIARG_CREATEDEVICE*  pCreateData)
{
	if ( IsInternalCall() )
	{
		HRESULT hResult = S_OK;
		NSCALL( GET_ORIG().pfnCreateDevice( hAdapter, pCreateData ) );
		return hResult;
	}

	int DDIMajorVersion = pCreateData->Interface >> 16;
	int DDIMinorVersion = pCreateData->Interface & 0xFFFF;
	DEBUG_MESSAGE(_T("DDI Interface %d.%d\n"), DDIMajorVersion, DDIMinorVersion);
	int DriverMajorVersion = pCreateData->Version >> 16;
	int DriverMinorVersion = pCreateData->Version & 0xFFFF;
	DEBUG_MESSAGE(_T("Driver Version %d.%d\n"), DriverMajorVersion, DriverMinorVersion);

	HRESULT hResult;
	if (!IsAcceptableD3DInterfaceVersion(pCreateData->Interface))
	{
		DEBUG_MESSAGE(_T("Warning: Unsupported version\n"));
		NSCALL(GET_ORIG().pfnCreateDevice(hAdapter, pCreateData));
		return hResult;
	}

	void* pWrp;
	D3D10DDI_HDEVICE  hOrigDevice = pCreateData->hDrvDevice;
	InitWrapper(hOrigDevice, pWrp);
	size_t wrapperSize = gInfo.UseMonoDeviceWrapper ? sizeof(D3DMonoDeviceWrapper) : sizeof(D3DDeviceWrapper);
	D3D10DDI_HDEVICE hDev = { ((BYTE*)hOrigDevice.pDrvPrivate + wrapperSize) }; 
	pCreateData->hDrvDevice = hDev;
	NSCALL(GET_ORIG().pfnCreateDevice(hAdapter, pCreateData));
	if (SUCCEEDED(hResult))
	{
		if(gInfo.UseMonoDeviceWrapper)
		{
			D3DMonoDeviceWrapper* pWrapper = (D3DMonoDeviceWrapper*)pWrp; 
			new(pWrapper) D3DMonoDeviceWrapper();
			g_pLastD3DDevice = (D3DDeviceWrapper*)pWrapper;		

			pWrapper->hDevice = pCreateData->hDrvDevice;
			pWrapper->SetInterfaceVersion( pCreateData->Interface );
			TD3DVersion ver = pWrapper->GetD3DVersion();		
			if ( ver == TD3DVersion_10_0 )
			{
				DEBUG_MESSAGE(_T("D3D 10.0\n"));
				D3D10DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->pDeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else if ( ver == TD3DVersion_10_1 )
			{
				DEBUG_MESSAGE(_T("D3D 10.1\n"));
				D3D10_1DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->p10_1DeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else if ( ver == TD3DVersion_11_0 )
			{
				DEBUG_MESSAGE(_T("D3D 11.0\n"));
				D3D11DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->p11DeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else 
			{
				// Try rebuild project
				_ASSERT(FALSE);
			}
			if (!IS_DXGI1_1_BASE_FUNCTIONS(pCreateData->Interface, pCreateData->Version))
			{
				DEBUG_MESSAGE(_T("DXGI 1.0\n"));
				DXGI_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions = pCreateData->DXGIBaseDDI.pDXGIDDIBaseFunctions;
				pWrapper->HookDXGIFuncs(pDXGIDDIBaseFunctions);			
			}
			else
			{
				DEBUG_MESSAGE(_T("DXGI 1.1\n"));
				DXGI1_1_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions = pCreateData->DXGIBaseDDI.pDXGIDDIBaseFunctions2;
				pWrapper->HookDXGIFuncs(pDXGIDDIBaseFunctions);	
			}
		}
		else
		{
			D3DDeviceWrapper* pWrapper = (D3DDeviceWrapper*)pWrp;
			new(pWrapper) D3DDeviceWrapper();
			g_pLastD3DDevice = pWrapper;		

			pWrapper->hDevice = pCreateData->hDrvDevice;
			pWrapper->dwCreationFlags = pCreateData->Flags;
			pWrapper->SetInterfaceVersion( pCreateData->Interface );
			TD3DVersion ver = pWrapper->GetD3DVersion();		
			if ( ver == TD3DVersion_10_0 )
			{
				DEBUG_MESSAGE(_T("D3D 10.0\n"));
				D3D10DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->pDeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else if ( ver == TD3DVersion_10_1 )
			{
				DEBUG_MESSAGE(_T("D3D 10.1\n"));
				D3D10_1DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->p10_1DeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else if ( ver == TD3DVersion_11_0 )
			{
				DEBUG_MESSAGE(_T("D3D 11.0\n"));
				D3D11DDI_DEVICEFUNCS *pDeviceFuncs = pCreateData->p11DeviceFuncs;
				pWrapper->HookDeviceFuncs(pDeviceFuncs);
			}
			else 
			{
				// Try rebuild project
				_ASSERT(FALSE);
			}
			if (!IS_DXGI1_1_BASE_FUNCTIONS(pCreateData->Interface, pCreateData->Version))
			{
				DEBUG_MESSAGE(_T("DXGI 1.0\n"));
				DXGI_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions = pCreateData->DXGIBaseDDI.pDXGIDDIBaseFunctions;
				pWrapper->HookDXGIFuncs(pDXGIDDIBaseFunctions);			
			}
			else
			{
				DEBUG_MESSAGE(_T("DXGI 1.1\n"));
				DXGI1_1_DDI_BASE_FUNCTIONS  *pDXGIDDIBaseFunctions = pCreateData->DXGIBaseDDI.pDXGIDDIBaseFunctions2;
				pWrapper->HookDXGIFuncs(pDXGIDDIBaseFunctions);	
			}
#ifndef FINAL_RELEASE
			pWrapper->m_ResourceManager.OpenFile(hOrigDevice);
#endif
			pWrapper->CreateOutput();
		}
#ifndef FINAL_RELEASE
		D3D10DDI_CORELAYER_DEVICECALLBACKS  *pD3D10UMFunctions = (D3D10DDI_CORELAYER_DEVICECALLBACKS*)pCreateData->pUMCallbacks;
		SetErrorCB_Orig = pD3D10UMFunctions->pfnSetErrorCb;
		pD3D10UMFunctions->pfnSetErrorCb = SetErrorCB;
#endif
	}
	
	pCreateData->hDrvDevice = hOrigDevice;
	return hResult;
}

HRESULT (APIENTRY  CloseAdapter)( D3D10DDI_HADAPTER  hAdapter )
{
	HRESULT hResult = GET_ORIG().pfnCloseAdapter(hAdapter);
	return hResult;
}

TCHAR* EnumToString(TD3DVersion ver)
{
	switch ( ver )
	{
	case TD3DVersion_10_0:
		return _T("D3D 10.0");
	case TD3DVersion_10_1:
		return _T("D3D 10.1");
	case TD3DVersion_11_0:
		return _T("D3D 11.0");
	default:
		return _T("Unknown");
	}
}

HRESULT (APIENTRY  GetSupportedVersions)( D3D10DDI_HADAPTER  hAdapter, UINT32* puEntries, UINT64* pSupportedDDIInterfaceVersions)
{
	HRESULT hResult = GET_ORIG2().pfnGetSupportedVersions(hAdapter, puEntries, pSupportedDDIInterfaceVersions);
	if (SUCCEEDED(hResult) && pSupportedDDIInterfaceVersions)
	{
		DEBUG_MESSAGE(_T("\tDriver support version:\n"));
		for(UINT32 i = 0; i < *puEntries; i++)
		{
			int MajorVersion = (pSupportedDDIInterfaceVersions[i] >> 48) & 0xFFFF;
			int MinorVersion = (pSupportedDDIInterfaceVersions[i] >> 32) & 0xFFFF;
			int BuildVersion = (pSupportedDDIInterfaceVersions[i] >> 16) & 0xFFFF;
			TD3DVersion ver = GetD3DVersionFromInterface( pSupportedDDIInterfaceVersions[i] >> 32 );
			DEBUG_MESSAGE(_T("\t\tD3D Interface %d.%d.%d (%s)\n"), MajorVersion, MinorVersion, BuildVersion, EnumToString(ver));
		}
	}
	return hResult;
}

HRESULT (APIENTRY  GetCaps)( D3D10DDI_HADAPTER  hAdapter, CONST D3D10_2DDIARG_GETCAPS* pCaps)
{
	HRESULT hResult = GET_ORIG2().pfnGetCaps(hAdapter, pCaps);
	switch (pCaps->Type)
	{
	case D3D11DDICAPS_THREADING:
		{
			_ASSERT(pCaps->DataSize == sizeof(D3D11DDI_THREADING_CAPS));
			D3D11DDI_THREADING_CAPS*  pData = (D3D11DDI_THREADING_CAPS*)pCaps->pData;
			if (pData->Caps & D3D11DDICAPS_FREETHREADED)
			{
				DEBUG_MESSAGE(_T("\tDriver support free threading\n"));
			}
			if (pData->Caps & D3D11DDICAPS_COMMANDLISTS )
			{
				DEBUG_MESSAGE(_T("\tDriver support command list\n"));
				pData->Caps &= ~D3D11DDICAPS_COMMANDLISTS;
			}
			else
			{
				DEBUG_MESSAGE(_T("\tDriver NOT support command list\n"));
			}
		}
		break;
	case D3D11DDICAPS_SHADER:
		{
			_ASSERT(pCaps->DataSize == sizeof(D3D11DDI_SHADER_CAPS));
			D3D11DDI_SHADER_CAPS*  pData = (D3D11DDI_SHADER_CAPS*)pCaps->pData;
		}
		break;
	case D3D11DDICAPS_3DPIPELINESUPPORT:
		{
			_ASSERT(pCaps->DataSize == sizeof(D3D11DDI_3DPIPELINESUPPORT_CAPS));
			D3D11DDI_3DPIPELINESUPPORT_CAPS*  pData = (D3D11DDI_3DPIPELINESUPPORT_CAPS*)pCaps->pData;
			if (pData->Caps & D3D11DDI_ENCODE_3DPIPELINESUPPORT_CAP(D3D11DDI_3DPIPELINELEVEL_10_0))
			{
				DEBUG_MESSAGE(_T("\tD3D11DDI_3DPIPELINELEVEL_10_0\n"));
			}
			if (pData->Caps & D3D11DDI_ENCODE_3DPIPELINESUPPORT_CAP(D3D11DDI_3DPIPELINELEVEL_10_1))
			{
				DEBUG_MESSAGE(_T("\tD3D11DDI_3DPIPELINELEVEL_10_1\n"));
			}
			if (pData->Caps & D3D11DDI_ENCODE_3DPIPELINESUPPORT_CAP(D3D11DDI_3DPIPELINELEVEL_11_0))
			{
				DEBUG_MESSAGE(_T("\tD3D11DDI_3DPIPELINELEVEL_11_0\n"));
			}
		}
		break;
	}
	return hResult;
}

#define D3D10_VISTA_MODULE_NAME	_T("DriverD3D10VistaModule")
#define D3D10_WIN7_MODULE_NAME	_T("DriverD3D10Win7Module")

void GetDriverModuleName(LPTSTR ModuleName, LPCTSTR Key)
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
		DWORD Size = MAX_PATH * sizeof(TCHAR);
		if (RegQueryValueEx(hDriver, Key, NULL, NULL, (BYTE*)ModuleName, &Size))
			Size = 0;
		ModuleName[Size / sizeof(TCHAR)] = 0;
		RegCloseKey(hDriver);
	}
}

S3DWRAPPER10_API HRESULT WINAPI OpenAdapter10(D3D10DDIARG_OPENADAPTER* pOpenData )
{
	HRESULT hResult = S_OK;
	TCHAR moduleName[MAX_PATH];
	GetDriverModuleName(moduleName, D3D10_VISTA_MODULE_NAME);
	TCHAR szBuffer[MAX_PATH];
	UINT nSize = GetSystemDirectory(szBuffer, MAX_PATH - 1);
	TCHAR szFileName[MAX_PATH];
	_stprintf_s(szFileName, _T("%s\\%s"), szBuffer, moduleName);
	//MAGIC OPTIMIZATION
	HMODULE hD3D10Lib = GetModuleHandle(moduleName);
	if (!hD3D10Lib)
		hD3D10Lib = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hD3D10Lib)
		return E_FAIL;
	PFND3D10DDI_OPENADAPTER pOpenAdapter10 = NULL;
	pOpenAdapter10 = (PFND3D10DDI_OPENADAPTER)GetProcAddress(hD3D10Lib, "OpenAdapter10");
	if (!pOpenAdapter10)
	{
		DEBUG_TRACE1(_T("GetProcAddress(OpenAdapter10) FAILED\n"));
		return E_FAIL;
	}
	hResult = pOpenAdapter10(pOpenData);
	if (SUCCEEDED(hResult))
	{
		memcpy(&OriginalAdapterFuncs, pOpenData->pAdapterFuncs, sizeof D3D10DDI_ADAPTERFUNCS);
#define SET_FUNC(x) pOpenData->pAdapterFuncs->pfn##x = ##x;
		SET_FUNC(CalcPrivateDeviceSize);
		SET_FUNC(CreateDevice);
		SET_FUNC(CloseAdapter);
#undef SET_FUNC
	}
	DEBUG_TRACE1(_T("OpenAdapter10(pOpenData) result=0x%X\n"), hResult );
	return hResult;
}

S3DWRAPPER10_API HRESULT WINAPI OpenAdapter10_2(D3D10DDIARG_OPENADAPTER* pOpenData )
{
	HRESULT hResult = S_OK;
	TCHAR moduleName[MAX_PATH];
	GetDriverModuleName(moduleName, D3D10_WIN7_MODULE_NAME);
	TCHAR szBuffer[MAX_PATH];
	UINT nSize = GetSystemDirectory(szBuffer, MAX_PATH - 1);
	TCHAR szFileName[MAX_PATH];
	_stprintf_s(szFileName, _T("%s\\%s"), szBuffer, moduleName);
	//MAGIC OPTIMIZATION
	HMODULE hD3D10Lib = GetModuleHandle(moduleName);
	if (!hD3D10Lib)
		hD3D10Lib = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hD3D10Lib)
		return E_FAIL;
	PFND3D10DDI_OPENADAPTER pOpenAdapter10 = NULL;
	pOpenAdapter10 = (PFND3D10DDI_OPENADAPTER)GetProcAddress(hD3D10Lib, "OpenAdapter10_2");
	if (!pOpenAdapter10)
	{
		DEBUG_TRACE1(_T("GetProcAddress(OpenAdapter10_2) FAILED\n"));
		return E_FAIL;
	}
	hResult = pOpenAdapter10(pOpenData);
	if (SUCCEEDED(hResult))
	{
		memcpy(&OriginalAdapterFuncs2, pOpenData->pAdapterFuncs_2, sizeof D3D10_2DDI_ADAPTERFUNCS);
#define SET_FUNC(x) pOpenData->pAdapterFuncs_2->pfn##x = ##x;
		SET_FUNC(CalcPrivateDeviceSize);
		SET_FUNC(CreateDevice);
		SET_FUNC(CloseAdapter);
		SET_FUNC(GetSupportedVersions);
		SET_FUNC(GetCaps);
#undef SET_FUNC
	}
	DEBUG_TRACE1(_T("OpenAdapter10_2(pOpenData) result=0x%X\n"), hResult );
	return hResult;
}
