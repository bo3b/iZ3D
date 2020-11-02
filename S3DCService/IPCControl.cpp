#include "stdafx.h"
#include <process.h>
#include <Sddl.h>
#include "IPCControl.h"

#define BUFFER_SIZE 4096

DWORD WINAPI PipeThread(IPCControl* lpvParam) 
{ 
	BYTE  chRequest[BUFFER_SIZE]; 
	BYTE  chReply[BUFFER_SIZE]; 
	DWORD cbBytesRead, cbReplyBytes, cbWritten; 
	BOOL  fSuccess; 

	while (!lpvParam->m_bStopThread) 
	{
		bool fConnected = ConnectNamedPipe(lpvParam->m_hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED); 
		if(fConnected)
		{
			// Read client requests from the pipe. 
			fSuccess = ReadFile(lpvParam->m_hPipe, chRequest, BUFFER_SIZE, &cbBytesRead, NULL);

			if (fSuccess && cbBytesRead != 0) 
			{
				lpvParam->PipeCallback(chRequest, cbBytesRead, chReply, &cbReplyBytes); 

				// Write the reply to the pipe.  
				fSuccess = WriteFile(lpvParam->m_hPipe,	chReply, cbReplyBytes, &cbWritten, NULL);        

				if (fSuccess && cbReplyBytes == cbWritten)
				{
					// Flush the pipe to allow the client to read the pipe's contents 
					FlushFileBuffers(lpvParam->m_hPipe);  
				}
			}
			DisconnectNamedPipe(lpvParam->m_hPipe); 
		}
	}
	return 0;
}

IPCControl::IPCControl(void)
{
	m_hPipe = 0;
	m_hThread = 0;
	m_bStopThread = FALSE;
}

IPCControl::~IPCControl(void)
{
	InterlockedExchange(&m_bStopThread, TRUE);
	if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 100))
		TerminateThread(m_hThread, 0);
	CloseHandle(m_hThread);
	CloseHandle(m_hPipe);
}

TCHAR* IPCControl::Initialize()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	TCHAR* strSecDesc = TEXT("D: (A;OICI;GRGWGX;;;AU)") // Allow 
														// read/write/execute 
														// to authenticated 
						TEXT("(A;OICI;GA;;;BA)");		// Allow full control 
														// to administrators
	ConvertStringSecurityDescriptorToSecurityDescriptor(strSecDesc , SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL);

	m_hPipe = CreateNamedPipe( 
#ifdef WIN64
	SERVICE_IPC_CHANNEL,				// pipe name 
#else	
	IsWin64() ? INTERSERVICE_IPC_CHANNEL : SERVICE_IPC_CHANNEL,
#endif	
	FILE_FLAG_FIRST_PIPE_INSTANCE |	// only one such type of pipe in system
	PIPE_ACCESS_DUPLEX,				// read/write access 
	PIPE_TYPE_BYTE |				// byte type pipe 
	PIPE_READMODE_BYTE |			// byte-read mode 
	PIPE_WAIT,						// blocking mode 
	PIPE_UNLIMITED_INSTANCES,		// max. instances  
	BUFFER_SIZE ,					// output buffer size 
	BUFFER_SIZE ,					// input buffer size 
	NMPWAIT_USE_DEFAULT_WAIT,		// client time-out 
	&sa);							// special security attribute 

	LocalFree(sa.lpSecurityDescriptor);

	if (m_hPipe == INVALID_HANDLE_VALUE) 
		return TEXT("Can't create pipe.");

	m_hThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))PipeThread, this, 0, NULL);
	if (m_hThread == NULL) 
		return TEXT("Can't create pipe thread.");

	return NULL;
}
