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
#include <Tlhelp32.h>
#include <Psapi.h>
#include "Injector.h"
#include "System.h"
#include "MadCHook.h"
#include "IPCService.h"
#include "CNTService_msg.h"
#include "..\CommonUtils\System.h"

#define SYS_PROCESS_LIST _T("*iRacing*.exe*")

//------------------------------ IPCControlService class --------------------------------
IPCControlService::IPCControlService(CNTEventLogSource* log)
{
	m_Log = log;
	pathD3DInjector[0] = 0;
	pathOGLInjector[0] = 0;
}

IPCControlService::~IPCControlService()
{
}

void IPCControlService::PipeCallback(void *pMessageBuf, DWORD dwMessageLen, void* pAnswerBuf, DWORD* dwAnswerLen)
{
	IPCBasicCommand* command = (IPCBasicCommand*)pMessageBuf;
	IPCBasicCommand* answer  = (IPCBasicCommand*)pAnswerBuf;
	*dwAnswerLen = sizeof(IPCBasicCommand);

#ifdef WIN64
	if(command->id != IPCCOMMAND_UPDATE || command->id != IPCCOMMAND_GET_PROTOCOL_VERSION)
	{
		//--- forward message into Win32 service ---
		TCHAR* message = SendIPCCommand(INTERSERVICE_IPC_CHANNEL, pMessageBuf, dwMessageLen, pAnswerBuf, dwAnswerLen);
		if(message)
		{
			answer->id = IPCANSWER_ERROR;
			_stprintf_s(answer->path1, _countof(answer->path1), TEXT("%s: %s"), SERVICE_EXE_NAME TEXT(" (") CURRENT_OS TEXT(")."), message);
			return;
		}

		if(answer->id == IPCANSWER_ERROR)
			return;
	}
#endif

	if(dwMessageLen < sizeof(IPCCommandHeader))
	{
		answer->id = IPCANSWER_ERROR;
		wcscpy_s(answer->path1, MAX_PATH, L"Empty command.");
		return;
	}

	answer->id = IPCANSWER_OK;
	wcscpy_s(answer->path1, MAX_PATH, L"Ok.");
	
	switch(command->id)
	{
		case IPCCOMMAND_GET_PROTOCOL_VERSION:
			answer->param1 = PROTOCOL_VERSION;
			break;
		case IPCCOMMAND_GET_INJECTION_STATUS:
			answer->param1 = NOT_INJECTED;
			answer->param1 |= wcslen(pathD3DInjector) == 0 ? 0 : D3D_INJECTED;
			answer->param1 |= wcslen(pathOGLInjector) == 0 ? 0 : OGL_INJECTED;
			break;
		case IPCCOMMAND_INJECT_D3D:
			if(!InjectD3D(command->path1))
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Fail to enable D3D driver. (" CURRENT_OS L")");
			}
			break;
		case IPCCOMMAND_INJECT_OGL:
			if(!InjectOGL(command->path1))
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Fail to enable OGL driver. (" CURRENT_OS L")");
			}
			break;
		case IPCCOMMAND_UNINJECT_D3D:
			if(!UnInjectD3D())
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Fail to disable D3D driver. (" CURRENT_OS L")");
			}
			break;
		case IPCCOMMAND_UNINJECT_OGL:
			if(!UnInjectOGL())
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Fail to disable OpenGL driver. (" CURRENT_OS L")");
			}
			break;
		case IPCCOMMAND_UPDATE:
			if(!CopyFileW(command->path1, command->path2, FALSE))
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Can't copy file.");
			}
			break;
		default:
			{
				answer->id = IPCANSWER_ERROR;
				wcscpy_s(answer->path1, MAX_PATH, L"Unsupported command. (" CURRENT_OS L")");
				break;
			}
	}
}

typedef int (WINAPI *hookFunc)(void);
typedef void (WINAPI *unhookFunc)(void);

