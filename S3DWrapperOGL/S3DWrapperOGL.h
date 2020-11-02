/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <glh/glh_extensions.h>
#include <GL/glext.h>

#ifdef S3DWRAPPEROGL_EXPORTS
#define S3DWRAPPEROGL_API __declspec(dllexport)
#else
#define S3DWRAPPEROGL_API __declspec(dllimport)
#endif

struct GlobalInfo
{                   
	UINT		Trace;
	UINT		EmulateQB; 
	UINT		RouterType;
	TCHAR		DriverDirectory[MAX_PATH];
	TCHAR		DriverFileName[MAX_PATH];
	TCHAR       ApplicationFileName[MAX_PATH];
	TCHAR		ApplicationName[MAX_PATH];
	TCHAR       LogFileName[MAX_PATH];
	TCHAR       LogFileDirectory[MAX_PATH];
	GlobalInfo()
	{
		ZeroMemory(this, sizeof(GlobalInfo));
	}
	void Initialize(HMODULE hModule);
};

extern HMODULE g_hDllInstance;
extern GlobalInfo gInfo;

#define PROFILES_VERSION	1
#define GINFO_ENABLED		(gInfo.RouterType != 2)
