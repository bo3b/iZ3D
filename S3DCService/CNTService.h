/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

// Command from command line
enum CNTServiceCommand
{ 
	Command_RunAsService,				//--- Start of CNTService specific commands range ---
	Command_StartService,
	Command_StopService,
	Command_InstallService, 
	Command_UninstallService,
	Command_StopAndUninstallService,
	Command_SelfUpdate,
	Command_ShowUninstallWindow,
	Command_DebugService,
	Command_ShowServiceHelp,			//--- End of CNTService specific commands range ---

	Command_Start_S3DServiceCommand,	//--- Start of S3DService control commands range ---
	Command_Inject,
	Command_UnInject,
	Command_OGLInject,
	Command_OGLUnInject,
	Command_End_S3DServiceCommand		//--- End of S3DService control commands range ---
};

class CAutoLocker
{
	LPCRITICAL_SECTION m_pCS;
public:
	CAutoLocker(LPCRITICAL_SECTION pCS) { m_pCS = pCS; if (m_pCS) EnterCriticalSection(m_pCS); }
	~CAutoLocker() { if (m_pCS) LeaveCriticalSection(m_pCS); }
};

enum ResponseStatus
{
	Response_Failed,
	Response_Success,
	Response_AlreadyDone,
};

//An encapsulation of the APIs used to register, unregister,
//write, install and uninstall Event log entries i.e. the
//server side to the Event log APIs
class CNTEventLogSource
{
public:
	//Constructors / Destructors
	CNTEventLogSource();
	~CNTEventLogSource();

	//Methods
	operator HANDLE() const;
	BOOL Attach(HANDLE hEventSource);
	HANDLE Detach();
	BOOL Register(LPCTSTR lpUNCServerName,	// server name for source 
				  LPCTSTR lpSourceName);	// source name for registered handle  
					
	BOOL Report(WORD wType,					// event type to log 
		WORD wCategory,						// event category 
		DWORD dwEventID,					// event identifier 
		PSID lpUserSid,						// user security identifier (optional) 
		WORD wNumStrings,					// number of strings to merge with message  
		DWORD dwDataSize,					// size of binary data, in bytes
		LPCTSTR* lpStrings,					// array of strings to merge with message 
		LPVOID lpRawData) const;			// address of binary data 
		
	BOOL Report(WORD wType, DWORD dwEventID, LPCTSTR lpszString) const;
	BOOL Report(WORD wType, DWORD dwEventID, LPCTSTR lpszString1, LPCTSTR lpszString2) const;
	BOOL Report(WORD wType, DWORD dwEventID, DWORD dwCode) const;
	BOOL Deregister();

	static BOOL Install(LPCTSTR lpSourceName, LPCTSTR lpEventMessageFile, DWORD dwTypesSupported);
	static BOOL Uninstall(LPCTSTR lpSourceName);

protected:
	static DWORD GetMultiStringLenght(LPCTSTR multiString);	//--- including all termination characters ---
	static BOOL  GetStringArrayFromRegistry(HKEY hKey, LPCTSTR sEntry, LPTSTR* multiString);
	static BOOL  SetStringArrayIntoRegistry(HKEY hKey, LPCTSTR sEntry, LPCTSTR multiString);

	HANDLE m_hEventSource;
	friend class CNTService;
};


//An MFC framework encapsulation of an NT service 
//You are meant to derive your own class from this and
//override its functions to implement your own 
//service specific functionality.
class CNTService
{
public:
	//Constructors / Destructors
	CNTService(LPCTSTR lpszServiceName, LPCTSTR lpszDisplayName, DWORD dwControlsAccepted, LPCTSTR lpszDescription = NULL); 
	~CNTService();

	//Accessors / Mutators
	LPCTSTR GetServiceName() const { return m_sServiceName; };
	LPCTSTR GetDisplayName() const { return m_sDisplayName; };
	LPCTSTR GetDescription() const { return m_sDescription; };

	//Other Methods
	//Helpful functions to parse the command line and execute the results
	CNTServiceCommand ParseCommandLine();
	BOOL ProcessShellCommand();
	CNTServiceCommand GetShellCommand() { return m_nShellCommand; }

	//Reports the status of this service back to the SCM
	BOOL ReportStatusToSCM();
	BOOL ReportStatusToSCM(DWORD dwCurrentState, DWORD dwWin32ExitCode, 
	DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);

	//Installs the callback funtion by calling RegisterServiceCtrlHandlerEx
	BOOL RegisterCtrlHandlerEx();

