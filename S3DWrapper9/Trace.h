/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "DxErr.h"
#include "EnumToString.h"
#include "../S3DAPI/Logging.h"

#ifdef FINAL_RELEASE

#define DEBUGVER					FALSE
#define DO_DUMP						FALSE
#define DO_RTDUMP					FALSE
#define DO_FULLDUMP			        FALSE
#define GINFO_DUMP_ON				FALSE
#define GINFO_LOCKABLEDEPTHSTENCIL  FALSE
#define GINFO_CLEARBEFOREEVERYDRAW  FALSE
#define GINFO_REPLACEDSSURFACES     FALSE

#define INC_DEBUG_COUNTER(x, c)		__noop

#define NSPTR(ptr) __noop
#define NSFAILIF(expression) __noop

#define DUMPDIR					""
#define DUMPTEXTURESANDRT(a,b,c)	{if (0) DumpTexturesAndRT(a,b,c);}

#else

#define DEBUGVER					TRUE
#define DO_DUMP						(GetBaseDevice()->m_DumpType > dtNone)
#define DO_RTDUMP					(GetBaseDevice()->m_DumpType >= dtOnlyRT)
#define DO_FULLDUMP			        (GetBaseDevice()->m_DumpType >= dtFull)
#define GINFO_DUMP_ON				gInfo.DumpOn
#define GINFO_LOCKABLEDEPTHSTENCIL  gInfo.LockableDepthStencil
#define GINFO_CLEARBEFOREEVERYDRAW  DO_RTDUMP && gInfo.ClearBeforeEveryDrawWhenDoDump
#define GINFO_REPLACEDSSURFACES     gInfo.ReplaceDSSurfacesByTextures

#define INC_DEBUG_COUNTER(x, c)		x+=c

#define NSPTR(ptr) NSFAILIF(ptr == 0)

#define NSFAILIF(expression)																			\
{if (expression)																						\
{																										\
	DEBUG_MESSAGE(_T("%s(), %s, line %d failed:\n"), _T( __FUNCTION__ ) , _T(__FILE__), __LINE__);		\
	DEBUG_MESSAGE(_T("%s\n"), _T(#expression));															\
	return E_FAIL;																						\
}}

#define DUMPDIR						gData.DumpDirectory
#define DUMPTEXTURESANDRT(a,b,c)	DumpTexturesAndRT(a,b,c)

#endif // FINAL_RELEASE


#ifdef ZLOG_ALLOW_TRACING

#define GINFO_DEBUG                 TRUE

#define ZLOG_MODULE _T("D3D9")
#define DEBUG_MESSAGE(...)	ZLOG_MESSAGE(zlog::SV_MESSAGE, __VA_ARGS__)
#define DEBUG_TRACE1(...)	ZLOG_MESSAGE(zlog::SV_NOTICE, __VA_ARGS__)
#define DEBUG_TRACE2(...)	ZLOG_MESSAGE(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_TRACE3(...)	ZLOG_MESSAGE(zlog::SV_FLOOD, __VA_ARGS__)
#define FLUSHLOGFILE()

#define NSCALL(func)																					\
{																										\
	if (FAILED(hResult = func))																			\
	{																									\
		ZLOG_MESSAGE(zlog::SV_MESSAGE, _T("%s() failed\n %s(%d): %s - %s\n"), _T(__FUNCTION__), _T(__FILE__), __LINE__, _T(#func), DXGetErrorString(hResult));	\
	}																									\
}

#define NSCALL_TRACE_LEV(lev, func, NSCALL_MESSAGE)														\
{																										\
	if (FAILED(hResult = func) && hResult != D3DERR_NOTFOUND &&											\
			hResult != D3DERR_DEVICELOST && hResult != D3DERR_NOTAVAILABLE)								\
	{																									\
		NSCALL_MESSAGE;																					\
		zlog::LogMessageEx(lev, ZLOG_MODULE, false, false, _T("%s(%d) : failed\n %s() failed: %s\n"), _T(__FILE__), __LINE__, _T( __FUNCTION__ ), DXGetErrorString(hResult));	\
	}																									\
	else																								\
	{																									\
		if (zlog::GetSeverity() >= lev) { NSCALL_MESSAGE; }												\
		ZLOG_MESSAGE(lev, _T(" %s\n"), DXGetErrorString(hResult));										\
	}																									\
}

#define NSCALL_TRACE1(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_NOTICE, func, NSCALL_MESSAGE)
#define NSCALL_TRACE2(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_FLOOD, func, NSCALL_MESSAGE)
#define NSCALL_TRACE3(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_FLOOD, func, NSCALL_MESSAGE)
#define NSCALL_ERRMSG(func, NSCALL_MESSAGE)	NSCALL_TRACE_LEV(zlog::SV_COUNT, func, NSCALL_MESSAGE)

#define NSCALL_TRACE_RES(func, var, NSCALL_MESSAGE)														\
{																										\
	if (FAILED(hResult = func))																			\
	{																									\
		NSCALL_MESSAGE;																					\
		zlog::LogMessageEx(zlog::SV_NOTICE, ZLOG_MODULE, false, false, _T("%s(%d) : failed\n %s() failed: %s\n"), _T(__FILE__), __LINE__, _T( __FUNCTION__ ), DXGetErrorString(hResult));	\
	}																									\
	else																								\
	{																									\
		ZLOG_MESSAGE(zlog::SV_NOTICE, _T("RES ") _T(#var) _T(": \n\t"));								\
		if (zlog::GetSeverity() >= zlog::SV_NOTICE) { NSCALL_MESSAGE; }									\
		ZLOG_MESSAGE(zlog::SV_NOTICE, _T(" %s\n"), DXGetErrorString(hResult));							\
	}																									\
}

#else // !ALLOW_TRACING

#define GINFO_DEBUG                 FALSE
#define GINFO_TRACE                 0
#define GINFO_TRACE_RES             0

#define DEBUG_MESSAGE				if (FALSE) __noop
#define DEBUG_TRACE1				if (FALSE) __noop
#define DEBUG_TRACE2				if (FALSE) __noop
#define DEBUG_TRACE3				if (FALSE) __noop
#define FLUSHLOGFILE()				if (FALSE) __noop

#define NSCALL(func)						hResult = func
#define NSCALL_TRACE1(func, NSCALL_MESSAGE)	hResult = func
#define NSCALL_TRACE2(func, NSCALL_MESSAGE)	hResult = func
#define NSCALL_TRACE3(func, NSCALL_MESSAGE)	hResult = func
#define NSCALL_ERRMSG(func, NSCALL_MESSAGE)	hResult = func
#define NSCALL_TRACE_RES(func, var, NSCALL_MESSAGE)	hResult = func

#endif // ALLOW_TRACING

template <class T>
inline T	Indirect(T *t) { return t? *t: 0; }

inline void TraceMatrix(CONST D3DMATRIX * mat, bool bTranspose = false) { DEBUG_TRACE3(_T("%s"), GetMatrixString(mat, bTranspose)); }