void BuildFullInjectorPath(const WCHAR* inPath, WCHAR* outPath, const WCHAR* injectorName)
{
	WCHAR path1[MAX_PATH];
	WCHAR path2[MAX_PATH];

	if(inPath == NULL || inPath[0] == '\0')
		GetServicePath(path1, false);
	else
		wcscpy_s(path1, MAX_PATH, inPath);

	//--- check for last '\' symbol ---
	if(path1[wcslen(path1)-1] != L'\\')
		wcscat_s(path1, MAX_PATH, L"\\");

	swprintf_s(path2, MAX_PATH, L"%s" SUBDIR_NAMEW L"\\", path1);
	wcscat_s(path1, MAX_PATH, injectorName);
	wcscat_s(path2, MAX_PATH, injectorName);

	if(FileExists(path1))
		wcscpy_s(outPath, MAX_PATH, path1);
	else
		wcscpy_s(outPath, MAX_PATH, path2);
}

bool IPCControlService::InjectD3D(const WCHAR* path)
{
	return InternalInject(path, D3D_INJECTOR_DLL_NAME, pathD3DInjector);
}

bool IPCControlService::InjectOGL(const WCHAR* path)
{
	return InternalInject(path, OGL_INJECTOR_DLL_NAME, pathOGLInjector);
}

bool IPCControlService::UnInjectD3D()
{
	return InternalUnInject(pathD3DInjector);
}

bool IPCControlService::UnInjectOGL()
{
	return InternalUnInject(pathOGLInjector);
}

bool IPCControlService::InternalInject(const WCHAR* path, const WCHAR* DllName, WCHAR* pathToSaveDllName, HANDLE Target)
{
	WCHAR newPath[MAX_PATH];
	BuildFullInjectorPath(path, newPath, DllName);
	if(wcsicmp(newPath, pathToSaveDllName) == 0)
		return true;

	bool result = InternalUnInject(pathToSaveDllName, Target);
	if(result)
	{
		BOOL b;
		wcscpy_s(pathToSaveDllName, MAX_PATH, newPath);
		m_Log->Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_CUSTOM_COMMAND_INJECT, pathToSaveDllName);
		//b = StartDllInjection(L"iZ3DInjectionDriver", pathToSaveDllName, (DWORD)-1, FALSE, NULL, NULL);
		//b = StartDllInjection(L"iZ3DInjectionDriver", pathToSaveDllName, (DWORD)-1, TRUE , SYS_PROCESS_LIST, NULL);
		// New variant from 3.0 Seems like it ought to be only Current_Session though
		b = InjectLibrary(L"iZ3DInjectionDriver", pathToSaveDllName, ALL_SESSIONS, FALSE, NULL, NULL);
		b = InjectLibrary(L"iZ3DInjectionDriver", pathToSaveDllName, ALL_SESSIONS, TRUE , SYS_PROCESS_LIST, NULL);
		if (!b)
		{
			DWORD err = GetLastError();
		}
		result = true;
		//result = InjectLibrary(Target, pathToSaveDllName) != 0;
		if(!result)
			pathToSaveDllName[0] = 0;
	}
	return result;
}

bool IPCControlService::InternalUnInject(WCHAR* savedDllPath, HANDLE Target)
{
	bool result = true;
	if(savedDllPath[0] != '\0')
	{
		BOOL b;
		m_Log->Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_CUSTOM_COMMAND_UNINJECT, savedDllPath);
		//b = StopDllInjection(L"iZ3DInjectionDriver", savedDllPath, (DWORD)-1, FALSE, NULL, NULL);
		//b = StopDllInjection(L"iZ3DInjectionDriver", savedDllPath, (DWORD)-1, TRUE , SYS_PROCESS_LIST, NULL);
		b = UninjectLibrary(L"iZ3DInjectionDriver", savedDllPath, ALL_SESSIONS, FALSE, NULL, NULL);
		b = UninjectLibrary(L"iZ3DInjectionDriver", savedDllPath, ALL_SESSIONS, TRUE , SYS_PROCESS_LIST, NULL);
		if (!b)
		{
			DWORD err = GetLastError();
		}
		// ToDo: Not sure about this call. Probably needs to be new form of UninjectLibrary with 
		//  System_Processes=true. All_Sessions.  The InjectLibrary above was already commented
		//  out, so for now, we'll skip this too.
		//result = UninjectLibrary(Target, savedDllPath) != 0;
		result=true;
		if(result)
			savedDllPath[0] = 0;
	}
	return result;
}
