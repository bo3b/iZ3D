#pragma once

#define D3D9_VISTA_MODULE_NAME	L"DriverD3D9VistaModule"
#define D3D10_VISTA_MODULE_NAME	L"DriverD3D10VistaModule"
#define D3D10_WIN7_MODULE_NAME	L"DriverD3D10Win7Module"

void WriteDriverModuleName(LPCWSTR KeyName, LPCWSTR ModuleName);
bool CheckModuleFunction(LPCSTR functionName, LPCWSTR ModuleName);
bool GetDriverModuleName(/* in */ LPCSTR functionName, /* out */ WCHAR ModuleName[MAX_PATH]);
void GetAndWriteToRegistryDriverModuleName();
