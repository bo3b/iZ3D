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

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "SharedInclude.h" 

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <ShellAPI.h>

#include <search.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <d3dx9math.h>
#include <dxerr.h>

#include <atlbase.h>
#include <atlcom.h>
#include <vector>
#include <map>
#include <algorithm>

#include "Globals.h"
#include "States.h"
#include "Trace.h"
#include "Utils.h"
