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
// S3DOGLInjector.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <madCHook.h>
#include <shlwapi.h>
#include "ProductNames.h"

#define HOOKING_FLAG (NO_SAFE_UNHOOKING)

#ifdef _DEBUG
#define	_TRACE(s) OutputDebugString(_T(s))
#else
#define	_TRACE(s) 
#endif

__declspec(dllexport) void WINAPI Disable3DDriver()
{
	FinalizeMadCHook();
}

_TCHAR	DllOGLPath[MAX_PATH];

typedef BOOL (APIENTRY *PFNPOGL)();
HMODULE hModOGL = 0;
typedef int ( APIENTRY* PFNWGLDESCRIBEPIXELFORMAT )(  __in HDC hdc, __in int iPixelFormat, __in UINT nBytes, __out_bcount_opt(nBytes) LPPIXELFORMATDESCRIPTOR ppfd);
PFNWGLDESCRIBEPIXELFORMAT wglDescribePixelFormatNext = NULL;

int WINAPI wglDescribePixelFormatCallback(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
	_TRACE("wglDescribePixelFormatCallback\n");

	UnhookAPI((PVOID*) &wglDescribePixelFormatNext);

	if(!hModOGL)
		hModOGL = LoadLibraryEx(DllOGLPath, 0, 0);
	if(hModOGL)
	{
		PFNPOGL p = (PFNPOGL)GetProcAddress(hModOGL, "HookOGL");
		if(p)
			p();
	}			

	int res = wglDescribePixelFormatNext(hdc, iPixelFormat, nBytes, ppfd);
	return res;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{	        
			DisableThreadLibraryCalls(hModule);
			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
			//_CrtSetBreakAlloc(82291);
			InitializeMadCHook();

			GetModuleFileName((HMODULE)hModule, DllOGLPath, sizeof(DllOGLPath));
			_tcsrchr(DllOGLPath, _T('\\'))[1] = '\0';
			PathAppend(DllOGLPath, S3DWRAPPEROGL_DLL_NAME);
			HookAPI(("opengl32.dll"), "wglDescribePixelFormat", wglDescribePixelFormatCallback, (PVOID*) &wglDescribePixelFormatNext, HOOKING_FLAG);
			break;
		} 
	case DLL_PROCESS_DETACH:
		{
			FinalizeMadCHook();
			// do not unload wrapper ddl's.  this prevent crash when uninject occured while app worked
			_RPT0(0, "S3DOGLInjector memory leaks:\n");
			break;
		}
	}

	return true;
}