	//Member function which does the job of responding to SCM requests
	virtual void WINAPI ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData);						

	//The ServiceMain function for this service
	virtual void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);

	//Called in reponse to a shutdown request
	virtual void OnStop();

	//Called in reponse to a pause request
	virtual void OnPause();

	//Called in reponse to a continue request
	virtual void OnContinue();

	//Called in reponse to a Interrogate request
	virtual void OnInterrogate();

	//Called in reponse to a Shutdown request
	virtual void OnShutdown();

	//Called in reponse to a SessionChange request
	virtual void OnSessionChange(DWORD dwEventType, LPVOID lpEventData);

	//Called in reponse to a user defined request
	virtual void OnUserDefinedRequest(DWORD dwControl);

	//Kicks off the Service. You would normally call this
	//some where in your main/wmain or InitInstance
	//a standard process rather than as a service. If you are
	//using the CNTServiceCommandLineInfo class, then internally
	//it will call this function for you.
	virtual BOOL Run();

	// Start service from command line 
	virtual ResponseStatus StartService();

	// Start service from command line - wait while service started
	virtual BOOL StartServiceEx();

	// Start service from command line 
	virtual ResponseStatus StopService();

	// Start service from command line - wait while service stopped
	virtual BOOL StopServiceEx();

	//Install the service
	virtual BOOL Install();

	//Install the service - install and change service name in 64-bit OS 
	virtual BOOL InstallEx(); 

	//Uninstall the service
	virtual BOOL Uninstall();

	//Update the service executable
	virtual BOOL SelfUpdate();

	//Runs the service as a normal function as opposed to a service
	virtual void Debug();

	//Displays help for this service
	virtual void ShowHelp();

	//Displays uninstall window
	virtual bool ShowUninstallWindow();

protected:
	//Methods
	//These two static functions are used internally to
	//go from the SDK functions to the C++ member functions
	static DWORD WINAPI _ServiceCtrlHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	static void  WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);

	// Command from command line
	CNTServiceCommand     m_nShellCommand;

	//Data
	SERVICE_STATUS_HANDLE m_hStatus;
	DWORD                 m_dwControlsAccepted;		// What Control requests will this service repond to
	DWORD                 m_dwCurrentState;			// Current Status of the service
	TCHAR				  m_sServiceName[MAX_PATH]; // Name of the service
	TCHAR                 m_sDisplayName[MAX_PATH]; // Display name for the service
	TCHAR                 m_sDescription[MAX_PATH]; // The description text for the service
	CNTEventLogSource     m_EventLogSource;			// For reporting to the event log
	static CNTService*    sm_lpService;				// Static which contains the this pointer
	CRITICAL_SECTION      m_CritSect;				// Protects changes to any member variables from multiple threads
};


////// Forward declaration
class CNTServiceControlManager;


typedef BOOL (CALLBACK* ENUM_SERVICES_PROC)(DWORD dwData, ENUM_SERVICE_STATUS& Service);


//An encapsulation of a service as returned from querying the SCM (i.e. an SC_HANDLE)
class CNTScmService
{
public:
	//Constructors / Destructors
	CNTScmService();
	~CNTScmService();

	//Methods
	//Releases the underlying SC_HANDLE
	void Close();

	//Allows access to the underlying SC_HANDLE representing the service
	operator SC_HANDLE() const;

	//Attach / Detach support from an SDK SC_HANDLE
	BOOL Attach(SC_HANDLE hService);
	SC_HANDLE Detach();

	//Changes the configuration of this service
	BOOL ChangeConfig(DWORD dwServiceType,	// type of service 
		DWORD dwStartType,					// when to start service 
		DWORD dwErrorControl,				// severity if service fails to start 
		LPCTSTR lpBinaryPathName,			// pointer to service binary file name 
		LPCTSTR lpLoadOrderGroup,			// pointer to load ordering group name 
		LPDWORD lpdwTagId,					// pointer to variable to get tag identifier 
		LPCTSTR lpDependencies,				// pointer to array of dependency names 
		LPCTSTR lpServiceStartName,			// pointer to account name of service 
		LPCTSTR lpPassword,					// pointer to password for service account  
		LPCTSTR lpDisplayName) const;		// pointer to display name 

	//Send a defined control code to the service
	BOOL Control(DWORD dwControl);

