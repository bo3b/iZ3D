#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "CallGuard.h"
#include "DxErr.h"
#include "../S3DAPI/Logging.h"

#ifndef ZLOG_ALLOW_TRACING

#define GINFO_DEBUG		FALSE
#define GINFO_DUMP_ON	FALSE
#define DEBUG_MESSAGE if (FALSE) __noop
#define DEBUG_TRACE1 if (FALSE) __noop
#define DEBUG_TRACE2 if (FALSE) __noop
#define DEBUG_TRACE3 if (FALSE) __noop

#define NSCALL(func) hResult = func;
#define NSCALL_TRACE1(func, NSCALL_MESSAGE)	hResult = func;
#define NSCALL_TRACE2(func, NSCALL_MESSAGE)	hResult = func;
#define NSCALL_TRACE3(func, NSCALL_MESSAGE)	hResult = func;

#else

#define GINFO_DEBUG		TRUE
#define GINFO_DUMP_ON	gInfo.DumpOn

#define ZLOG_MODULE _T("D3D10")
#define DEBUG_MESSAGE(...)	ZLOG_MESSAGE(zlog::SV_MESSAGE, __VA_ARGS__)
#define DEBUG_TRACE1(...)	ZLOG_MESSAGE(zlog::SV_NOTICE, __VA_ARGS__)
#define DEBUG_TRACE2(...)	ZLOG_MESSAGE(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_TRACE3(...)	ZLOG_MESSAGE(zlog::SV_FLOOD, __VA_ARGS__)

#define NSCALL_TRACE_LEV(lev, func, NSCALL_MESSAGE)														\
{																										\
	if (FAILED(hResult = func))																			\
	{																									\
		NSCALL_MESSAGE;																					\
		zlog::LogMessageEx(lev, ZLOG_MODULE, false, false, _T("%s(%d) : failed\n %s() failed: %s(0x%X)\n"),\
						   _T(__FILE__), __LINE__, _T( __FUNCTION__ ), DXGetErrorString(hResult), hResult);	\
	}																									\
	else																								\
	{																									\
		if (zlog::GetSeverity() >= lev) { NSCALL_MESSAGE; }												\
		ZLOG_MESSAGE(lev, _T(" %s(0x%X)\n"), DXGetErrorString(hResult), hResult);						\
	}																									\
}

#define NSCALL_TRACE1(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_NOTICE, func, NSCALL_MESSAGE)
#define NSCALL_TRACE2(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_FLOOD, func, NSCALL_MESSAGE)
#define NSCALL_TRACE3(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_FLOOD, func, NSCALL_MESSAGE)
#define NSCALL(func)																									\
{																														\
	if (FAILED(hResult = func))																							\
	{																													\
		ZLOG_MESSAGE(zlog::SV_MESSAGE, _T("%s() failed\n %s(%d): %s - %s\n"), _T(__FUNCTION__), _T(__FILE__), __LINE__, _T(#func), DXGetErrorString(hResult));	\
	}																													\
} 

#endif // ZLOG_ALLOW_TRACING

TCHAR*			FormatStr( const TCHAR* szFormat, ... );
const TCHAR*	GetErrorMessage( DWORD nError, const TCHAR* szFunction );
