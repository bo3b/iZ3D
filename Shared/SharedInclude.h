#ifndef SHARED_INCLUDE_H
#define SHARED_INCLUDE_H

#ifdef _DEBUG
   #define DEBUG_MEMORY_LEAKS
#endif

#ifdef DEBUG_MEMORY_LEAKS
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
#endif

#include <crtdbg.h>

#ifdef DEBUG_MEMORY_LEAKS

#define DNew new(_NORMAL_BLOCK, __FILE__, __LINE__)

#else

#define DNew new

#endif

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) UNREFERENCED_PARAMETER(x)
#endif
#define UNUSED_ALWAYS(x) UNREFERENCED_PARAMETER(x)

#endif // SHARED_INCLUDE_H