	//These functions call Control() with the 
	//standard predefined control codes
	BOOL Stop() const;			 //Ask the service to stop
	BOOL Pause() const;			 //Ask the service to pause
	BOOL Continue() const;	 //Ask the service to continue
	BOOL Interrogate() const; //Ask the service to update its status to the SCM

	//Start the execution of the service
	BOOL Start(DWORD dwNumServiceArgs,				// number of arguments 
			   LPCTSTR* lpServiceArgVectors) const;	// address of array of argument string pointers  

	//Determines what Control codes this service supports
	BOOL AcceptStop(BOOL& bStop);                   //Ask the service can it stop
	BOOL AcceptPauseContinue(BOOL& bPauseContinue);	//Ask the service can it pause continue
	BOOL AcceptShutdown(BOOL& bShutdown);           //Ask the service if it is notified of shutdowns

	//Get the most return status of the service reported to the SCM by this service
	BOOL QueryStatus(LPSERVICE_STATUS lpServiceStatus) const;

	// Useful for command-line processing  
	BOOL WaitForState(DWORD status, DWORD maxWaitTime = 6000);

	//Get the configuration parameters of this service from the SCM
	BOOL QueryConfig(LPQUERY_SERVICE_CONFIG& lpServiceConfig) const;

	//Add a new service to the SCM database
	BOOL Create(CNTServiceControlManager& Manager,	// handle to service control manager database  
		LPCTSTR lpServiceName,						// pointer to name of service to start 
		LPCTSTR lpDisplayName,						// pointer to display name 
		DWORD dwDesiredAccess,						// type of access to service 
		DWORD dwServiceType,						// type of service 
		DWORD dwStartType,							// when to start service 
		DWORD dwErrorControl,						// severity if service fails to start 
		LPCTSTR lpBinaryPathName,					// pointer to name of binary file 
		LPCTSTR lpLoadOrderGroup,					// pointer to name of load ordering group 
		LPDWORD lpdwTagId,							// pointer to variable to get tag identifier 
		LPCTSTR lpDependencies,						// pointer to array of dependency names 
		LPCTSTR lpServiceStartName,					// pointer to account name of service 
		LPCTSTR lpPassword							// pointer to password for service account 
		);

	//Mark this service as to be deleted from the SCM.
	BOOL Delete() const;

	//Enumerate the services that this service depends upon
	BOOL EnumDependents(DWORD dwServiceState,							// state of services to enumerate 
						DWORD dwUserData,								// User defined data
						ENUM_SERVICES_PROC lpEnumServicesFunc) const;	// The callback function to use

	//Get the security information associated with this service
	BOOL QueryObjectSecurity(SECURITY_INFORMATION dwSecurityInformation,		// type of security information requested  
							 PSECURITY_DESCRIPTOR& lpSecurityDescriptor) const;	// address of security descriptor 

	//Set the security descriptor associated with this service
	BOOL SetObjectSecurity(SECURITY_INFORMATION dwSecurityInformation,			// type of security information requested  
						   PSECURITY_DESCRIPTOR lpSecurityDescriptor) const;	// address of security descriptor 

protected:
	SC_HANDLE m_hService;
};


//An encapsulation of the NT Service Control Manager
class CNTServiceControlManager
{
public:
	//Constructors / Destructors
	CNTServiceControlManager();
	~CNTServiceControlManager();

	//Methods
	//Allows access to the underlying SC_HANDLE representing the SCM
	operator SC_HANDLE() const;

	//Attach / Detach support from an SDK SC_HANDLE
	BOOL Attach(SC_HANDLE hSCM);
	SC_HANDLE Detach();

	//Opens a connection to the SCM
	BOOL Open(LPCTSTR pszMachineName, DWORD dwDesiredAccess);

	//Close the connection to the SCM
	void Close();																							 

	//Get the SCM Status
	BOOL QueryLockStatus(LPQUERY_SERVICE_LOCK_STATUS& lpLockStatus) const; 

	//Enumerates the specified services
	BOOL EnumServices(DWORD dwServiceType, DWORD dwServiceState, DWORD dwUserData, ENUM_SERVICES_PROC lpEnumServicesFunc) const;

	//Opens the specified service
	BOOL OpenService(LPCTSTR lpServiceName,	DWORD dwDesiredAccess, CNTScmService& service) const;

	//Lock the SCM database
	BOOL Lock();

	//Unlocks the SCM database
	BOOL Unlock();

protected:
	SC_HANDLE	m_hSCM;	 //Handle to the SCM
	SC_LOCK		m_hLock; //Handle of any lock on the Database
};
