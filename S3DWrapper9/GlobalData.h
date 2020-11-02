/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once
#include "..\S3DAPI\GlobalData.h"

class GlobalData
{
public:
	// app global data
	HMODULE		hModule;
	UINT		DumpIndex; // just global index

	// wrapper local data
	INT			ShutterMode;
	TCHAR		DumpDirectory[MAX_PATH];

	GlobalData()
	{
		ZeroMemory(this, sizeof(GlobalData));
		//--- we MUST use _gInfo, not gInfo, because this global variable not initialized yet ---
		ShutterMode = _gInfo.ShutterMode;
	}
};

extern GlobalData gData;
