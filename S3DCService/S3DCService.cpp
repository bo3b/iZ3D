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
#include "CNTService_msg.h"
#include <MadCHook.h>
#include "Injector.h"
#include "S3DCService.h"
#include <Tlhelp32.h>
#include <Psapi.h>
#include <ShlwApi.h>
#include "ProductNames.h"

#pragma warning(push)
#pragma warning(disable : 4100) //--- disable unsed parameter warning ---
BOOL CALLBACK EnumServices(DWORD /*dwData*/, ENUM_SERVICE_STATUS&  Service)
{
	TRACE(_T("Service name is %s\n"), Service.lpServiceName);
	TRACE(_T("Friendly name is %s\n"), Service.lpDisplayName);

	return TRUE; //continue enumeration
}
#pragma warning(pop)

int __stdcall WinMain(HINSTANCE /* not used */, HINSTANCE /* not used */, LPSTR /* not used */, int /* not used */)
{
	S3DCService Service;
	Service.ParseCommandLine();
	if (Service.ProcessShellCommand())
		return 0;
	else
	{
		if (Service.GetShellCommand() != Command_InstallService || GetLastError() != ERROR_SERVICE_MARKED_FOR_DELETE)
			return -1;
		else
			return -2;
	}
}

S3DCService::S3DCService() : CNTService(SERVICE_NAME, DISPLAY_NAME, SERVICE_ACCEPT_STOP, _T("Inject and UnInject 3D driver")), driverControl(&m_EventLogSource)
{	
}

void S3DCService::ServiceMain(DWORD /*dwArgc*/, LPTSTR* /*lpszArgv*/)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(9860);
	//register our control handler
	RegisterCtrlHandlerEx();
	ReportStatusToSCM(SERVICE_RUNNING, NO_ERROR, 0, 1, 0);

	// The service should not call any system functions during service initialization. 
	// One should call system functions only after it reports a status of SERVICE_RUNNING.

	//SetMadCHookOption(DONT_TOUCH_RUNNING_PROCESSES, NULL);
#ifndef WIN64
	if(!IsWin64())
#endif
	{
		if (!StartInjectionDriver(L"iZ3DInjectionDriver"))
		{
			DWORD err = GetLastError();
		}
	}

	InitializeMadCHook();
	if(TCHAR* error = driverControl.Initialize())
		m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_FAIL_INITIALIZE_IPC, error);

	if(ReadConfigValue("RunAtStartup"))
		driverControl.InjectD3D();
	if(ReadConfigValue("OGLRunAtStartup"))
		driverControl.InjectOGL();

	//Report to the event log that the service has started successfully
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_STARTED, m_sDisplayName);
}

void S3DCService::OnStop()
{
	CAutoLocker lock(&m_CritSect); // synchronize access to the variables

	//Change the current state to STOP_PENDING
	ReportStatusToSCM(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 0);

	driverControl.UnInjectD3D();
	driverControl.UnInjectOGL();
	FinalizeMadCHook();

#ifndef WIN64
	if(!IsWin64())
#endif
	{
		if (!StopInjectionDriver(L"iZ3DInjectionDriver"))
		{
			DWORD err = GetLastError();
		}
	}

	ReportStatusToSCM(SERVICE_STOPPED, NO_ERROR, 0, 1, 0);
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_STOPPED, m_sDisplayName);

	_RPT0(0, "S3DCService memory leaks:\n");
}

void S3DCService::ShowHelp()
{
	MessageBox(NULL, _T("CService keys:\n -start\n -stop\n -install\n -uninstall")
		_T("\n -stopAndUninstall\n -inject\n -uninject\n -injectOGL")
		_T("\n -uninjectOGL\n -selfUpdate\n -LockMouse\n -UnLockMouse\n -help"), _T("Command line keys"), MB_OK);
}

