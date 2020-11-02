/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <dxerr.h>
#ifndef FINAL_RELEASE
#	define ZLOG_ALLOW_TRACING
#ifdef _DEBUG
#	define ZLOG_PRINT_FILE_AND_LINE
#endif
#endif
#include "../ZLOg/ZLOg.h"

#ifdef FINAL_RELEASE

#define GINFO_DEBUG FALSE
#define GINFO_TRACE FALSE
#define DEBUG_MESSAGE if (FALSE) __noop
#define DEBUG_TRACE1 if (FALSE) __noop
#define DEBUG_TRACE2 if (FALSE) __noop
#define DEBUG_TRACE3 if (FALSE) __noop
#define DEBUG_FAILED(result) __noop;

#define NSTRY HRESULT hResult = S_OK; 
#define NSRET return hResult;

#define NSCALL(func) hResult = func;
#define NSFAILIF(expression) false
#define NSPTR(ptr) false
#define NSFINALRELEASE(intf) (intf)->Release()

#else

#define GINFO_DEBUG TRUE
#define ZLOG_MODULE "D3D8"
#define DEBUG_MESSAGE(...)	ZLOG_MESSAGE_C(zlog::SV_MESSAGE, __VA_ARGS__)
#define DEBUG_TRACE1(...)	ZLOG_MESSAGE_C(zlog::SV_NOTICE, __VA_ARGS__)
#define DEBUG_TRACE2(...)	ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_TRACE3(...)	ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_FAILED(result) {DEBUG_MESSAGE("%s failed, hr = %s\n", __FUNCTION__ , GetResultString(result));}

#define NSTRY HRESULT hResult = S_OK; 
#define NSRET NSCALL(hResult); return hResult;

#define NSCALL(func)																\
{if (FAILED(hResult = func))														\
{																					\
	ZLOG_MESSAGE_C(zlog::SV_MESSAGE, "%s() failed\n %s(%d): %s - %s\n", __FUNCTION__, __FILE__, __LINE__, #func, DXGetErrorStringA(hResult));	\
}} 
#define NSFAILIF(expression)														\
{if (expression)																	\
{																					\
	ZLOG_MESSAGE_C(zlog::SV_MESSAGE, "%s() failed\n %s(%d): %s\n", __FUNCTION__, __FILE__, __LINE__, #expression);	\
	return E_FAIL;																	\
}}
#define NSFINALRELEASE(intf)														\
{ DWORD rc = (intf)->Release();														\
	if (rc != 0)																	\
{																					\
	ZLOG_MESSAGE_C(zlog::SV_MESSAGE, "%s() failed\n %s(%d) : WARNING rc = %d\n", __FUNCTION__, __FILE__, __LINE__, rc);	\
}} 
#define NSPTR(ptr) NSFAILIF(ptr == 0)

#endif // FINAL_RELEASE

const char *GetFVFString(DWORD fvf);
const char *GetRenderStateString(UINT state);
const char *GetResultString(HRESULT hr);
const char *GetPoolString(DWORD Pool);
const char *GetUsageString(DWORD Usage);
const char *GetFormatString( D3DFORMAT format );
const char *GetBehaviorFlagsString( DWORD BehaviorFlags );
const char *GetVSDEString( DWORD VertexRegister );
const char *GetVSDTString( DWORD Type  );
const char *GetVEDTypeString( BYTE Type );
const char *GetVEDMethodString( BYTE Method  );
const char *GetVEDUsageString( BYTE Usage  );
const char *GetDisplayModeString(const D3DDISPLAYMODE *pMode);
const char *GetPrimitiveTypeString( D3DPRIMITIVETYPE PrimitiveType );
const char *GetRectString(CONST RECT * pRect);
const char *GetLockedRectString(CONST D3DLOCKED_RECT * pLockedRect);
const char *GetSurfaceDescString(CONST D3DSURFACE_DESC8 *pDesc);
