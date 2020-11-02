/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3F861DD7_BD50_47F8_AFD4_FFF26509D127__INCLUDED_)
#define AFX_STDAFX_H__3F861DD7_BD50_47F8_AFD4_FFF26509D127__INCLUDED_

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

#include "string"
#include "map"

#define THIS_MODULE_NAME	_T("S3DUtils.dll")

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3F861DD7_BD50_47F8_AFD4_FFF26509D127__INCLUDED_)

extern HMODULE g_hModule;
