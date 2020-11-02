/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "S3DAPI.h"
#if !defined(FINAL_RELEASE) && !defined(DEBUGANALYSING) // && !defined(OUTPUT_LIBRARY)
#	define ZLOG_ALLOW_TRACING
#	ifdef _DEBUG
//#		define ZLOG_PRINT_FILE_AND_LINE
#	endif
#endif

#include "../ZLOg/ZLOg.h"
#include "EnumToString.h"

#ifdef ZLOG_ALLOW_TRACING
void S3DAPI_API InitializeLogging();
#endif
