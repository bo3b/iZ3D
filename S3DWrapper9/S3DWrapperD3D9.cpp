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
#include <string.h>
#include <setupapi.h>
#include <initguid.h>
#include "Globals.h"
#include <madCHook.h>
#include "Hook.h"
#include "WDirect3DCreate9.h" 
#include <IL/il.h>
#include "BaseStereoRenderer.h"
#include "../S3DAPI/ReadData.h"
//#include <vld.h>

BOOL (WINAPI *Original_SetDeviceGammaRamp)(HDC hDC, LPVOID lpRamp);
BOOL (WINAPI *Original_GetDeviceGammaRamp)(HDC hDC, LPVOID lpRamp);

D3DGAMMARAMP g_DeviceGammaRamp;

static CComModule g_Module;
extern bool g_bD3D9DllAlreadyLoaded;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason) 
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hinstDLL);
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
			_CrtSetReportHookW2( _CRT_RPTHOOK_INSTALL, zlog::VldReportHook );
			//_CrtSetBreakAlloc(66244);
			//VLDSetReportHook( VLD_RPTHOOK_INSTALL, zlog::VldReportHook );
			gData.hModule = hinstDLL;
			InitDirectories();
			DetectShutterMode();
			InitializeMadCHook();
			if (!gInfo.UseMonoDeviceWrapper)
			{
				if(gInfo.OutputCaps & odResetGDIGammaRamp)
					InitGammaRamp();
				HDC dc = GetDC(0);
				GetDeviceGammaRamp(dc, &g_DeviceGammaRamp);
				ReleaseDC(0, dc);

				HookCode(SetDeviceGammaRamp, Hooked_SetDeviceGammaRamp, (PVOID*)&Original_SetDeviceGammaRamp, HOOKING_FLAG);
				HookCode(GetDeviceGammaRamp, Hooked_GetDeviceGammaRamp, (PVOID*)&Original_GetDeviceGammaRamp, HOOKING_FLAG);
			}
			//--- check for "d3d9VistaNoSP1.dll" existance ---
			TCHAR PathD3D9[MAX_PATH];
			GetD3D9DllFullPath(PathD3D9);	
			if(!PathFileExists(PathD3D9))
			{
				DEBUG_MESSAGE(_T("DLL d3d9VistaNoSP1.dll not founded\n"));
				gInfo.FixVistaSP1ResetBug = FALSE;
			}
			g_RenderInfo.OneTimeInitialize();

#ifndef FINAL_RELEASE
			// Initialize DevIL.
			ilInit(); 
#endif 	
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			if (!gInfo.UseMonoDeviceWrapper)
			{
				UnhookCode((PVOID*)&Original_SetDeviceGammaRamp);  
				UnhookCode((PVOID*)&Original_GetDeviceGammaRamp);  
				//--- restore gammaRAMP ---
				HDC dc = GetDC(0);
				SetDeviceGammaRamp(dc, &g_DeviceGammaRamp);
				ReleaseDC(0, dc);
			}
			FinalizeMadCHook();

			if (g_bD3D9DllAlreadyLoaded)
			{
				TCHAR name[MAX_PATH];
				if(GetD3D9DllFullPath(name))
				{
					HMODULE D3D9Handle = GetModuleHandle(name);
					if(D3D9Handle)
						FreeLibrary(D3D9Handle);
				}
			}
			_RPT0(0, "S3DWrapperD3D9 memory leaks:\n");
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
 
	return true; 
} 
