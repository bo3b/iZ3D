/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

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

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <atlbase.h>
#include <atlcom.h>

#include "Globals.h"
#include <d3d9.h>
#include <ddraw.h>
#include <d3d.h>
#include "Trace.h"
#include "utils.h"
#include "declguid.h"
#include "wrapper.h"
#include "wrappertable.h"
