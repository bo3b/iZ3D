#include "StdAfx.h"
#include "GlobalData.h"
#include "ReadData.h"
#include "../../CommonUtils/System.h"

#ifdef ZLOG_ALLOW_TRACING

void InitializeLogging()
{
	_tcscpy(gInfo.LogFileDirectory, gInfo.DriverDirectory);
	PathAppend(gInfo.LogFileDirectory, _T("Logs"));
	CreateDirectory(gInfo.LogFileDirectory, 0);
	PathAppend(gInfo.LogFileDirectory, gInfo.ApplicationName);
	TCHAR* c = _tcsrchr(gInfo.LogFileDirectory, '.');
	if (c != NULL)
		c[0] = '\0';
	CreateDirectory(gInfo.LogFileDirectory, 0);

	//--- LogFileName ---
	_tcscpy(gInfo.LogFileName, gInfo.LogFileDirectory);
	PathAppend(gInfo.LogFileName, gInfo.ApplicationName);
	_tcscat(gInfo.LogFileName, _T(".log"));

	//--- Start ZLOg ---
	if ( !zlog::IsLoggerOpened() )
	{
		TCHAR configPath[MAX_PATH];
		GetConfigFullPath(configPath);
		zlog::InitializeFromXML(configPath, _T("Config.ZLOg"));
		zlog::ReopenLogger(gInfo.LogFileName, true);
	}
}

#endif