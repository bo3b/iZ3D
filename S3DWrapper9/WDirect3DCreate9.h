/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once 

#include "Direct3D9.h"

typedef IDirect3D9* (WINAPI *pfDirect3DCreate)(UINT sdkVersion);
typedef HRESULT (WINAPI *pfDirect3DCreateEx)(UINT sdkVersion, IDirect3D9Ex **ppD3D);

BOOL GetD3D9DllName(TCHAR name[MAX_PATH]);
BOOL GetD3D9DllFullPath(TCHAR dllFullPath[MAX_PATH]);
HMODULE GetD3D9HMODULE();
pfDirect3DCreate WINAPI GetD3D9Creator();
pfDirect3DCreateEx WINAPI GetD3D9CreatorEx();
IDirect3D9 *WINAPI WDirect3DCreate9(UINT SDKVersion);

BOOL IsVistaSP1();
void DetectShutterMode();
extern bool g_bD3D9DllAlreadyLoaded;
