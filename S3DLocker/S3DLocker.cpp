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
// S3DLocker.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <shlwapi.h>
#include "S3DLocker.h"
#define	CLASS_NAME				TEXT("S3DLockerClass")
#ifndef WIN64
	#define	WINDOW_NAME			TEXT("S3DLocker32Window")
	#define	MOUSE_LOCK_DLL_NAME TEXT("S3DMouseLock32.dll")
#else
	#define	WINDOW_NAME			TEXT("S3DLocker64Window")
	#define	MOUSE_LOCK_DLL_NAME	TEXT("S3DMouseLock64.dll")
#endif

typedef bool (CALLBACK *S3DSetLockFP)(bool);
S3DSetLockFP S3DSetLock;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(82291);

	// Remove the working directory from the search path to work around the DLL preloading vulnerability
	HMODULE hKernel = GetModuleHandle(L"Kernel32.dll");
	if (hKernel)
	{
		typedef BOOL (WINAPI *SetDllDirectoryWType)(LPCWSTR lpPathName);
		SetDllDirectoryWType pfnSetDllDirectory = (SetDllDirectoryWType)GetProcAddress(hKernel, "SetDllDirectoryW");
		if (pfnSetDllDirectory)
			pfnSetDllDirectory(L"");

		typedef BOOL (WINAPI *SetSearchPathModeType)(DWORD Flags);
		SetSearchPathModeType pfnSetSearchPathMode = (SetSearchPathModeType)GetProcAddress(hKernel, "SetSearchPathMode");
		if (pfnSetSearchPathMode)
			pfnSetSearchPathMode(BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE);
	}

	if(!FindWindow(CLASS_NAME, WINDOW_NAME))
	{
		TCHAR mouse_lock_full_path [MAX_PATH];
		GetModuleFileName(hInstance, mouse_lock_full_path, sizeof(mouse_lock_full_path));
		_tcsrchr(mouse_lock_full_path, _T('\\'))[1] = '\0';
		PathAppend(mouse_lock_full_path, MOUSE_LOCK_DLL_NAME);
		HMODULE hMod = LoadLibrary(mouse_lock_full_path);
		if(hMod)
		{
			S3DSetLock = (S3DSetLockFP)GetProcAddress(hMod, "S3DSetLock");
			if(S3DSetLock)
			{
				if (S3DSetLock(true))
				{
					WNDCLASS wndClass = { 0,DefWindowProc,0,0,hInstance,NULL,LoadCursor(NULL,IDC_ARROW),(HBRUSH)GetStockObject(BLACK_BRUSH),NULL,CLASS_NAME };
					if(RegisterClass( &wndClass ))
					{
						HWND hWnd = CreateWindow(CLASS_NAME, WINDOW_NAME, WS_POPUP, 0,0,1,1,0, NULL,NULL, 0);
						if(hWnd)
						{
							MSG msg;
							while (GetMessage(&msg, NULL, 0, 0))
								DispatchMessage(&msg);
							S3DSetLock(false);
							DestroyWindow(hWnd);
						}
						UnregisterClass(CLASS_NAME, hInstance);
					}
				}
			}
		}
	}
	_RPT0(0, "S3DLocker memory leaks:\n");
}
