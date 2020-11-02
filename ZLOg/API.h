#pragma once

#if defined(ZLOG_EXPORTS) && (defined(ZLOG_DEBUG_MEMORY_LEAKS) || defined(ZLOG_DEBUG_MEMORY_ERRORS))
#	define _CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ZLOG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ZLOG_EXPORTS functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ZLOG_EXPORTS
#define ZLOG_API __declspec(dllexport)
#else
#define ZLOG_API __declspec(dllimport)
#endif