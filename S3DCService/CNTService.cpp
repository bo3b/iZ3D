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
/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include <winsvc.h>
#include <lmcons.h>
#include <MMSystem.h>
#include "System.h"
#include "CNTService.h"
#include "Injector.h"
#include "CNTService_msg.h"

///////////////////////// CNTService implementation ///////////////////////////
CNTService* CNTService::sm_lpService = NULL;

CNTService::CNTService(LPCTSTR lpszServiceName, LPCTSTR lpszDisplayName, DWORD dwControlsAccepted, LPCTSTR lpszDescription)
{
	//Validate our parameters
	_ASSERT(lpszServiceName);
	_ASSERT(lpszDisplayName);

	InitializeCriticalSection(&m_CritSect);
	CAutoLocker lock(&m_CritSect); //synchronise access to the variables

	m_nShellCommand = Command_RunAsService;
	_tcscpy_s(m_sServiceName, MAX_PATH, lpszServiceName);
	_tcscpy_s(m_sDisplayName, MAX_PATH, lpszDisplayName);
	m_hStatus = 0;
	m_dwCurrentState = SERVICE_STOPPED;
	m_dwControlsAccepted = dwControlsAccepted;
	if (lpszDescription)
		_tcscpy_s(m_sDescription, MAX_PATH, lpszDescription);

	// Copy the address of the current object so we can access it from
	// the static member callback functions.
	// WARNING: This limits the application to only one CNTService object. 
	sm_lpService = this;	//hive away the this pointer;

	//Register ourselves as a source for the event log
	m_EventLogSource.Register(NULL, m_sDisplayName);
}

CNTService::~CNTService()
{
	{
		CAutoLocker lock(&m_CritSect); //synchronise access to the variables
		sm_lpService = NULL;
	}
	DeleteCriticalSection(&m_CritSect);
}

BOOL CNTService::ReportStatusToSCM(DWORD dwCurrentState, DWORD dwWin32ExitCode, 
								   DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	CAutoLocker lock(&m_CritSect); //synchronise access to the variables

	m_dwCurrentState = dwCurrentState;
	SERVICE_STATUS ServiceStatus;
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	//Disable control requests until the service is started
	if (dwCurrentState == SERVICE_START_PENDING)
		ServiceStatus.dwControlsAccepted = 0;    
	else
		ServiceStatus.dwControlsAccepted = m_dwControlsAccepted;

	//May need to thread protect this
	ServiceStatus.dwCurrentState = dwCurrentState;
	ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	ServiceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
	ServiceStatus.dwCheckPoint = dwCheckPoint;
	ServiceStatus.dwWaitHint = dwWaitHint;

	BOOL bSuccess = ::SetServiceStatus(m_hStatus, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to SetServiceStatus in ReportStatusToSCM, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTService::ReportStatusToSCM()
{
	CAutoLocker lock(&m_CritSect); //synchronise access to the variables

	SERVICE_STATUS ServiceStatus;
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	//Disable control requests until the service is started
	if (m_dwCurrentState == SERVICE_START_PENDING)
		ServiceStatus.dwControlsAccepted = 0;    
	else
		ServiceStatus.dwControlsAccepted = m_dwControlsAccepted;

	ServiceStatus.dwCurrentState = m_dwCurrentState;
	ServiceStatus.dwWin32ExitCode = NO_ERROR;
	ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	BOOL bSuccess = ::SetServiceStatus(m_hStatus, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to SetServiceStatus in ReportStatusToSCM, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

void CNTService::OnStop()
{
	//Derived classes are required to implement
	//their own code to stop a service, all we do is
	//report that we were succesfully stopped

	//Add an Event log entry to say the service was stopped
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_STOPPED, m_sDisplayName);

	_ASSERT(FALSE);
}

void CNTService::OnPause()
{
	//Add an Event log entry to say the service was stopped
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_PAUSED, m_sDisplayName);

	//Derived classes are required to implement
	//their own code to pause a service
	_ASSERT(FALSE);
}

void CNTService::OnContinue()
{
	//Add an Event log entry to say the service was stopped
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_CONTINUED, m_sDisplayName);

	//Derived classes are required to implement
	//their own code to continue a service
	_ASSERT(FALSE);
}

void CNTService::OnInterrogate()
{
	//Default implementation returns the current status
	//as stored in m_ServiceStatus
	ReportStatusToSCM();
}

void CNTService::OnShutdown()
{
	//Add an Event log entry to say the service was stopped
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_SHUTDOWN, m_sDisplayName);

	//Derived classes are required to implement
	//their own code to shutdown a service
	_ASSERT(FALSE);
}

void CNTService::OnSessionChange(DWORD /* dwEventType */, LPVOID /* lpEventData */)
{
	//Add an Event log entry to say the service was stopped
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_SESSIONCHANGE, m_sDisplayName);

	//Derived classes are required to implement
	//their own code to SessionChange handling
	_ASSERT(FALSE);
}

void CNTService::OnUserDefinedRequest(DWORD /*dwControl*/)
{
	TRACE(_T("CNTService::OnUserDefinedRequest was called\n"));

	//Default implementation is do nothing
}

void CNTService::ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
{
	//Just switch on the control code sent to 
	//us and call the relavent virtual function
	switch (dwControl)
	{
		case SERVICE_CONTROL_STOP: 
		{
			OnStop();
			break;
		}
		case SERVICE_CONTROL_PAUSE:
		{
			OnPause();
			break;
		}
		case SERVICE_CONTROL_CONTINUE:
		{
			OnContinue();
			break;
		}
		case SERVICE_CONTROL_INTERROGATE:
		{
			OnInterrogate();
			break;
		}
		case SERVICE_CONTROL_SHUTDOWN:
		{
			OnShutdown();
			break;
		}
		case SERVICE_CONTROL_SESSIONCHANGE:
		{
			OnSessionChange(dwEventType, lpEventData);
			break;
		}
		default:
		{
			OnUserDefinedRequest(dwControl);
			break;
		}
	}

  //Any request from the SCM will be acked by this service
  ReportStatusToSCM();
}