BOOL S3DCService::ProcessShellCommand()
{	
	BOOL bResult = TRUE;
	TCHAR s[128];
	TCHAR* message = NULL;
	if(m_nShellCommand >= Command_RunAsService && m_nShellCommand <= Command_ShowServiceHelp)
	{
		bResult = CNTService::ProcessShellCommand();
	}
	else if(m_nShellCommand > Command_Start_S3DServiceCommand && m_nShellCommand < Command_End_S3DServiceCommand)
	{
		IPCBasicCommand command;
		//--- check running service version ---
		DWORD answerLen;
		command.id = IPCCOMMAND_GET_PROTOCOL_VERSION;
		message = SendIPCCommand(SERVICE_IPC_CHANNEL, &command, sizeof(command), &command, &answerLen);
		if(message == NULL)
		{
			if(command.id == IPCANSWER_OK)
			{
				if(command.param1 == PROTOCOL_VERSION)
				{
					switch (m_nShellCommand)
					{
						case Command_Inject:
							command.id = IPCCOMMAND_INJECT_D3D;
							break;
						case Command_UnInject:
							command.id = IPCCOMMAND_UNINJECT_D3D;
							break;
						case Command_OGLInject:
							command.id = IPCCOMMAND_INJECT_OGL;
							break;
						case Command_OGLUnInject:
							command.id = IPCCOMMAND_UNINJECT_OGL;
							break;
					}

					//--- force to inject from current directory ---
					GetServicePath(command.path1, true);
					PathRemoveFileSpec(command.path1);

					message = SendIPCCommand(SERVICE_IPC_CHANNEL, &command, sizeof(command), &command, &answerLen);
					if(message == NULL)
					{
						message = command.path1;
						bResult = (command.id == IPCANSWER_OK);
					}
					else bResult = FALSE;
				} 
				else
				{
					_stprintf_s(s, _countof(s), _T("Current service protocol version is %i, but required %i.\n Please, reinstall driver."), command.param1, PROTOCOL_VERSION);
					message = s;
					bResult = FALSE;
				}
			}
			else bResult = FALSE;
		}
		else bResult = FALSE;
	}

	if(!bResult)
	{
		DWORD err = GetLastError();
		if(message) 
			MessageBox(0, message, 0, MB_OK);						//--- show error as Dialog Box ---
		else
		{
			if(AttachConsole(ATTACH_PARENT_PROCESS))
			{
				_cputws(TEXT("\n\nFailed to execute command. Press Enter.\n"));	//--- show info in console window ---
				FreeConsole();
			}
		}
		SetLastError(err); // restore value
	}

	return bResult;
}

int S3DCService::BuildAppList( TCHAR message[MAX_STR_SIZE] )
{
	int cnt = 0;
	TCHAR* pos = message;
	pos[0] = '\0';
	HANDLE CTH = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	for(BOOL res = Process32First(CTH, &pe); res; res = Process32Next(CTH, &pe))
	{
		HANDLE CTHM = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
		if(CTHM != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32  meFirst = { sizeof(MODULEENTRY32) };
			BOOL res = Module32First(CTHM, &meFirst); //--- get process module name ---
			MODULEENTRY32  me = { sizeof(MODULEENTRY32) };
			for(res = Module32Next(CTHM, &me); res; res = Module32Next(CTHM, &me))
			{
				if( _tcsicmp(me.szModule, S3DWRAPPERD3D8_DLL_NAME) == 0		|| _tcsicmp(me.szModule, S3DWRAPPERD3D9_DLL_NAME) == 0 ||
					_tcsicmp(me.szModule, S3DWRAPPERD3D10_DLL_NAME) == 0	|| _tcsicmp(me.szModule, S3DWRAPPEROGL_DLL_NAME) == 0 /*||
					_tcsicmp(me.szModule, S3DINJECTOR_DLL_NAME) == 0		|| _tcsicmp(me.szModule, S3DOGLINJECTOR_DLL_NAME) == 0*/ )
				{
					DWORD size = MAX_STR_SIZE - (DWORD)(pos - message)/sizeof(TCHAR);
					if (AddToList(pe.th32ProcessID, meFirst.szModule, cnt, pos, size))
						pos += size;
					break;
				}
			}
			CloseHandle(CTHM);
		}
	}
	CloseHandle(CTH);
	return cnt;
}

