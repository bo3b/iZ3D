/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "S3DWrapperOGL.h"
#ifndef FINAL_RELEASE
#	define ZLOG_ALLOW_TRACING
#ifdef _DEBUG
#	define ZLOG_PRINT_FILE_AND_LINE
#endif
#endif
#include "../ZLOg/ZLOg.h"

inline bool IsFocusInCurrentProcess()
{
	HWND focusWnd = GetForegroundWindow();
	if(focusWnd)
	{
		DWORD processID;
		GetWindowThreadProcessId(focusWnd, &processID);
		if(processID == GetCurrentProcessId())
			return true;
	}
	return false;
}

inline bool IsKeyPressedGlobal(int vKey)
{ 
	SHORT State = GetAsyncKeyState(vKey);
	if(State & 0x7FFF)
		return true;
	return false; 
}

inline bool IsKeyDownGlobal(int vKey)
{ 
	SHORT State = GetAsyncKeyState(vKey);
	if(State & 0xFFFF)
		return true;
	return false; 
}

inline bool IsKeyPressed(int vKey)
{
	return IsKeyPressedGlobal(vKey) && IsFocusInCurrentProcess();
}

inline bool IsKeyDown(int vKey)
{
	return IsKeyDownGlobal(vKey) && IsFocusInCurrentProcess();
}

#ifdef FINAL_RELEASE
#define GINFO_DEBUG                 FALSE
#define GINFO_TRACE                 FALSE

#define DEBUG_MESSAGE				if (FALSE) __noop
#define DEBUG_TRACE1				if (FALSE) __noop
#define DEBUG_TRACE2				if (FALSE) __noop
#define DEBUG_TRACE3				if (FALSE) __noop

#else

#define GINFO_DEBUG                 TRUE
#define GINFO_TRACE                 gInfo.Trace

#define ZLOG_MODULE "OGL"
#define DEBUG_MESSAGE(...)			ZLOG_MESSAGE_C(zlog::SV_MESSAGE, __VA_ARGS__)
#define DEBUG_TRACE1(...)			ZLOG_MESSAGE_C(zlog::SV_NOTICE, __VA_ARGS__)
#define DEBUG_TRACE2(...)			ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)
#define DEBUG_TRACE3(...)			ZLOG_MESSAGE_C(zlog::SV_FLOOD, __VA_ARGS__)

#endif

const char *GetPixelFormatDescriptorFlagsString(DWORD Flags);
const char *GetPixelFormatDescriptorString(const PIXELFORMATDESCRIPTOR *ppfd);
const char *GetDrawBufferFlagsString(DWORD Flags);
const char *GetLastErrorString();
const char *GetClearMaskString(GLbitfield mask);
const char *GetEnableString(GLenum cap);