BOOL CNTService::RegisterCtrlHandlerEx()
{
	CAutoLocker lock(&m_CritSect); //synchronise access to the variables

	m_hStatus = ::RegisterServiceCtrlHandlerEx(m_sServiceName, _ServiceCtrlHandlerEx, this);
	if (m_hStatus == 0)
		TRACE(_T("Failed in call to RegisterServiceCtrlHandler in RegisterCtrlHandlerEx, GetLastError:%d\n"), ::GetLastError());

	return (m_hStatus != 0);
}

void CNTService::ServiceMain(DWORD /*dwArgc*/, LPTSTR* /*lpszArgv*/)
{
	//Default implementation does nothing but asserts, your version should
	//call RegisterCtrlHandlerEx plus implement its own service specific code
	_ASSERT(FALSE); 
}

DWORD CNTService::_ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	//Convert from the SDK world to the C++ world. 
	_ASSERT(lpContext != NULL);
	((CNTService*)lpContext)->ServiceCtrlHandlerEx(dwControl, dwEventType, lpEventData);
	return NO_ERROR;
}

void CNTService::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	TRACE(_T("CNTService::_ServiceMain\n"));

	//Convert from the SDK world to the C++ world. In this
	//implementation we just use a single static, In the 
	//future we could use a map just like MFC does for HWND
	//to CWnds conversions
	_ASSERT(sm_lpService != NULL);
	sm_lpService->ServiceMain(dwArgc, lpszArgv);
}