struct  FINDWINDOWHANDLESTRUCT
{
	DWORD pid;
	TCHAR* title;
	TCHAR* exeFile;
	DWORD size;
	bool res;
};

BOOL CALLBACK EnumWindowCallBack(HWND hwnd, LPARAM lParam) 
{ 
	FINDWINDOWHANDLESTRUCT* pfwhs = (FINDWINDOWHANDLESTRUCT * )lParam;
	DWORD ProcessId; 
	GetWindowThreadProcessId ( hwnd, &ProcessId ); 
	if (pfwhs->pid == ProcessId)
	{
		GetWindowText(hwnd, pfwhs->title, pfwhs->size);
		if (pfwhs->title[0] != 0 && _tcscmp(pfwhs->title, _T("S3D Driver Window")) != 0)
		{
			pfwhs->res = true;
			_tcscat_s(pfwhs->title, pfwhs->size, _T(" ["));
			_tcscat_s(pfwhs->title, pfwhs->size, pfwhs->exeFile);
			_tcscat_s(pfwhs->title, pfwhs->size, _T("]\n"));
			pfwhs->size = (DWORD)_tcsnlen(pfwhs->title, pfwhs->size);
			return false;
		}
	}
	return true;
}

bool S3DCService::AddToList( DWORD pid, TCHAR* exeFile, int &cnt, TCHAR* message, DWORD &size )
{	
	if (size > 3)
	{
		FINDWINDOWHANDLESTRUCT fwhs;
		fwhs.title = message;
		fwhs.exeFile = exeFile;
		fwhs.pid = pid;
		fwhs.size = size;
		fwhs.res = false;
		EnumWindows ( EnumWindowCallBack, (LPARAM)&fwhs ) ;
		if (fwhs.res)
		{
			cnt++;
			size = fwhs.size;
			return true;
		}
	}
	return false;
}

bool S3DCService::ShowUninstallWindow()
{
	int res = IDCONTINUE;
	TCHAR message[MAX_STR_SIZE] = _T("");
	int cnt = BuildAppList(message);
	while (cnt > 0)
	{
		res = MessageBox(NULL, message, _T("Please close the following programs"), MB_ICONWARNING | MB_RETRYCANCEL | MB_DEFBUTTON1);
		if (res == IDCANCEL)
			break;
		cnt =  BuildAppList(message);
	}
	if (cnt == 0)
		return true;
	else
		return false;
}

BOOL S3DCService::Install()
{
	BOOL res = CNTService::Install();
	if (res)
	{
#ifndef WIN64
		if(!IsWin64())
#endif
		{
			wchar_t fileName[MAX_PATH];
			HMODULE hMod = GetModuleHandle(NULL);
			GetModuleFileNameW(hMod, fileName, MAX_PATH);
			wchar_t* s = wcsrchr(fileName, _T('\\'));
			if(s)
			{
				s++;
				*s = 0;
			}
			wcscat_s(fileName, MAX_PATH, L"S3DInjectionDriver.sys");
// Let's not make it active after reboot, so switching to LoadInjectionDriver
//			if (!InstallInjectionDriver(L"iZ3DInjectionDriver", fileName, L"Driver inject our D3D and OGL wrappers"))
			if (!LoadInjectionDriver(L"iZ3DInjectionDriver", fileName, L"Driver inject our D3D and OGL wrappers"))
			{
				DWORD err = GetLastError();
			}
		}
	}
	return res;
}

BOOL S3DCService::Uninstall()
{
	BOOL res = CNTService::Uninstall();
#ifndef WIN64
	if(!IsWin64())
#endif
	{
// Since it's not installed on a permanent basis, no need to uninstall now.  Will be removed at reboot.
//		if (!UninstallInjectionDriver(L"iZ3DInjectionDriver"))
		//{
		//	DWORD err = GetLastError();
		//}
	}
	return res;
}
