/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

//#include <afxext.h> 
#include <windows.h> 
#include <tchar.h> 
#include <stdio.h> 

#include "SharedInclude.h" 

#define TRACE LogFormatMessage

#ifdef _DEBUG
	inline void LogFormatMessage(LPCTSTR fmt, ...)
	{
		TCHAR szBuffer[MAX_PATH];
		va_list args;
		va_start(args, fmt);
		_vsntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, fmt, args);
		va_end(args);
		OutputDebugString(szBuffer);
	}
#else
	inline void LogFormatMessage(LPCTSTR, ...) {} 
#endif