BOOL CNTService::Run()
{
	TRACE(_T("CNTService::Run\n"));

	//Uncomment the DebugBreak function below when you want to debug your service
	//DebugBreak();

	//Set up the SERVICE table array
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = m_sServiceName;
	ServiceTable[0].lpServiceProc = _ServiceMain;
	ServiceTable[1].lpServiceName = 0;
	ServiceTable[1].lpServiceProc = 0;

	//Notify the SCM of our service
	BOOL bSuccess = ::StartServiceCtrlDispatcher(ServiceTable);
	if (!bSuccess)
		TRACE(_T("Failed in call to StartServiceCtrlDispatcher in Run, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

ResponseStatus CNTService::StartService()
{
	TRACE(_T("CNTService::StartService\n"));

	//Open up the SCM requesting connect rights
	CNTServiceControlManager manager;
	if (!manager.Open(NULL, SC_MANAGER_CONNECT))
	{
		TRACE(_T("Failed in call to open SCM in StartService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_CONNECT_SCM, ::GetLastError());
		return Response_Failed;
	}

	//Open up the existing service requesting stop rights
	CNTScmService service;
	if (!manager.OpenService(m_sServiceName, SERVICE_START | SERVICE_QUERY_STATUS, service))
	{
		TRACE(_T("Failed in call to OpenService in StartService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_OPEN_SERVICE, ::GetLastError());
		return Response_Failed;
	}

	SERVICE_STATUS serviceStatus;
	if(!service.QueryStatus(&serviceStatus))
	{
		TRACE(_T("Failed in call to QueryStatus in StartService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_QUERYSTATUS, ::GetLastError());
		return Response_Failed;
	}

	if(serviceStatus.dwCurrentState != SERVICE_START_PENDING && serviceStatus.dwCurrentState != SERVICE_RUNNING)
	{
		if(!service.Start(0, NULL))
		{
			TRACE(_T("Failed in call to Start in StartService, GetLastError:%d\n"), ::GetLastError());
			m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_START, ::GetLastError());
			return Response_Failed;
		}
		return Response_Success;
	}
	
	return Response_AlreadyDone;
}

BOOL CNTService::StartServiceEx()
{
	DWORD initialCount = GetProcessCount();
	ResponseStatus rResult = StartService();
	
	if(rResult == Response_AlreadyDone)
		return TRUE;

	if(rResult == Response_Success)
	{
		CNTScmService service;
		CNTServiceControlManager manager;
		if(manager.Open(NULL, SC_MANAGER_CONNECT))
			if(manager.OpenService(m_sServiceName, SERVICE_QUERY_STATUS, service))
				service.WaitForState(SERVICE_RUNNING);
		//--- try to wait again (line up not already worked succsessfully) ---
		WaitForProcessCount(initialCount+1);
		return TRUE;
	}

	return FALSE;
}

ResponseStatus CNTService::StopService()
{
	TRACE(_T("CNTService::StopService\n"));

	//Open up the SCM requesting connect rights
	CNTServiceControlManager manager;
	if (!manager.Open(NULL, SC_MANAGER_CONNECT))
	{
		TRACE(_T("Failed in call to open SCM in StopService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_CONNECT_SCM, ::GetLastError());
		return Response_Failed;
	}

	//Open up the existing service requesting stop rights
	CNTScmService service;
	if (!manager.OpenService(m_sServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS, service))
	{
		TRACE(_T("Failed in call to OpenService in StopService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_OPEN_SERVICE, ::GetLastError());
		return Response_Failed;
	}

	SERVICE_STATUS serviceStatus;
	if(!service.QueryStatus(&serviceStatus))
	{
		TRACE(_T("Failed in call to QueryStatus in StopService, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_QUERYSTATUS, ::GetLastError());
		return Response_Failed;
	}

	if(serviceStatus.dwCurrentState != SERVICE_STOP_PENDING && serviceStatus.dwCurrentState != SERVICE_STOPPED)
	{
		if(!service.Stop())
		{
			TRACE(_T("Failed in call to Stop in StopService, GetLastError:%d\n"), ::GetLastError());
			m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_STOP, ::GetLastError());
			return Response_Failed;
		}
		return Response_Success;
	}

	return Response_AlreadyDone;
}

BOOL CNTService::StopServiceEx()
{
	DWORD initialCount = GetProcessCount();
	ResponseStatus rResult = StopService();
	
	if(rResult == Response_AlreadyDone)
		return TRUE;

	if(rResult == Response_Success)
	{
		CNTScmService service;
		CNTServiceControlManager manager;
		if(manager.Open(NULL, SC_MANAGER_CONNECT))
			if(manager.OpenService(m_sServiceName, SERVICE_QUERY_STATUS, service))
				service.WaitForState(SERVICE_STOPPED);
		
		//--- we have at least one process: command line executable module ---
		if(initialCount > 1)
			WaitForProcessCount(initialCount-1);
		return TRUE;
	}

	return FALSE;
}

BOOL CNTService::Install()
{
	TRACE(_T("CNTService::Install\n"));
	
	// Get this exes full pathname
	TCHAR szAppPath[_MAX_PATH];
	GetModuleFileName(NULL, szAppPath, _MAX_PATH);

	//Open up the SCM requesting Creation rights
	CNTServiceControlManager manager;
	if (!manager.Open(NULL, SC_MANAGER_CREATE_SERVICE))
	{
		DWORD err = ::GetLastError();
		LPTSTR errMsg = NULL;
		::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, err, 0, (LPTSTR)&errMsg, 0, NULL);
		TRACE(_T("Failed in call to open SCM in Install, GetLastError: %d\n\t%s"), err, errMsg);
		LocalFree(errMsg);
		return FALSE;
	}

	//Create the new service entry in the SCM database
	CNTScmService service;
	if (!service.Create(manager, m_sServiceName, m_sDisplayName, 0, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, 
		SERVICE_ERROR_IGNORE, szAppPath, NULL, NULL, NULL, NULL, NULL))
	{
		DWORD err = ::GetLastError();
		LPTSTR errMsg = NULL;
		::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, err, 0, (LPTSTR)&errMsg, 0, NULL);
		TRACE(_T("Failed in call to CreateService in Install, GetLastError: %d\n\t%s"), err, errMsg);
		LocalFree(errMsg);
		return FALSE;
	}

	//Setup this service as an event log source (using the friendly name)
	if (!m_EventLogSource.Install(m_sDisplayName, szAppPath, EVENTLOG_ERROR_TYPE | 
		EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE))
	{
		DWORD err = ::GetLastError();
		LPTSTR errMsg = NULL;
		::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, err, 0, (LPTSTR)&errMsg, 0, NULL);
		TRACE(_T("Failed in call to install service as an Event log source, GetLastError: %d\n\t%s\n"), err, errMsg);
		LocalFree(errMsg);
		return FALSE;
	}

	//Add the description text to the registry if need be
	size_t nDescriptionLen = _tcslen(m_sDescription);
	if (nDescriptionLen)
	{
		TCHAR sKey[MAX_PATH];
		_stprintf_s(sKey, _countof(sKey), _T("SYSTEM\\CurrentControlSet\\Services\\%s"), m_sServiceName);
		HKEY hService;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, sKey, 0, KEY_WRITE|KEY_READ, &hService) == ERROR_SUCCESS)
		{
			if (RegSetValueEx(hService, _T("Description"), NULL, REG_SZ, (LPBYTE)m_sDescription, (DWORD)nDescriptionLen*sizeof(TCHAR)+1) != ERROR_SUCCESS)
				TRACE(_T("Failed in call to set service description text, GetLastError: %d\n"), ::GetLastError());

			RegCloseKey(hService);
		}
	}

	//Add an Event log entry to say the service was successfully installed
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_INSTALLED, m_sDisplayName);

	TRACE(_T("Service: %s was succesfully installed\n"), m_sServiceName);

	return TRUE;
}

BOOL CNTService::InstallEx()
{
	BOOL bResult = Install();
#ifndef WIN64
	if(bResult && IsWin64())	
	{
		bResult = false;
		CNTScmService service;
		CNTServiceControlManager manager;
		if(manager.Open(NULL, SC_MANAGER_CONNECT))
			if(manager.OpenService(m_sServiceName, SERVICE_CHANGE_CONFIG, service))
			{
				TCHAR servicename[_countof(SERVICE_NAME_WIN64) + 1];
				_tcscpy_s(servicename, _countof(servicename), SERVICE_NAME_WIN64);
				servicename[_tcslen(servicename)+1] = _T('\0');
				bResult = service.ChangeConfig(SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
					NULL, NULL, NULL, servicename, NULL, NULL, NULL);
			}
		if (!bResult) {
			TRACE(_T("Failed add dependency to 64-bit service, GetLastError:%d\n"), ::GetLastError());
		}
	}
#endif
	return bResult;
}

BOOL CNTService::Uninstall()
{
	TRACE(_T("CNTService::Uninstall\n"));

	//Open up the SCM requesting connect rights
	CNTServiceControlManager manager;
	if (!manager.Open(NULL, SC_MANAGER_CONNECT))
	{
		TRACE(_T("Failed in call to open SCM in Uninstall, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_CONNECT_SCM, ::GetLastError());
		return FALSE;
	}

	//Open up the existing service requesting deletion rights
	CNTScmService service;
	if (!manager.OpenService(m_sServiceName, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS, service))
	{
		TRACE(_T("Failed in call to OpenService in Uninstall, GetLastError:%d\n"), ::GetLastError());
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_OPEN_SERVICE, ::GetLastError());
		return FALSE;
	}

	//Delete the service from the SCM database
	if (!service.Delete())
	{
		m_EventLogSource.Report(EVENTLOG_ERROR_TYPE, CNTS_MSG_SERVICE_FAIL_DELETE_SERVICE, GetLastError());
		TRACE(_T("Failed in call to DeleteService in Uninstall, GetLastError:%d\n"), ::GetLastError());
		return FALSE;
	}

	//Add an Event log entry to say the service was successfully installed
	m_EventLogSource.Report(EVENTLOG_INFORMATION_TYPE, CNTS_MSG_SERVICE_UNINSTALLED, m_sDisplayName);

	//Remove this service as an event log source
	if (!m_EventLogSource.Uninstall(m_sDisplayName))
	{
		TRACE(_T("Failed in call to delete service as an Event log source, GetLastError:%d\n"), ::GetLastError());
		return FALSE;
	}

	TRACE(_T("Service: %s was succesfully uninstalled\n"), m_sServiceName);

	return TRUE;
}

BOOL CNTService::SelfUpdate()
{
	BOOL bResult = StopServiceEx();
	if(bResult)
	{
		CNTScmService service;
		CNTServiceControlManager manager;
		LPQUERY_SERVICE_CONFIG config = NULL;

		bResult = FALSE;
		if(manager.Open(NULL, SC_MANAGER_CONNECT))
			if(manager.OpenService(m_sServiceName, SERVICE_QUERY_CONFIG, service))
				if(service.QueryConfig(config))
				{
					TCHAR currentPath[MAX_PATH];
					GetServicePath(currentPath, FALSE, TRUE);
					bResult = CopyFile(currentPath, config->lpBinaryPathName, FALSE);
				}

		StartService();
	}
	return bResult;
}

void CNTService::Debug()
{
#ifndef FINAL_RELEASE
	//Runing as EXE not as service, just execute the services
	//SeviceMain function
	ServiceMain(0, NULL);
	Sleep(10000);
#endif
}

void CNTService::ShowHelp()
{
	//Default behaviour is to do nothing. In your
	//service application, you should override
	//this function to either display something
	//helpful to the console or if the service
	//is running in the GUI subsystem, to 
	//display a messagebox or dialog to provide
	//info about your service.
}

bool CNTService::ShowUninstallWindow()
{
	return false;
}

//Based upon the function of the same name in CWinApp
CNTServiceCommand CNTService::ParseCommandLine()
{
	m_nShellCommand = Command_RunAsService;

#ifdef UNICODE
	__wargv = CommandLineToArgvW(GetCommandLineW(), &__argc);
#endif

	// we support only one command-line parameter 
	for (int i = 1; i < __argc && i< 2; i++)
	{
		LPCTSTR pszParam = __targv[i];

		if (pszParam[0] == _T('-') || pszParam[0] == _T('/'))
			++pszParam;

		if (_tcsicmp(pszParam, _T("start")) == 0)
			m_nShellCommand = Command_StartService;
		else if (_tcsicmp(pszParam, _T("stop")) == 0)
			m_nShellCommand = Command_StopService;
		else if (_tcsicmp(pszParam, _T("install")) == 0)
			m_nShellCommand = Command_InstallService;
		else if ( (_tcsicmp(pszParam, _T("remove")) == 0) ||
			(_tcsicmp(pszParam, _T("uninstall")) == 0) )
			m_nShellCommand = Command_UninstallService;
		else if ( (_tcsicmp(pszParam, _T("stopAndRemove")) == 0) ||
			(_tcsicmp(pszParam, _T("stopAndUninstall")) == 0) )
			m_nShellCommand = Command_StopAndUninstallService;
		else if (_tcsicmp(pszParam, _T("debug")) == 0)
			m_nShellCommand = Command_DebugService;
		else if ( (_tcsicmp(pszParam, _T("help")) == 0) ||
			(_tcsicmp(pszParam, _T("?")) == 0) )
			m_nShellCommand = Command_ShowServiceHelp;
		else if (_tcsicmp(pszParam, _T("Inject")) == 0)
			m_nShellCommand = Command_Inject;
		else if (_tcsicmp(pszParam, _T("UnInject")) == 0)
			m_nShellCommand = Command_UnInject;
		else if (_tcsicmp(pszParam, _T("InjectOGL")) == 0)
			m_nShellCommand = Command_OGLInject;
		else if (_tcsicmp(pszParam, _T("UnInjectOGL")) == 0)
			m_nShellCommand = Command_OGLUnInject;
		else if (_tcsicmp(pszParam, _T("SelfUpdate")) == 0)
			m_nShellCommand = Command_SelfUpdate;
		else if (_tcsicmp(pszParam, _T("ShowUninstallWindow")) == 0)
			m_nShellCommand = Command_ShowUninstallWindow;
	}

#ifdef UNICODE
	LocalFree(__wargv);
	__wargv = NULL;
#endif

	return m_nShellCommand;
}

//Based upon the function of the same name in CWinApp
BOOL CNTService::ProcessShellCommand()
{
	BOOL bResult = FALSE;
	switch (m_nShellCommand)
	{
	case Command_RunAsService:
		bResult = Run();
		break;

	case Command_StartService:
		bResult = StartServiceEx();
		break;
	
	case Command_StopService:
	
		bResult = StopServiceEx();
		break;
	
	case Command_InstallService:
		bResult = InstallEx();
		break;

	case Command_UninstallService:
		bResult = Uninstall();
		break;

	case Command_StopAndUninstallService:
		StopServiceEx();
#ifndef WIN64
		if (IsWin64()) // try fix issue when we can't uninstall driver without restart
			Sleep(5000);
#endif
		bResult = Uninstall();
		break;
	
	case Command_SelfUpdate:
		bResult = SelfUpdate();
		break;

	case Command_DebugService:
		Debug();
		bResult = TRUE;
		break;

	case Command_ShowServiceHelp:
		ShowHelp();
		bResult = TRUE;
		break;

	case Command_ShowUninstallWindow:
		bResult = ShowUninstallWindow();
		break;
	}
	return bResult;
}

///////////////////////// CNTScmService implementation ////////////////////////
CNTScmService::CNTScmService()
{
	m_hService = NULL;
}

CNTScmService::~CNTScmService()
{
	Close();
}

void CNTScmService::Close()
{
	if (m_hService)
	{
		CloseServiceHandle(m_hService);
		m_hService = NULL;
	}
}

CNTScmService::operator SC_HANDLE() const
{
	return m_hService;
}

BOOL CNTScmService::Attach(SC_HANDLE hService)
{
	if (m_hService != hService)
		Close();

	m_hService = hService;
	return TRUE;
}

SC_HANDLE CNTScmService::Detach()
{
	SC_HANDLE hReturn = m_hService;
	m_hService = NULL;
	return hReturn;
}

BOOL CNTScmService::ChangeConfig(DWORD dwServiceType,	DWORD dwStartType,
								 DWORD dwErrorControl, LPCTSTR lpBinaryPathName,
								 LPCTSTR lpLoadOrderGroup, LPDWORD lpdwTagId,
								 LPCTSTR lpDependencies, LPCTSTR lpServiceStartName,
								 LPCTSTR lpPassword, LPCTSTR lpDisplayName) const
{
	_ASSERT(m_hService != NULL);
	BOOL bSuccess = ::ChangeServiceConfig(m_hService, dwServiceType, dwStartType,
		dwErrorControl, lpBinaryPathName, lpLoadOrderGroup, lpdwTagId,
		lpDependencies, lpServiceStartName, lpPassword, lpDisplayName);
	if (!bSuccess)
		TRACE(_T("Failed in call to ChangeServiceConfig in ChangeConfig, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Control(DWORD dwControl)
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = ::ControlService(m_hService, dwControl, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Control, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Stop() const
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = ::ControlService(m_hService, SERVICE_CONTROL_STOP, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Stop, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Pause() const
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = ::ControlService(m_hService, SERVICE_CONTROL_PAUSE, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Pause, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Continue() const
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = ::ControlService(m_hService, SERVICE_CONTROL_CONTINUE, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Continue, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Interrogate() const
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = ::ControlService(m_hService, SERVICE_CONTROL_INTERROGATE, &ServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Interrogate, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Start(DWORD dwNumServiceArgs,	LPCTSTR* lpServiceArgVectors) const
{
	_ASSERT(m_hService != NULL);
	BOOL bSuccess = ::StartService(m_hService, dwNumServiceArgs, lpServiceArgVectors);
	if (!bSuccess)
		TRACE(_T("Failed in call to ControlService in Start, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::AcceptStop(BOOL& bStop)
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = QueryStatus(&ServiceStatus);
	if (bSuccess)
		bStop = ((ServiceStatus.dwControlsAccepted & SERVICE_ACCEPT_STOP) != 0);
	else
		TRACE(_T("Failed in call to QueryStatus in AcceptStop, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTScmService::AcceptPauseContinue(BOOL& bPauseContinue)
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = QueryStatus(&ServiceStatus);
	if (bSuccess)
		bPauseContinue = ((ServiceStatus.dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE) != 0);
	else
		TRACE(_T("Failed in call to QueryStatus in AcceptPauseContinue, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTScmService::AcceptShutdown(BOOL& bShutdown)
{
	_ASSERT(m_hService != NULL);
	SERVICE_STATUS ServiceStatus;
	BOOL bSuccess = QueryStatus(&ServiceStatus);
	if (bSuccess)
		bShutdown = ((ServiceStatus.dwControlsAccepted & SERVICE_ACCEPT_SHUTDOWN) != 0);
	else
		TRACE(_T("Failed in call to QueryStatus in AcceptShutdown, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTScmService::QueryStatus(LPSERVICE_STATUS lpServiceStatus) const
{
	_ASSERT(m_hService != NULL);
	BOOL bSuccess = ::QueryServiceStatus(m_hService, lpServiceStatus);
	if (!bSuccess)
		TRACE(_T("Failed in call to QueryServiceStatus in QueryStatus, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::WaitForState(DWORD state, DWORD maxWaitTime)
{
	SERVICE_STATUS status;
	DWORD timeLimit = timeGetTime() + maxWaitTime;
	for(QueryStatus(&status); status.dwCurrentState != state; QueryStatus(&status))
	{
		if(timeGetTime() > timeLimit)
			return FALSE;
		Sleep(status.dwWaitHint);
	}
	return TRUE;
}

BOOL CNTScmService::QueryConfig(LPQUERY_SERVICE_CONFIG& lpServiceConfig) const
{
	_ASSERT(m_hService != NULL);
	_ASSERT(lpServiceConfig == NULL); //To prevent double overwrites, this function
	//asserts if you do not send in a NULL pointer

	DWORD dwBytesNeeded;
	BOOL bSuccess = ::QueryServiceConfig(m_hService, NULL, 0, &dwBytesNeeded);
	if (!bSuccess && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		lpServiceConfig = (LPQUERY_SERVICE_CONFIG) DNew BYTE[dwBytesNeeded];
		DWORD dwSize;
		bSuccess = ::QueryServiceConfig(m_hService, lpServiceConfig, dwBytesNeeded, &dwSize);
	}

	if (!bSuccess)
		TRACE(_T("Failed in call to QueryServiceConfig in QueryConfig, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::Create(CNTServiceControlManager& Manager, LPCTSTR lpServiceName, LPCTSTR lpDisplayName,	  
						   DWORD dwDesiredAccess,	DWORD dwServiceType, DWORD dwStartType,	DWORD dwErrorControl,	    
						   LPCTSTR lpBinaryPathName, LPCTSTR lpLoadOrderGroup, LPDWORD lpdwTagId,	        
						   LPCTSTR lpDependencies, LPCTSTR lpServiceStartName, LPCTSTR lpPassword)
{
	Close();
	m_hService = ::CreateService(Manager, lpServiceName, lpDisplayName,	dwDesiredAccess, dwServiceType, 
		dwStartType,	dwErrorControl,	lpBinaryPathName, lpLoadOrderGroup, 
		lpdwTagId,	lpDependencies, lpServiceStartName, lpPassword);
	if (m_hService == NULL)
		TRACE(_T("Failed in call to CreateService in Create, GetLastError:%d\n"), ::GetLastError());

	return (m_hService != NULL);
}

BOOL CNTScmService::Delete() const
{
	_ASSERT(m_hService != NULL);
	BOOL bSuccess = ::DeleteService(m_hService);
	if (!bSuccess)
		TRACE(_T("Failed in call to DeleteService in Delete, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::SetObjectSecurity(SECURITY_INFORMATION dwSecurityInformation,
									  PSECURITY_DESCRIPTOR lpSecurityDescriptor) const

{
	_ASSERT(m_hService != NULL);
	BOOL bSuccess = ::SetServiceObjectSecurity(m_hService, dwSecurityInformation, lpSecurityDescriptor);
	if (!bSuccess)
		TRACE(_T("Failed in call to SetServiceObjectSecurity in SetObjectSecurity, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTScmService::QueryObjectSecurity(SECURITY_INFORMATION dwSecurityInformation,
										PSECURITY_DESCRIPTOR& lpSecurityDescriptor) const
{
	_ASSERT(m_hService != NULL);
	_ASSERT(lpSecurityDescriptor == NULL); //To prevent double overwrites, this function
	//asserts if you do not send in a NULL pointer

	DWORD dwBytesNeeded;
	BOOL bSuccess = ::QueryServiceObjectSecurity(m_hService, dwSecurityInformation, NULL, 0, &dwBytesNeeded);
	if (!bSuccess && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		lpSecurityDescriptor = (PSECURITY_DESCRIPTOR) DNew BYTE[dwBytesNeeded];
		DWORD dwSize;
		bSuccess = ::QueryServiceObjectSecurity(m_hService, dwSecurityInformation, lpSecurityDescriptor, dwBytesNeeded, &dwSize);
	}

	if (!bSuccess)
		TRACE(_T("Failed in call to QueryServiceObjectSecurity in QueryObjectSecurity, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTScmService::EnumDependents(DWORD dwServiceState, DWORD dwUserData, ENUM_SERVICES_PROC lpEnumServicesFunc) const
{
	_ASSERT(m_hService != NULL);

	DWORD dwBytesNeeded;
	DWORD dwServices;
	BOOL bSuccess = ::EnumDependentServices(m_hService, dwServiceState, NULL, 0, &dwBytesNeeded, &dwServices);
	if (!bSuccess && ::GetLastError() == ERROR_MORE_DATA)
	{
		BYTE* lpServices = DNew BYTE[dwBytesNeeded];
		DWORD dwSize;
		bSuccess = ::EnumDependentServices(m_hService, dwServiceState, (LPENUM_SERVICE_STATUS) lpServices, dwBytesNeeded, &dwSize, &dwServices);
		if (bSuccess)
		{
			BOOL bContinue = TRUE;
			for (DWORD i=0; i<dwServices; i++)
				bContinue = lpEnumServicesFunc(dwUserData, *(LPENUM_SERVICE_STATUS)(lpServices + i*sizeof(ENUM_SERVICE_STATUS)));
		}
		delete [] lpServices;
	}

	if (!bSuccess)
		TRACE(_T("Failed in call to EnumDependentServices in EnumDependents, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}


///////////////////////// CNTServiceControlManager implementation /////////////
CNTServiceControlManager::CNTServiceControlManager()
{
	m_hSCM = NULL;
	m_hLock = NULL;
}

CNTServiceControlManager::~CNTServiceControlManager()
{
	Unlock();
	Close();
}

CNTServiceControlManager::operator SC_HANDLE() const
{
	return m_hSCM;
}

BOOL CNTServiceControlManager::Attach(SC_HANDLE hSCM)
{
	if (m_hSCM != hSCM)
		Close();

	m_hSCM = hSCM;
	return TRUE;
}

SC_HANDLE CNTServiceControlManager::Detach()
{
	SC_HANDLE hReturn = m_hSCM;
	m_hSCM = NULL;
	return hReturn;
}

BOOL CNTServiceControlManager::Open(LPCTSTR pszMachineName, DWORD dwDesiredAccess)
{
	Close();
	m_hSCM = ::OpenSCManager(pszMachineName, SERVICES_ACTIVE_DATABASE, dwDesiredAccess);

	if (m_hSCM == NULL)
	{
		DWORD err = ::GetLastError();
		LPTSTR errMsg = NULL;
		::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, err, 0, (LPTSTR)&errMsg, 0, NULL);
		TRACE(_T("Failed in call to OpenSCManager in Open, GetLastError: %d\n\t%s"), err, errMsg);
		LocalFree(errMsg);
	}
	return (m_hSCM != NULL);
}

void CNTServiceControlManager::Close()
{
	if (m_hSCM)
	{
		::CloseServiceHandle(m_hSCM);
		m_hSCM = NULL;
	}
}

BOOL CNTServiceControlManager::QueryLockStatus(LPQUERY_SERVICE_LOCK_STATUS& lpLockStatus) const
{
	_ASSERT(m_hSCM != NULL);
	_ASSERT(lpLockStatus == NULL); //To prevent double overwrites, this function
	//asserts if you do not send in a NULL pointer

	DWORD dwBytesNeeded;
	BOOL bSuccess = ::QueryServiceLockStatus(m_hSCM, NULL, 0, &dwBytesNeeded);
	if (!bSuccess && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		lpLockStatus = (LPQUERY_SERVICE_LOCK_STATUS) DNew BYTE[dwBytesNeeded];
		DWORD dwSize;
		bSuccess = ::QueryServiceLockStatus(m_hSCM, lpLockStatus, dwBytesNeeded, &dwSize);
	}

	if (!bSuccess)
		TRACE(_T("Failed in call to QueryServiceLockStatus in QueryLockStatus, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTServiceControlManager::EnumServices(DWORD dwServiceType, DWORD dwServiceState, DWORD dwUserData, ENUM_SERVICES_PROC lpEnumServicesFunc) const
{
	_ASSERT(m_hSCM != NULL);

	DWORD dwBytesNeeded;
	DWORD dwServices;
	DWORD dwResumeHandle = 0;
	BOOL bSuccess = ::EnumServicesStatus(m_hSCM, dwServiceType, dwServiceState, NULL, 0, &dwBytesNeeded, &dwServices, &dwResumeHandle);
	if (!bSuccess && ::GetLastError() == ERROR_MORE_DATA)
	{
		BYTE* lpServices = DNew BYTE[dwBytesNeeded];
		DWORD dwSize;
		bSuccess = ::EnumServicesStatus(m_hSCM, dwServiceType, dwServiceState, (LPENUM_SERVICE_STATUS) lpServices, dwBytesNeeded, &dwSize, &dwServices, &dwResumeHandle);
		if (bSuccess)
		{
			BOOL bContinue = TRUE;
			for (DWORD i=0; i<dwServices; i++)
				bContinue = lpEnumServicesFunc(dwUserData, *(LPENUM_SERVICE_STATUS)(lpServices + i*sizeof(ENUM_SERVICE_STATUS)));
		}
		delete [] lpServices;
	}

	if (!bSuccess)
		TRACE(_T("Failed in call to EnumServicesStatus in EnumServices, GetLastError:%d\n"), ::GetLastError());
	return bSuccess;
}

BOOL CNTServiceControlManager::OpenService(LPCTSTR lpServiceName,	DWORD dwDesiredAccess, CNTScmService& service) const
{
	_ASSERT(m_hSCM != NULL);

	SC_HANDLE hService = ::OpenService(m_hSCM, lpServiceName, dwDesiredAccess);
	if (hService != NULL)
		service.Attach(hService);
	else
		TRACE(_T("Failed in call to OpenService in OpenService, GetLastError:%d\n"), ::GetLastError());

	return (hService != NULL);
}

BOOL CNTServiceControlManager::Lock()
{
	_ASSERT(m_hSCM != NULL);

	m_hLock = LockServiceDatabase(m_hSCM);
	if (m_hLock == NULL)
		TRACE(_T("Failed in call to LockServiceDatabase in Lock, GetLastError:%d\n"), ::GetLastError());
	return (m_hLock != NULL);
}

BOOL CNTServiceControlManager::Unlock()
{
	BOOL bSuccess = TRUE;
	if (m_hLock)
	{
		bSuccess = ::UnlockServiceDatabase(m_hLock);
		if (!bSuccess)
			TRACE(_T("Failed in call to UnlockServiceDatabase in Unlock, GetLastError:%d\n"), ::GetLastError());
		m_hLock = NULL;
	}

	return bSuccess;
}

///////////////////////// CNTEventLogSource implementation ////////////////////
CNTEventLogSource::CNTEventLogSource()
{
	m_hEventSource = NULL;
}

CNTEventLogSource::~CNTEventLogSource()
{
	Deregister();
}

CNTEventLogSource::operator HANDLE() const
{
	return m_hEventSource;
}

BOOL CNTEventLogSource::Attach(HANDLE hEventSource)
{
	if (m_hEventSource != hEventSource)
		Deregister();

	m_hEventSource = hEventSource;
	return TRUE;
}

HANDLE CNTEventLogSource::Detach()
{
	HANDLE hReturn = m_hEventSource;
	m_hEventSource = NULL;
	return hReturn;
}

BOOL CNTEventLogSource::Register(LPCTSTR lpUNCServerName, LPCTSTR lpSourceName)
{
	Deregister();
	m_hEventSource = ::RegisterEventSource(lpUNCServerName, lpSourceName);
	if (m_hEventSource == NULL)
		TRACE(_T("Failed in call to RegisterEventSource in Register, GetLastError:%d\n"), ::GetLastError());
	return (m_hEventSource != NULL);
}

BOOL CNTEventLogSource::Report(WORD wType, WORD wCategory, DWORD dwEventID, PSID lpUserSid,
							   WORD wNumStrings, DWORD dwDataSize, LPCTSTR* lpStrings, LPVOID lpRawData) const
{
	_ASSERT(m_hEventSource != NULL);

	//If we were send a NULL SID, then automatically create
	//one prior to calling the SDK ReportEvent. This really should 
	//have been done internally in ReportEvent.

	//First get the user name
	DWORD dwUserNameSize = UNLEN + 1;
	TCHAR szUserName[UNLEN + 1];
	::GetUserName(szUserName, &dwUserNameSize);

	DWORD dwDomainNameSize = UNLEN + 1;
	TCHAR szDomainName[UNLEN + 1] = _T("");

	SID_NAME_USE accountType; 
	DWORD dwSidSize = 0;

	PSID lpSid = lpUserSid;
	BYTE* lpBuffer = NULL;
	if (::LookupAccountName(NULL, szUserName, NULL, &dwSidSize, szDomainName, 
		&dwDomainNameSize, &accountType) == FALSE) 
	{
		lpBuffer = DNew BYTE[dwSidSize];
		if (::LookupAccountName(NULL, szUserName, lpBuffer, &dwSidSize, szDomainName, 
			&dwDomainNameSize, &accountType)) 
			lpSid = lpBuffer;
	}


	//Finally call the SDK version of the function
	BOOL bSuccess = ::ReportEvent(m_hEventSource, wType,	wCategory, dwEventID, lpSid,
		wNumStrings, dwDataSize, lpStrings, lpRawData);
	if (!bSuccess)
		TRACE(_T("Failed in call to ReportEvent in Report, GetLastError:%d\n"), ::GetLastError());

	//Delete any memory we may have used
	if (lpBuffer)
		delete [] lpBuffer;

	return bSuccess;
}

BOOL CNTEventLogSource::Report(WORD wType, DWORD dwEventID, LPCTSTR lpszString) const
{
	_ASSERT(lpszString);
	return Report(wType, 0, dwEventID, NULL, 1, 0, &lpszString, NULL);
}

BOOL CNTEventLogSource::Report(WORD wType, DWORD dwEventID, LPCTSTR lpszString1, LPCTSTR lpszString2) const
{
	_ASSERT(lpszString1);
	_ASSERT(lpszString2);
	LPCTSTR lpStrings[2];
	lpStrings[0] = lpszString1;
	lpStrings[1] = lpszString2;
	return Report(wType, 0, dwEventID, NULL, 2, 0, lpStrings, NULL);
}

BOOL CNTEventLogSource::Report(WORD wType, DWORD dwEventID, DWORD dwCode) const
{
	TCHAR sError[32];
	_stprintf_s(sError, _countof(sError), _T("%d"), dwCode);
	return Report(wType, dwEventID, sError);
}

BOOL CNTEventLogSource::Deregister()
{
	BOOL bSuccess = TRUE;
	if (m_hEventSource != NULL)
	{
		bSuccess = ::DeregisterEventSource(m_hEventSource);
		if (!bSuccess)
			TRACE(_T("Failed in call to DeregisterEventSource in Deregister, GetLastError:%d\n"), ::GetLastError());
		m_hEventSource = NULL;
	}

	return bSuccess;
}

BOOL CNTEventLogSource::Install(LPCTSTR lpSourceName, LPCTSTR lpEventMessageFile, DWORD dwTypesSupported)
{
	//Validate our parameters
	_ASSERT(lpSourceName);
	_ASSERT(lpEventMessageFile);

	//Make the necessary updates to the registry
	BOOL bSuccess = FALSE;
	HKEY hAppKey;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"), 0, 
		KEY_WRITE|KEY_READ, &hAppKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		HKEY hSourceKey;
		if (RegCreateKeyEx(hAppKey, lpSourceName, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
			&hSourceKey, &dw) == ERROR_SUCCESS)
		{
			//Write the Message file string
			bSuccess = (RegSetValueEx(hSourceKey, _T("EventMessageFile"), NULL, REG_SZ, (LPBYTE)lpEventMessageFile, 
				(DWORD)(_tcslen(lpEventMessageFile)+1)*sizeof(TCHAR)) == ERROR_SUCCESS);
			if (!bSuccess)
				TRACE(_T("Failed in call to RegSetValueEx in Install, GetLastError:%d\n"), ::GetLastError());


			//Write the Types supported dword
			bSuccess = bSuccess && (RegSetValueEx(hSourceKey, _T("TypesSupported"), NULL, REG_DWORD,
				(LPBYTE)&dwTypesSupported, sizeof(dwTypesSupported)) == ERROR_SUCCESS);
			if (!bSuccess)
				TRACE(_T("Failed in call to RegSetValueEx in Install, GetLastError:%d\n"), ::GetLastError());

			//Close the registry key we opened
			::RegCloseKey(hSourceKey);

			//Update the sources registry key so that the event viewer can filter 
			//on the events which we write to the event log
			LPTSTR sources = NULL;
			if (GetStringArrayFromRegistry(hAppKey, _T("Sources"), &sources))
			{
				//If our name is not in the array then add it
				BOOL bFoundMyself = FALSE;
				LPTSTR lpszStrings = sources;
				while (lpszStrings[0] != 0 && !bFoundMyself)
				{
					if(_tcscmp(lpSourceName, lpszStrings) == 0)
						bFoundMyself = TRUE;
					size_t len =_tcslen(lpszStrings);
					size_t MaximumServiceNameLength = 256;
					if (len <= MaximumServiceNameLength)
						lpszStrings += (len + 1);
					else // fix our old bug
						lpszStrings[0] = '\0';
				}
				if (!bFoundMyself)
				{
					_tcscpy(lpszStrings, lpSourceName);
					lpszStrings[_tcslen(lpSourceName) + 1] = 0;
					SetStringArrayIntoRegistry(hAppKey, _T("Sources"), sources);
				}
				delete [] sources;
			}
		}
		else
			TRACE(_T("Failed in call to RegCreateKeyEx in Install, GetLastError:%d\n"), ::GetLastError());

		//Close the registry key we opened
		::RegCloseKey(hAppKey);
	}
	else
		TRACE(_T("Failed in call to RegOpenKeyEx in Install, GetLastError:%d\n"), ::GetLastError());

	return bSuccess;
}

BOOL CNTEventLogSource::Uninstall(LPCTSTR lpSourceName)
{
	//Validate our parameters
	_ASSERT(lpSourceName);

	//Remove the settings from the registry
	TCHAR sSubKey[MAX_PATH];
	_stprintf_s(sSubKey, _countof(sSubKey), _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), lpSourceName);
	BOOL bSuccess = (RegDeleteKey(HKEY_LOCAL_MACHINE, sSubKey) == ERROR_SUCCESS);
	if (!bSuccess)
		TRACE(_T("Failed in call to RegDeleteKey in Uninstall, GetLastError:%d\n"), ::GetLastError());

	//Remove ourself from the "Sources" registry key
	HKEY hAppKey;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"), 0, KEY_WRITE|KEY_READ, &hAppKey) == ERROR_SUCCESS)
	{
		LPTSTR sources;
		if (GetStringArrayFromRegistry(hAppKey, _T("Sources"), &sources))
		{
			DWORD sourceSize = GetMultiStringLenght(sources);
			LPTSTR destination = DNew TCHAR[sourceSize];
			ZeroMemory(destination, sourceSize * sizeof(TCHAR));
			LPTSTR lpszInStrings = sources;
			LPTSTR lpszOutStrings = destination;
			BOOL bFoundMyself = FALSE;
			while (lpszInStrings[0] != 0)
			{
				size_t len = _tcslen(lpszInStrings);
				if(_tcscmp(lpSourceName, lpszInStrings) == 0)
					bFoundMyself = TRUE;
				else
				{
					_tcscpy_s(lpszOutStrings, len+1, lpszInStrings);
					lpszOutStrings += len + 1;
				}
				lpszInStrings += len + 1;
			}
			if (bFoundMyself)
			{
				lpszOutStrings[1] = 0; 
				SetStringArrayIntoRegistry(hAppKey, _T("Sources"), destination);
			}
			delete [] destination;
			delete [] sources;
		}

		//Close the registry key we opened
		::RegCloseKey(hAppKey);
	}

	return bSuccess;
}

DWORD CNTEventLogSource::GetMultiStringLenght(LPCTSTR multiString)
{
	if(multiString == NULL)
		return 0;

	//Work out the size of the buffer we will need
	DWORD dwSize = 0;
	while (multiString[dwSize] != 0)
		dwSize += (DWORD)_tcslen(multiString + dwSize) + 1;

	//Need one second NULL for the double NULL at the end
	dwSize+=1;
	return dwSize;
}

BOOL CNTEventLogSource::GetStringArrayFromRegistry(HKEY hKey, LPCTSTR sEntry, LPTSTR* multiString)
{
	//Validate our input parameters
	_ASSERT(hKey);
	_ASSERT(sEntry);

	//First find out the size of the key
	DWORD dwLongestValueDataLength;
	DWORD dwError = ::RegQueryInfoKey(hKey, NULL, NULL, (LPDWORD) NULL, NULL, NULL, NULL, NULL, NULL, &dwLongestValueDataLength, NULL, NULL);
	if (dwError != ERROR_SUCCESS)
		return FALSE;

	DWORD size = dwLongestValueDataLength + MAX_PATH * sizeof(TCHAR);

	//Allocate some memory and additional memory to retrieve the data back into
	*multiString = (LPTSTR) DNew TCHAR[size];

	DWORD dwDataType;
	dwError = ::RegQueryValueEx(hKey, sEntry, NULL, &dwDataType, (LPBYTE)*multiString, &size);

	if ((dwError != ERROR_SUCCESS) || (dwDataType != REG_MULTI_SZ))
	{
		delete [] *multiString;
		*multiString = NULL;
		return FALSE;
	}
	if (size == 0)
	{
		(*multiString)[0] = '\0';
		(*multiString)[1] = '\0';
	}

	return TRUE;
}

BOOL CNTEventLogSource::SetStringArrayIntoRegistry(HKEY hKey, LPCTSTR sEntry, LPCTSTR multiString)
{   
	//Validate our input parameters
	_ASSERT(hKey);
	_ASSERT(sEntry);

	//Work out the size of the buffer we will need
	DWORD dwSize = GetMultiStringLenght(multiString);

	//Write it into the registry
	DWORD dwError = ::RegSetValueEx(hKey, sEntry, NULL, REG_MULTI_SZ, (LPBYTE)multiString, dwSize * sizeof(TCHAR));

	if (dwError == ERROR_SUCCESS)
	{
#ifdef _DEBUG
		LPTSTR newString = NULL;
		if (GetStringArrayFromRegistry(hKey, _T("Sources"), &newString))
		{
			DWORD dwNewSize = GetMultiStringLenght(multiString);
			if (dwSize == dwNewSize)
			{
				_ASSERT(memcmp(multiString, newString, dwSize) == 0);
			}
			else
			{
				_ASSERT(FALSE);
			}
			delete[] newString;
		}
#endif
		return TRUE;
	}

	return FALSE;
}

