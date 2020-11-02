/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>

#define DISPLAY_NAME_WIN64		TEXT("S3D Service (Win64)")
#define SERVICE_NAME_WIN64		TEXT("S3DSvc64")
#define DISPLAY_NAME_WIN32		TEXT("S3D Service (Win32)")
#define SERVICE_NAME_WIN32		TEXT("S3DSvc32")

#ifdef  WIN64
	#define DISPLAY_NAME		DISPLAY_NAME_WIN64	
	#define SERVICE_NAME		SERVICE_NAME_WIN64		
	#define CURRENT_OS			L"Win64"
	#define SUBDIR_NAMEA		"Win64"
	#define SUBDIR_NAMEW		L"Win64"
#else
	#define DISPLAY_NAME		DISPLAY_NAME_WIN32
	#define SERVICE_NAME		SERVICE_NAME_WIN32		
	#define CURRENT_OS			L"Win32"
	#define SUBDIR_NAMEA		"Win32"
	#define SUBDIR_NAMEW		L"Win32"
#endif

#define SERVICE_EXE_NAME		TEXT("S3DCService.exe")
#define D3D_INJECTOR_DLL_NAME	L"S3DInjector.dll"
#define OGL_INJECTOR_DLL_NAME	L"S3DOGLInjector.dll"
#define CONFIG_XML_NAME			L"Config.xml"

BOOL  ReadConfigValue(LPCSTR paramName);
BOOL  GetServicePath(CHAR fileName[MAX_PATH], BOOL removeLastSlash = TRUE, BOOL returnExeName = FALSE);
BOOL  GetServicePath(WCHAR fileName[MAX_PATH], BOOL removeLastSlash = TRUE, BOOL returnExeName = FALSE);

// How many services are currently running
DWORD GetProcessCount();

// Used in combination with previous function
BOOL WaitForProcessCount(DWORD expectedCount, DWORD maxWaitTime = 6000);

void GetShellPathName(TCHAR* path);
void GetProcessHandleList(const TCHAR* path, std::vector<HANDLE>& list);
