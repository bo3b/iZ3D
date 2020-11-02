#define ZLOG_ALLOW_TRACING
#include "Utils.h"
#include <intrin.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <sstream>

namespace {

	const int		BUFFER_SIZE = 2048;
	const int		NUM_BUFFERS = 128;
	char			g_Buffers[NUM_BUFFERS][BUFFER_SIZE];
	volatile long	g_BufferCounter = 0;

} // anonymous namespace

namespace zlog {
namespace detail {

const char* dbg_sprintf(const char *format, ...)
{
	long counter = _InterlockedIncrement(&g_BufferCounter);
	unsigned long bufferId = unsigned long(counter) % NUM_BUFFERS;

	va_list valist;
	va_start(valist, format);
	_vsnprintf(g_Buffers[bufferId], BUFFER_SIZE / sizeof(char), format, valist);
	va_end(valist);

	return g_Buffers[bufferId];
}
		
const wchar_t* dbg_sprintf(const wchar_t *format, ...)
{
	long counter = _InterlockedIncrement(&g_BufferCounter);
	unsigned long bufferId = unsigned long(counter) % NUM_BUFFERS;

	va_list valist;
	va_start(valist, format);
	_vsnwprintf( (wchar_t*)g_Buffers[bufferId], BUFFER_SIZE / sizeof(wchar_t), format, valist);
	va_end(valist);

	return (wchar_t*)g_Buffers[bufferId];
}

} // namespace detail
} // namespace zlog