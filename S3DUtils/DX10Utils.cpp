#include "stdafx.h"
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <atlbase.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <Windows.h>
#include <ddraw.h>
#include "DX10Utils.h"
#include "..\CommonUtils\System.h"
#pragma comment(lib, "wbemuuid.lib")

void __declspec(dllexport) CALLBACK GetAndWriteToRegistryDriverModuleName(HWND /*hwnd*/, HINSTANCE /*hinst*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
	// Initialize COM. ------------------------------------------
	HRESULT hr =  CoInitializeEx(0, COINIT_MULTITHREADED); 
	if (FAILED(hr))
		return;

	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------

	hr =  CoInitializeSecurity(
		NULL, 
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);

	if (FAILED(hr))
		return;

	WCHAR name[MAX_PATH];
	if(GetDriverModuleName("OpenAdapter", name))
		WriteDriverModuleName(D3D9_VISTA_MODULE_NAME, name);
	else
		WriteDriverModuleName(D3D9_VISTA_MODULE_NAME, NULL);

	if(GetDriverModuleName("OpenAdapter10", name))
		WriteDriverModuleName(D3D10_VISTA_MODULE_NAME, name);
	else
		WriteDriverModuleName(D3D10_VISTA_MODULE_NAME, NULL);
	if(GetDriverModuleName("OpenAdapter10_2", name))
		WriteDriverModuleName(D3D10_WIN7_MODULE_NAME, name);
	else
		WriteDriverModuleName(D3D10_WIN7_MODULE_NAME, NULL);

	// Cleanup
	CoUninitialize();
}

void __declspec(dllexport) CALLBACK RemoveFromRegistryDriverModuleName(HWND /*hwnd*/, HINSTANCE /*hinst*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
	WriteDriverModuleName(D3D9_VISTA_MODULE_NAME, NULL);
	WriteDriverModuleName(D3D10_VISTA_MODULE_NAME, NULL);
	WriteDriverModuleName(D3D10_WIN7_MODULE_NAME, NULL);
}

void WriteDriverModuleName(LPCWSTR KeyName, LPCWSTR ModuleName)
{
	HKEY hDriver;
	REGSAM flags = KEY_WRITE | KEY_READ | KEY_CREATE_SUB_KEY;
#ifndef WIN64
	LPCTSTR key = _T("SOFTWARE\\iZ3D\\iZ3D Driver\\Win32");
#else
	LPCTSTR key = _T("SOFTWARE\\iZ3D\\iZ3D Driver\\Win64");
#endif
	if (RegCreateKeyEx(HKEY_CURRENT_USER,
		key,
		0,
		NULL,
		0,
		flags,
		0,
		&hDriver,
		NULL) == ERROR_SUCCESS)
	{
		if(ModuleName)
		{
			DWORD nModuleNameLen = (DWORD)wcslen(ModuleName) + 1;
			RegSetValueExW(hDriver, KeyName, NULL, REG_SZ, (LPBYTE)ModuleName, nModuleNameLen * sizeof(WCHAR));
		}
		else
			RegDeleteKeyW(hDriver, KeyName);
		RegCloseKey(hDriver);
	}
}

bool CheckModuleFunction(LPCSTR functionName, LPCWSTR ModuleName)
{
	bool res = false;
	HMODULE hModule = LoadLibraryW(ModuleName);
	if(hModule)
	{
		if(GetProcAddress(hModule, functionName) != NULL)
			res = true;
		FreeLibrary(hModule);
	}
	return res;
}

bool ParseModules( WCHAR * ModuleName, LPCWSTR driverModule, LPCSTR functionName );

struct DDRAW_MATCH
{
	HMONITOR hMonitor;
	CHAR strDriverName[512];
	bool bFound;
};

//-----------------------------------------------------------------------------
BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
{
	UNREFERENCED_PARAMETER( lpDriverDescription );

	DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
	if( pDDMatch->hMonitor == hm )
	{
		pDDMatch->bFound = true;
		strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
	}
	return TRUE;
}

HMONITOR GetPrimaryMonitorHandle()
{
	const POINT ptZero = { 0, 0 };
	return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
}

HRESULT GetPrimaryDeviceID( WCHAR* strDeviceID, int cchDeviceID )
{
	HINSTANCE hInstDDraw;

	hInstDDraw = LoadLibrary( L"ddraw.dll" );
	if( hInstDDraw )
	{
		DDRAW_MATCH match;
		ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
		match.hMonitor = GetPrimaryMonitorHandle();

		LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
		pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

		if( pDirectDrawEnumerateEx )
			pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );

		if( match.bFound )
		{
			LONG iDevice = 0;
			DISPLAY_DEVICEA dispdev;

			ZeroMemory( &dispdev, sizeof( dispdev ) );
			dispdev.cb = sizeof( dispdev );

			while( EnumDisplayDevicesA( NULL, iDevice, ( DISPLAY_DEVICEA* )&dispdev, 0 ) )
			{
				// Skip devices that are monitors that echo another display
				if( dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
				{
					iDevice++;
					continue;
				}

				// Skip devices that aren't attached since they cause problems
				if( ( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
				{
					iDevice++;
					continue;
				}

				if( _stricmp( match.strDriverName, dispdev.DeviceName ) == 0 )
				{
					MultiByteToWideChar( CP_ACP, 0, dispdev.DeviceID, -1, strDeviceID, cchDeviceID );
					return S_OK;
				}

				iDevice++;
			}
		}

		FreeLibrary( hInstDDraw );
	}

	return E_FAIL;
}

bool GetDriverModuleName(/* in */ LPCSTR functionName, /* out */ WCHAR ModuleName[MAX_PATH])
{ 
	WCHAR strInputDeviceID[512];
	GetPrimaryDeviceID( strInputDeviceID, 512 );

	HRESULT hr;
	bool bModuleFound = false;
	ModuleName[0] = '\0';

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	CComPtr<IWbemLocator> pLoc;
	hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
	if (SUCCEEDED(hr))
	{
		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method

		CComPtr<IWbemServices> pSvc;

		// Connect to the root\cimv2 namespace with
		// the current user and obtain pointer pSvc
		// to make IWbemServices calls.
		hr = pLoc->ConnectServer(
			bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (e.g. Kerberos)
			0,                       // Context object 
			&pSvc                    // pointer to IWbemServices proxy
			);

		if (SUCCEEDED(hr))
		{
			// Step 5: --------------------------------------------------
			// Set security levels on the proxy -------------------------

			hr = CoSetProxyBlanket(
				pSvc,                        // Indicates the proxy to set
				RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
				RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
				NULL,                        // Server principal name 
				RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
				RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
				NULL,                        // client identity
				EOAC_NONE                    // proxy capabilities 
				);

			if (SUCCEEDED(hr))
			{
				// Step 6: --------------------------------------------------
				// Use the IWbemServices pointer to make requests of WMI ----

				// For example, get the name of the operating system
				CComPtr<IEnumWbemClassObject> pEnumerator;
				hr = pSvc->ExecQuery(
					bstr_t("WQL"), 
					bstr_t("SELECT * FROM Win32_VideoController"),
					WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
					NULL,
					&pEnumerator);

				if (SUCCEEDED(hr))
				{
					// Step 7: -------------------------------------------------
					// Get the data from the query in step 6 -------------------
					// --- search for first suitable Module ---
					while (pEnumerator || !bModuleFound)
					{
						ULONG uReturn = 0;
						CComPtr<IWbemClassObject> pclsObj;
						HRESULT hr = pEnumerator->Next(60000, 1, &pclsObj, &uReturn);

						if(0 == uReturn)
							break;

						VARIANT vtProp;
						hr = pclsObj->Get( bstr_t("PNPDeviceID"), 0L, &vtProp, NULL, NULL );
						if( FAILED( hr ) || wcsstr( vtProp.bstrVal, strInputDeviceID ) == 0 )
						{
							VariantClear(&vtProp);
							continue;
						}
						VariantClear(&vtProp);

						// Get the value of the Name property
						hr = pclsObj->Get(bstr_t("InstalledDisplayDrivers"), 0, &vtProp, 0, 0);
						if (vtProp.vt == VT_BSTR)
						{
							//--- convert BSTR to WCHAR ---
							//--- possible incorrect conversion ---
							LPCWSTR driverModule = (LPCWSTR)vtProp.bstrVal;
							bModuleFound = ParseModules(ModuleName, driverModule, functionName);

							VariantClear(&vtProp);
							if (bModuleFound)
								break;
						}
						VariantClear(&vtProp);
					}
				}
			}
		} 
	}
	return bModuleFound;
}

bool ParseModules( WCHAR * ModuleName, LPCWSTR driverModule, LPCSTR functionName )
{
	bool bModuleFound = false;
	//--- parse Module names and analyze each ---
	const WCHAR *s1 = driverModule;
	do
	{
		WCHAR *s2 = ModuleName;
		while(*s1 != 0 && *s1 != ',' && *s1 != ' ')
			*s2++ = *s1++;
		if(*s1 == ',' || *s1 == ' ')
			s1++;
		*s2 = 0;
		if (wcschr(ModuleName, L'.') == NULL)
			wcscat(ModuleName, L".dll");
		if(CheckModuleFunction(functionName, ModuleName))
		{
			bModuleFound = true;
			break;
		}
	} while(*s1);	
	return bModuleFound;
}
