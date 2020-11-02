// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include "SharedInclude.h" 

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "strstream"

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

#ifndef _NTDEF_
typedef __success(return >= 0) LONG NTSTATUS, *PNTSTATUS;
#endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "..\S3DAPI\GlobalData.h"
#include "CallGuard.h"

#include "Trace.h"
