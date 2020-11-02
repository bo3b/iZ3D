#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD   ul_reason_for_call,
					   LPVOID  lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			DWORD dbgFlags = 0;
		#ifdef ZLOG_DEBUG_MEMORY_LEAKS
			dbgFlags |= _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF;
		#endif
		#ifdef ZLOG_DEBUG_MEMORY_ERRORS
			dbgFlags |= _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF;
		#endif
			if (dbgFlags != 0) {
				_CrtSetDbgFlag(dbgFlags);
			}
		}
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}