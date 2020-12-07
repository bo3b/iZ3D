#pragma once
#include "IPCCommand.h"

#define INTERSERVICE_IPC_CHANNEL	TEXT("\\\\.\\pipe\\S3D InterServiceChannel")

// ToDo: System_Processes is no longer passed this way.
//#define DEFAULT_TARGET  ((HANDLE)(ALL_SESSIONS | SYSTEM_PROCESSES))
#define DEFAULT_TARGET  ((HANDLE)(ALL_SESSIONS))

class IPCControl
{
public:
	IPCControl(void);
	~IPCControl(void);

	TCHAR* Initialize();

protected:
	HANDLE m_hPipe;
	HANDLE m_hThread;
	LONG   m_bStopThread;
	virtual void PipeCallback(void *pMessageBuf, DWORD dwMessageLen, void*	pAnswerBuf, DWORD* dwAnswerLen) = 0;
	friend DWORD WINAPI PipeThread(IPCControl* lpvParam);
};

BOOL IsWin64();