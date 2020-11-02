/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "SynchroWindow.h"

class  MonitorPanel
{
public:
	// Filled by detect models
	BOOL bAttached;
	BOOL bPrimary;
	BOOL bFound;
	TCHAR szDeviceName[32];	// adapter device name
	TCHAR szModel[8];		// monitor model

	DWORD nModel;
	void ModelToNumber();

	DEVMODE	deviceMode;		// device mode
	void GetDisplaySettings();

	HMONITOR GetMonitorHandle();

	void AttachOrRestore();
	void SetPrimary();
	void ChangeSettings( DEVMODE* pDeviceMode );
	void ChangeSettings();
	MonitorPanel ()
	{
		bAttached = false;
		bPrimary = false;
		bFound = false;
		szDeviceName[0] = '\0';
		szModel[0] = '\0';
		nModel = false;
		ZeroMemory(&deviceMode, sizeof(deviceMode));
	}
};

extern void DetectPanels( MonitorPanel * pPrimary, MonitorPanel * pSecondary );

extern bool CheckBackPanel( const TCHAR* model );
extern bool CheckFrontPanel( const TCHAR* model );
