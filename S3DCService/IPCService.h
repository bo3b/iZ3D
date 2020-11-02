/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <windows.h>
#include "CNTService.h"
#include "IPCControl.h"
#include "IPCCommand.h"

class IPCControlService : public IPCControl
{
public:
	IPCControlService(CNTEventLogSource* log);
	~IPCControlService();

	// true - no errors
	bool InjectD3D(const WCHAR* path = NULL);
	bool InjectOGL(const WCHAR* path = NULL);
	bool UnInjectD3D();
	bool UnInjectOGL();

protected:
	CNTEventLogSource* m_Log;

	WCHAR pathD3DInjector[MAX_PATH];
	WCHAR pathOGLInjector[MAX_PATH];

	bool InternalInject(const WCHAR* path, const WCHAR* DllName, WCHAR* WathToSaveDllName, HANDLE Target = DEFAULT_TARGET);
	bool InternalUnInject(WCHAR* savedDllPath, HANDLE Target = DEFAULT_TARGET);
	virtual void PipeCallback(void *pMessageBuf, DWORD dwMessageLen, void*	pAnswerBuf, DWORD* dwAnswerLen);
};
