/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <DxErr.h>
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

void LogMessage(const char *pBuffer);
void LogFormatMessage(const char *fmt, ...);

#define GINFO_DEBUG TRUE
#define GINFO_TRACE gInfo.Trace
#define ZLOG_MODULE "D3D7"
#define DEBUG_MESSAGE(...)	ZLOG_MESSAGE_C(zlog::SV_MESSAGE, __VA_ARGS__)
#define DEBUG_TRACE1(...)	ZLOG_MESSAGE_C(zlog::SV_NOTICE, __VA_ARGS__)
#define DEBUG_TRACE2(...)	ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_TRACE3(...)	ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_FAILED(result) {DEBUG_MESSAGE("%s failed, hr = %s\n", __FUNCTION__ , GetResultString(result));}

#define NSTRY HRESULT hResult = S_OK; 
#define NSRET return hResult;

#define NSCALL(func)																\
{if (FAILED(hResult = func))														\
{																					\
	DEBUG_MESSAGE("%s(%d) : failed %s():\n", __FILE__, __LINE__, __FUNCTION__);		\
	DEBUG_MESSAGE("%s", #func);														\
	DEBUG_MESSAGE(" %s\n", GetResultString(hResult));								\
}} 
#define NSFAILIF(expression)														\
{if (expression)																	\
{																					\
	DEBUG_MESSAGE("%s(%d) : failed %s():\n", __FILE__, __LINE__, __FUNCTION__);		\
	DEBUG_MESSAGE("%s\n", #expression);												\
	return E_FAIL;																	\
}}
#define NSFINALRELEASE(intf)														\
{ DWORD rc = (intf)->Release();														\
	if (rc != 0)																	\
{																					\
	DEBUG_MESSAGE("%s(%d) : %s()\n", __FILE__, __LINE__, __FUNCTION__);				\
	DEBUG_MESSAGE("WARNING rc = %d\n", rc);											\
}} 
#define NSPTR(ptr) NSFAILIF(ptr == 0)

#endif // FINAL_RELEASE

const char *GetResultString(HRESULT hr);

