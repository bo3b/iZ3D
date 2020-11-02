#include "stdafx.h"
#include "HookPolice.h"
#include "../S3DAPI/HookAPI.h"
#include <process.h>
bool isHookPoliceWorking = true;

TCHAR txtBuf[MAX_PATH];
void __cdecl RefreshHooks( void* args/*** pNextHooksToRenew, size_t numNextHooks*/ ){
	// copy the hooks
	auto nextHook = (PVOID*)args;
	while ( isHookPoliceWorking ) {
#ifdef _DEBUG
		_itow_s( _getpid(), txtBuf, MAX_PATH, 10 );
		_tcscat_s( txtBuf, MAX_PATH, L" === iz3d thread is working" );
		OutputDebugString( txtBuf );
		auto res = 
#endif 
			RenewHook( nextHook );
#ifdef _DEBUG
			if( !res ){
				OutputDebugString( L"RenewHook Failed: "  );
				_itow_s( res, txtBuf, MAX_PATH, 16 );
				OutputDebugString( txtBuf );
			}
#endif
		//}
		Sleep( 1 );
	}
}

// starts hook police
HANDLE __cdecl StartHookPolice( PVOID* pNextHooksToRenew )
{
	HANDLE threadID = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))RefreshHooks, 
		(LPVOID)pNextHooksToRenew, 0,	NULL);
	if( !threadID )
		OutputDebugString( L"Hook Police thread error" );
	return threadID;
}