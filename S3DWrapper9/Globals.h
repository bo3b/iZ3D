/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "..\S3DAPI\GlobalData.h"
#include "GlobalData.h"
#include "CallGuard.h"

inline bool IsLeftViewUnmodified() {
	return gInfo.SeparationMode == 1;
}
inline bool IsRightViewUnmodified() {
	return gInfo.SeparationMode == 2;
}

#define	IS_D3D9EX_PRESENTER					(USE_MULTI_DEVICE_PRESENTER || USE_UM_PRESENTER_D3D9EX)

void InitDirectories();

//--- GDI GetDeviceGammaRamp/GetDeviceGammaRamp manage section ---
BOOL WINAPI Hooked_SetDeviceGammaRamp(HDC hDC, LPVOID lpRamp);
BOOL WINAPI Hooked_GetDeviceGammaRamp(HDC hDC, LPVOID lpRamp);

extern BOOL (WINAPI *Original_SetDeviceGammaRamp)(HDC hDC, LPVOID lpRamp);
extern BOOL (WINAPI *Original_GetDeviceGammaRamp)(HDC hDC, LPVOID lpRamp);

extern D3DGAMMARAMP g_DeviceGammaRamp;
extern bool g_bGammaRampInitialized;
void InitGammaRamp();
