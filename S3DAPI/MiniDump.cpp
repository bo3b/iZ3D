/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"
#include <Shlwapi.h>
#include <Shlobj.h>
#include "..\CommonUtils\CommonResourceFolders.h"
#include "dbghelp.h"

typedef BOOL (WINAPI *MINIDUMP_WRITE_DUMP)(HANDLE hProcess, DWORD ProcessId,
										   HANDLE hFile, MINIDUMP_TYPE DumpType, PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										   PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

void CreateMiniDump( EXCEPTION_POINTERS* pException ) 
{
	TCHAR DbgDllPath[MAX_PATH];
	_tcscpy(DbgDllPath, gInfo.DriverDirectory);
	PathAppend(DbgDllPath, _T("DBGHELP.DLL"));
	HMODULE hDbgHelp = LoadLibrary(DbgDllPath);
	if (!hDbgHelp)
		return;
	MINIDUMP_WRITE_DUMP MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, 
		"MiniDumpWriteDump");
	if (MiniDumpWriteDump_)
	{
		MINIDUMP_EXCEPTION_INFORMATION  M;
		HANDLE  hDump_File;

		M.ThreadId = GetCurrentThreadId();
		M.ExceptionPointers = pException;  //got by GetExceptionInformation()

		M.ClientPointers = 0;

		TCHAR Dump_Path[MAX_PATH];
		if(iz3d::resources::GetAllUsersDirectory(Dump_Path)) 
		{
			PathAppend(Dump_Path, _T("MiniDumps\\") );
			CreateDirectory(Dump_Path, NULL);
		}
		TCHAR Dump_File[MAX_PATH];
		SYSTEMTIME st;
		GetLocalTime(&st);
		_stprintf_s(Dump_File, L"%s\\%s-%04d%02d%02d_%02d%02d%02d.mdmp", Dump_Path, gInfo.ProfileName, 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		hDump_File = CreateFile(Dump_File, GENERIC_WRITE, 0, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hDump_File)
		{
			MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(),
				hDump_File, MiniDumpNormal,
				(pException) ? &M : NULL, NULL, NULL);

			CloseHandle(hDump_File);
		}
	}

	FreeLibrary(hDbgHelp);
}

LPTOP_LEVEL_EXCEPTION_FILTER originalExceptionFilter = NULL;
LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS *pException)
{
	CreateMiniDump(pException);
	if (originalExceptionFilter)
		return originalExceptionFilter(pException);
	else 
		return 0;
}

void HookExceptions()
{
	originalExceptionFilter = SetUnhandledExceptionFilter(unhandledExceptionFilter);
}

void UnhookExceptions()
{
	SetUnhandledExceptionFilter(originalExceptionFilter);
}
