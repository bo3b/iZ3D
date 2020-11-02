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
#include "SetupS3DMonitor.h"
#include <mmsystem.h>
#include "ProductNames.h"
#include "resource.h"
#include "..\CommonUtils\StringResourceManager.h"

bool CheckBackPanel( const TCHAR* model )
{
#ifdef _DEBUG
	return true;
#else
	// iZ3D Old
	return _tcscmp(model, _T("CMO3228") ) == 0 || _tcscmp(model, _T( "CMO3229") ) == 0 || 
		// iZ3D New
		_tcscmp(model, _T("CMO3238") ) == 0 || _tcscmp(model, _T( "CMO3237") ) == 0 ||
		// Matrox TripleHead2Go Digital Edition 
		_tcscmp(model, _T("MTX0510") ) == 0 || _tcscmp(model, _T( "MTX0511") ) == 0;
#endif
}

bool CheckFrontPanel( const TCHAR* model )
{
#ifdef _DEBUG
	return true;
#else
	// iZ3D Old
	return _tcscmp(model, _T( "CMO3228") ) == 0 || _tcscmp(model, _T( "CMO3229" ) ) == 0 || 
		// iZ3D New
		_tcscmp(model, _T( "CMO3239") )== 0 ||
		// Matrox TripleHead2Go Digital Edition 
		_tcscmp(model, _T( "MTX0510") ) == 0 || _tcscmp(model, _T( "MTX0511") ) == 0;
#endif
}

// Setup S3D Monitors

void MonitorPanel::AttachOrRestore()
{
	int maxXResolution = 1680;
	int maxYResolution = 1050;
	//--- attach disabled monitor or/and change resolution, bpp, refresh rate ---
	if(!bAttached)
	{
		deviceMode.dmPelsWidth = maxXResolution;
		deviceMode.dmPelsHeight = maxYResolution;
		deviceMode.dmBitsPerPel = 32;
		deviceMode.dmPosition.x = 0;
		deviceMode.dmPosition.y = 0;
		ChangeSettings();
		ChangeDisplaySettingsEx(0,0,0,0,0);
	}
	else
	{	
		//--- change resolution, bpp, refresh rate on primary monitor ---
		if(deviceMode.dmPelsWidth != maxXResolution || deviceMode.dmPelsHeight != maxYResolution || deviceMode.dmBitsPerPel != 32)
		{
			deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
			deviceMode.dmPelsWidth = maxXResolution;
			deviceMode.dmPelsHeight = maxYResolution;
			deviceMode.dmBitsPerPel = 32;
			ChangeSettings();
			ChangeDisplaySettingsEx(0,0,0,0,0);

			//--- get real primary resolution ---
			deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
			EnumDisplaySettings(szDeviceName, ENUM_CURRENT_SETTINGS, &deviceMode);
		}
	}
}

