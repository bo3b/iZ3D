// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"			

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "strstream"

#include "SharedInclude.h" 

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

#ifndef _NTDEF_
typedef __success(return >= 0) LONG NTSTATUS, *PNTSTATUS;
#endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "..\S3DAPI\GlobalData.h"

#include "Commands\Command.h"
#include "Trace.h"
#include "Utils.h"

#define SHADERTEST

void LogMessage(const char *pBuffer);

void LogFormatMessage(const char *fmt, ...);

DWORD CalculateCRC32(const void *data, DWORD Size);

