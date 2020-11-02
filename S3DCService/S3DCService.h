/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "CNTService.h"
#include "IPCService.h"

#define MAX_STR_SIZE 10000

class S3DCService : public CNTService
{
public:
	S3DCService();
	virtual BOOL Install();
	virtual BOOL Uninstall();
	virtual void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	virtual void OnStop();
	virtual void ShowHelp();
	virtual bool ShowUninstallWindow();
	
	IPCControlService driverControl;

	//--- add specific command-line processing ---
	BOOL ProcessShellCommand();

	int BuildAppList(TCHAR message[MAX_STR_SIZE]);
	bool AddToList(DWORD pid, TCHAR* exeFile, int &cnt, TCHAR* message, DWORD &size);
};

