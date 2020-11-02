// S3DWrapper10.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include "S3DWrapperD3D10.h"
#include <initguid.h>
#include "..\S3DAPI\Logging.h"
#include "..\S3DAPI\GlobalData.h"
#include "..\S3DAPI\ReadData.h"
#include "../S3DAPI/ShutterAPI.h"
#include "DXGIFactoryWrapper.h"
#include <madchook.h>
#include <IL/il.h>
#include "../OutputMethods/OutputLib/OutputData.h"
//#include <vld.h>

#define HOOKING_FLAG NO_SAFE_UNHOOKING | DONT_RENEW_OVERWRITTEN_HOOK | RESTORE_CODE_BEFORE_HOOK

static GUID g_magicTag =  { 0x5f9fd868, 0xb2a7, 0xbf42, 0xaa, 0x13, 0xe3, 0x39, 0x34, 0xe1, 0x2a, 0x20  };

__declspec(dllexport) DWORD WINAPI SetSyncRenderCallback(void *p)
{
	static void* i;
	if(!p)
		i = p;
	else 
		return (DWORD)-1;
	return (DWORD)&g_magicTag;
}

__declspec(dllexport) DWORD WINAPI StartDDAServer()
{
	return gInfo.DisableD3D10Driver;
}

__declspec(dllexport) DWORD WINAPI InitializeExchangeServer()
{
	switch(gInfo.RouterType)
	{
	case 0:
	case 1: //--- RouterType = 1 not supported now ---
		{
			DEBUG_MESSAGE(_T("Creating D3D10 wrapper interface.\n"));
			break;
		}
	default:
		DEBUG_MESSAGE(_T("Creating original D3D10 interface.\n"));
		break;
	}
	return gInfo.RouterType; 
}

// {D9556D45-72EB-4327-A0CE-79E7DD007412}
DEFINE_GUID(LIBID_DX10LIB, 0xd9556d45, 0x72eb, 0x4327, 0xa0, 0xce, 0x79, 0xe7, 0xdd, 0x0, 0x74, 0x12);

class CDX10Module : public CAtlDllModuleT< CDX10Module >
{
public :
	DECLARE_LIBID(LIBID_DX10LIB)
};

CDX10Module _AtlModule;


void InitDirectories( )
{
#ifdef ZLOG_ALLOW_TRACING
	InitializeLogging();

	_tcscpy(gData.DumpDirectory, gInfo.LogFileDirectory);
	PathAppend(gData.DumpDirectory, _T("DX10"));
	CreateDirectory(gData.DumpDirectory, 0);
	ClearDirectory(gData.DumpDirectory);

	wchar_t	CommandFlow[MAX_PATH];
#ifndef DISABLE_D3DDEVICE_WRAPPER
	swprintf_s(CommandFlow, MAX_PATH, L"%s\\CommandFlow.xml", gData.DumpDirectory);
#endif
	WriteStreamer::Get().OpenFile( CommandFlow, "iZ3DCommandFlow");
#endif
}

void HookCreateD3DDevice();

void DetectShutterMode()
{
	// Unsupported modes
	if (gData.ShutterMode == SHUTTER_MODE_MULTITHREADED	||
		gData.ShutterMode == SHUTTER_MODE_MULTIDEVICE)
		gData.ShutterMode = SHUTTER_MODE_AUTO;
	else if (gData.ShutterMode == SHUTTER_MODE_UMHOOK_D3D9EX)
		gData.ShutterMode = SHUTTER_MODE_UMHOOK_D3D10;

	if( (gData.ShutterMode == SHUTTER_MODE_AUTO || gData.ShutterMode == SHUTTER_MODE_IZ3DKMSERVICE) && (gInfo.OutputCaps & odShutterModeKMAllowed) )
	{	
		// for skipping problems with Aero
		if( TRUE == g_KMShutter.InitShutter() )
			gData.ShutterMode = SHUTTER_MODE_IZ3DKMSERVICE;
		else
		{
			if(	gData.ShutterMode != SHUTTER_MODE_AUTO )
				gData.ShutterMode = SHUTTER_MODE_DISABLED;
		}
	}

	if (gData.ShutterMode == SHUTTER_MODE_AUTO)
	{
		// TODO: Add detection when ATIQB not supported
		gData.ShutterMode = SHUTTER_MODE_ATIQB;
	}
}

BOOL WINAPI DllMain( HMODULE hModule,
					DWORD  ul_reason_for_call,
					LPVOID lpReserved
					)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		_CrtSetReportHookW2( _CRT_RPTHOOK_INSTALL, zlog::VldReportHook );
		//VLDSetReportHook( VLD_RPTHOOK_INSTALL, zlog::VldReportHook );
		//_CrtSetBreakAlloc(304370);
		gData.hModule = hModule;
		InitDirectories();
		DetectShutterMode();
		InitializeMadCHook();
		// Initialize DevIL.
		ilInit();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		FinalizeMadCHook();
		_RPT0(0, "S3DWrapperD3D10 memory leaks:\n");
		break;
	}

	return _AtlModule.DllMain(ul_reason_for_call, lpReserved);
}