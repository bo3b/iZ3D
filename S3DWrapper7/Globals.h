/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

struct GlobalInfo
{
	UINT		Trace;
	UINT		RouterType;
	TCHAR		DriverDirectory[MAX_PATH];
	TCHAR		ApplicationFileName[MAX_PATH];
	TCHAR		ApplicationName[MAX_PATH];
	TCHAR		LogFileDirectory[MAX_PATH];
	TCHAR		LogFileName[MAX_PATH];
	GlobalInfo()
	{
		ZeroMemory(this, sizeof(GlobalInfo));
	}
};

extern GlobalInfo gInfo;
