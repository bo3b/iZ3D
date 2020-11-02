#include "stdafx.h"
#include "IPCCommand.h"

TCHAR* SendIPCCommand(TCHAR* channelName, void *pMessageBuf, DWORD dwMessageLen, void* pAnswerBuf, DWORD* dwAnswerLen)
{
	TCHAR* message = TEXT("Could not open pipe."); 
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	int i = 0;
	while (i < 10)
	{
		hPipe = CreateFile(channelName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPipe !=  INVALID_HANDLE_VALUE)
			break;
		Sleep(100);
		++i;
	}        
	if(hPipe != INVALID_HANDLE_VALUE)
	{
		message = TEXT("Pipe is busy."); 
		if(GetLastError() != ERROR_PIPE_BUSY) 
		{
			message = TEXT("Write to pipe failed.");  
			DWORD cbWritten;
			if(WriteFile(hPipe, pMessageBuf, dwMessageLen, &cbWritten,	NULL))
			{
				message = TEXT("Failed to flush pipe."); 
				if(FlushFileBuffers(hPipe))
				{
					message = TEXT("Failed to read from pipe."); 
					DWORD bytesAvailable;
					ReadFile(hPipe, pAnswerBuf, 0, dwAnswerLen, NULL);
					PeekNamedPipe(hPipe, NULL, NULL, NULL, &bytesAvailable, NULL);
					_ASSERT(bytesAvailable <= dwMessageLen);
					if(ReadFile(hPipe, pAnswerBuf, bytesAvailable, dwAnswerLen, NULL)) 
						message = NULL; 
				}
			}
		}
		CloseHandle(hPipe); 
	}
	return message;
}