void MonitorPanel::SetPrimary()
{
	DEVMODE deviceMode;
	deviceMode.dmSize = sizeof(DEVMODE);
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;

	DISPLAY_DEVICE DisplayDevice;
	ZeroMemory(&DisplayDevice, sizeof(DISPLAY_DEVICE));
	DisplayDevice.cb = sizeof(DisplayDevice); 

	for(int i = 0; EnumDisplayDevices(NULL, i, &DisplayDevice, 0); i++)
	{
		if(!(DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) && (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
		{
			EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode);

			//--- switch monitor to primary ---
			deviceMode.dmPosition.x -= deviceMode.dmPosition.x;
			deviceMode.dmPosition.y -= deviceMode.dmPosition.y;
			DWORD dwFlags = CDS_UPDATEREGISTRY | CDS_NORESET;
			if (_tcscmp( szDeviceName, DisplayDevice.DeviceName) == 0)
				dwFlags |= CDS_SET_PRIMARY;
			ChangeDisplaySettingsEx(DisplayDevice.DeviceName, &deviceMode, NULL, dwFlags, NULL);
		}
	}
	ChangeDisplaySettingsEx(0,0,0,0,0);
}

void MonitorPanel::ChangeSettings( DEVMODE* pDeviceMode )
{
	ChangeDisplaySettingsEx(szDeviceName, pDeviceMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
}

void MonitorPanel::ChangeSettings()
{
	ChangeDisplaySettingsEx(szDeviceName, &deviceMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
}

void MonitorPanel::GetDisplaySettings()
{
	deviceMode.dmSize = sizeof(DEVMODE);
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
	if (EnumDisplaySettingsEx(szDeviceName, ENUM_CURRENT_SETTINGS, &deviceMode, 0) == FALSE)
		EnumDisplaySettingsEx(szDeviceName, ENUM_REGISTRY_SETTINGS, &deviceMode, 0);
}

HMONITOR MonitorPanel::GetMonitorHandle()
{
	HMONITOR hm = 0;
	if (bAttached)
	{
		// display is enabled. only enabled displays have a monitor handle
		POINT pt = { deviceMode.dmPosition.x, deviceMode.dmPosition.y };
		hm = MonitorFromPoint(pt, MONITOR_DEFAULTTONULL);
	}
	return hm;
}

void MonitorPanel::ModelToNumber()
{
	nModel = 0;
	if(_tcslen(szModel) > 3)
		_stscanf_s(szModel + 3, _T("%d"), &nModel);
}

void DetectPanels( MonitorPanel * pPrimary, MonitorPanel * pSecondary )
{
	DISPLAY_DEVICE adapterDevice;
	ZeroMemory(&adapterDevice, sizeof(DISPLAY_DEVICE));
	adapterDevice.cb = sizeof(adapterDevice); 
	pPrimary->bFound = FALSE;
	pSecondary->bFound = FALSE;

	for(int i = 0; EnumDisplayDevices(NULL, i, &adapterDevice, 0); i++)
	{
		if(!(adapterDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
		{
			DISPLAY_DEVICE monitorDevice;
			ZeroMemory(&monitorDevice, sizeof(monitorDevice));
			monitorDevice.cb = sizeof(monitorDevice);
			DWORD devMon = 0;

			// please note that this enumeration may not return the correct monitor if multiple monitors
			// are attached. this is because not all display drivers return the ACTIVE flag for the monitor
			// that is actually active
			while (EnumDisplayDevices(adapterDevice.DeviceName, devMon, &monitorDevice, 0))
			{
				if (monitorDevice.StateFlags & DISPLAY_DEVICE_ACTIVE &&
					monitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED) // for ATI, Windows XP
					break;

				devMon++;
			}

			if (!*monitorDevice.DeviceString)
				EnumDisplayDevices(adapterDevice.DeviceName, 0, &monitorDevice, 0);

			if(!pPrimary->bFound && monitorDevice.DeviceID[0] != '\0') //back
			{
				TCHAR* s = _tcschr(monitorDevice.DeviceID, '\\') + 1;
				size_t len = _tcschr(s, '\\') - s;
				if (len >= _countof(pPrimary->szModel))
					len = _countof(pPrimary->szModel) - 1;
				_tcsncpy_s(pPrimary->szModel, s, len);
				if (CheckBackPanel(pPrimary->szModel))
				{
					if(adapterDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
						pPrimary->bAttached = TRUE;
					pPrimary->bPrimary = adapterDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
					pPrimary->bFound = TRUE;
					_tcscpy_s(pPrimary->szDeviceName, adapterDevice.DeviceName);
					continue;
				}
			} 
			if(!pSecondary->bFound && monitorDevice.DeviceID[0] != '\0') //front
			{
				TCHAR* s = _tcschr(monitorDevice.DeviceID, '\\') + 1;
				size_t len = _tcschr(s, '\\') - s;
				if (len >= _countof(pSecondary->szModel))
					len = _countof(pSecondary->szModel) - 1;
				_tcsncpy_s(pSecondary->szModel, s, len);
				if (CheckFrontPanel(pSecondary->szModel))
				{
					if(adapterDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
						pSecondary->bAttached = TRUE;
					pSecondary->bPrimary = adapterDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
					pSecondary->bFound = TRUE;
					_tcscpy_s(pSecondary->szDeviceName, adapterDevice.DeviceName);
					continue;
				}
			}
		}
	}
}

void TurnOnAllMonitors( )
{
	DISPLAY_DEVICE adapterDevice;
	ZeroMemory(&adapterDevice, sizeof(DISPLAY_DEVICE));
	adapterDevice.cb = sizeof(adapterDevice); 

	int c = 0;
	for(int i = 0; EnumDisplayDevices(NULL, i, &adapterDevice, 0); i++)
	{
		if(!(adapterDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) &&
		   !(adapterDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
		{
			// not attached
			DEVMODE	deviceMode;
			ZeroMemory(&deviceMode, sizeof(DEVMODE));
			deviceMode.dmSize = sizeof(DEVMODE);
			deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
			if (EnumDisplaySettingsEx(adapterDevice.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode, 0) == FALSE)
				EnumDisplaySettingsEx(adapterDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &deviceMode, 0);

			//--- attach disabled monitor or/and change resolution, bpp, refresh rate ---
			int maxXResolution = 1680;
			int maxYResolution = 1050;
			deviceMode.dmPelsWidth = maxXResolution;
			deviceMode.dmPelsHeight = maxYResolution;
			deviceMode.dmBitsPerPel = 32;
			deviceMode.dmPosition.x = -10000;
			deviceMode.dmPosition.y = 0;
			ChangeDisplaySettingsEx((LPTSTR)adapterDevice.DeviceName, &deviceMode, NULL, CDS_UPDATEREGISTRY | CDS_NORESET, NULL);
			ChangeDisplaySettingsEx(0,0,0,0,0);
			c++;
		}
	}
}

void GenerateSetupWindowBitmap(HBITMAP &backScreenBitmap, HBITMAP &frontScreenBitmap)
{
	using namespace iz3d::resources;
	//--- read localized strings ---	
	const wchar_t* strFront = TStringResourceSingleton::instance().getString( _T("\\Utils\\BackScreenMsg") );
	const wchar_t* strBack  = TStringResourceSingleton::instance().getString( _T("\\Utils\\FrontScreenMsg") );
	if(!strFront)
		strFront = L"take off the glasses\n and press key '1'";
	if(!strBack)
		strBack = L"take off the glasses\n and press key 'A'";

	//--- create bitmap ---
	int imageWidth = 1400;
	int imageHeight = 320; 
	RECT rc = { 0, 0, imageWidth, imageHeight };

	HDC hDC = CreateCompatibleDC(0); 
	SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	SetBkColor(hDC, 0);
	HFONT hFont = CreateFontW(150,    // Height
		0,                            // Width
		0,                            // Escapement
		0,                            // Orientation
		FW_BOLD,                      // Weight
		FALSE,                        // Italic
		FALSE,                        // Underline
		0,                            // StrikeOut
		DEFAULT_CHARSET,              // CharSet
		OUT_DEFAULT_PRECIS,           // OutPrecision
		CLIP_DEFAULT_PRECIS,          // ClipPrecision
		ANTIALIASED_QUALITY,          // Quality
		DEFAULT_PITCH,                // PitchAndFamily
		_T("Arial Unicode MS"));      // Facename

	backScreenBitmap  = CreateCompatibleBitmap(hDC, imageWidth, imageHeight); 
	frontScreenBitmap = CreateCompatibleBitmap(hDC, imageWidth, imageHeight); 
	HGDIOBJ old = SelectObject(hDC, frontScreenBitmap); 
	FillRect(hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, hFont);
	DrawText(hDC, strFront, (int)wcslen(strFront), &rc, DT_CENTER);
	
	SelectObject(hDC, backScreenBitmap); 
	FillRect(hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, hFont);
	DrawText(hDC, strBack, (int)wcslen(strBack), &rc, DT_CENTER);

	SelectObject(hDC, old);
	DeleteObject(hFont);
}

//------------------------------------ Monitor setup function -------------------------------------
DWORD WINAPI ShowSetupWindow(HMONITOR hBack, HMONITOR hFront, DWORD showTime)
{
	if (!hBack || !hFront)
		return 27;
	MSG msg;
	SynchroWindow sWindow;	
	CComPtr<IPicture> backScreenPicture;
	CComPtr<IPicture> frontScreenPicture;
	//--- HBITMAP backScreenBitmap and frontScreenBitmap will be deleted in SynchroWindow destructor ---
	HBITMAP backScreenBitmap, frontScreenBitmap;
	GenerateSetupWindowBitmap(backScreenBitmap, frontScreenBitmap);
	sWindow.initialize(g_hModule, hBack, hFront, backScreenBitmap, frontScreenBitmap, FALSE);
	sWindow.show();
	DWORD startTime = timeGetTime();

	do
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT)
				break;
			if(msg.message == WM_KEYDOWN)
				if ((msg.wParam == '1' || msg.wParam == VK_NUMPAD1 || msg.wParam == 'A' || msg.wParam == 27))
					return (DWORD)msg.wParam;
		}
		Sleep(0);
	}	while(timeGetTime() - startTime < showTime*1000);

	return 27;
}

void __declspec(dllexport) CALLBACK SetupS3DMonitor(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{	
	using namespace iz3d::resources;
	//--- read localized strings ---
	TStringResourceSingleton::instance().setLanguage( GetCurrentLanguage().c_str() );
	MonitorPanel	first, second;
	MonitorPanel	*pPrimary, *pSecondary;
	pPrimary = &first;
	pSecondary = &second;
	DetectPanels(pPrimary, pSecondary);

	if (!pPrimary->bFound || !pSecondary->bFound)
	{
		TurnOnAllMonitors();
		DetectPanels(pPrimary, pSecondary);
	}

	// 1. enable primary & secondary 
	// 2. detect monitors
	// 3. disable secondary monitor (for icons)
	// 4. enable secondary monitor 
	// 5. set primary(back) as primary monitor
	if (pPrimary->bFound && pSecondary->bFound)
	{
		pPrimary->ModelToNumber();
		pSecondary->ModelToNumber();

		// Old new model
		if (pPrimary->nModel == 3229)
			pPrimary->nModel = 3228;
		if (pSecondary->nModel == 3229)
			pSecondary->nModel = 3228;

		// Matrox
		if (pPrimary->nModel == 0510 || pPrimary->nModel == 0511)
			pPrimary->nModel = 3228;
		if (pSecondary->nModel == 0510 || pSecondary->nModel == 0511)
			pSecondary->nModel = 3228;

		if (pPrimary->nModel == 3228 && pSecondary->nModel == 3228 && pSecondary->bPrimary)
		{
			MonitorPanel*	pTemp;
			pTemp = pPrimary;
			pPrimary = pSecondary;
			pSecondary = pTemp;
		}

		pSecondary->GetDisplaySettings();
		pSecondary->AttachOrRestore();

		pPrimary->GetDisplaySettings();
		pPrimary->AttachOrRestore();

		if (pPrimary->nModel == 3228 && pSecondary->nModel == 3228)
		{
			HMONITOR hBack = pPrimary->GetMonitorHandle();
			HMONITOR hFront = pSecondary->GetMonitorHandle();
			DWORD keyValue = ShowSetupWindow(hBack, hFront, 60);
			if(keyValue == 27)
				return;
			if(keyValue == 'A')
			{
				MonitorPanel*	pTemp;
				pTemp = pPrimary;
				pPrimary = pSecondary;
				pSecondary = pTemp;
			}
		}
		else
		{
			if(pPrimary->nModel == 3239 || pPrimary->nModel == 3229 || pPrimary->nModel == 4932)
			{
				MonitorPanel*	pTemp;
				pTemp = pPrimary;
				pPrimary = pSecondary;
				pSecondary = pTemp;
			}
		}

		//--- Attach and detach primary or secondary monitors ---

		//--- switch primary ---
		//--- on NVidia cards monitor will be primary again if other was not set as primary ---
		if(!pPrimary->bPrimary)
		{
			pPrimary->SetPrimary();
			pSecondary->deviceMode.dmPosition.x = -100000;
			pSecondary->deviceMode.dmPosition.y = 0;
		}

		//--- detach monitor for icon moving ---
		DEVMODE deviceMode;
		ZeroMemory(&deviceMode, sizeof(DEVMODE));
		deviceMode.dmSize = sizeof(DEVMODE);
		deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;
		pSecondary->ChangeSettings(&deviceMode);
		ChangeDisplaySettingsEx(0,0,0,0,0);

		//--- attach monitor ---
		pSecondary->ChangeSettings();
		ChangeDisplaySettingsEx(0,0,0,0,0);
	}
	/*else
	{
		if(lpszCmdLine == NULL || strstr(lpszCmdLine, "/SILENT") == NULL)
			MessageBox(NULL, TStringResourceSingleton::instance().getString( _T("\\Utils\\MonitorNotFound") ), TEXT("Control Center"), MB_ICONWARNING | MB_OK);
	}*/
}
