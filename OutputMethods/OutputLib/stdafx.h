// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "SharedInclude.h"

#include <boost\pool\pool_alloc.hpp>

// DX10
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

#ifndef _NTDEF_
typedef __success(return >= 0) LONG NTSTATUS, *PNTSTATUS;
#endif
