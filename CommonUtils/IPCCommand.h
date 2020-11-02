
#pragma once

#define SERVICE_IPC_CHANNEL			TEXT("\\\\.\\pipe\\S3D IPCChannel")

#define PROTOCOL_VERSION 0x3

#define NOT_INJECTED 0
#define D3D_INJECTED 1
#define OGL_INJECTED 2

enum IPCCommand { 
	IPCCOMMAND_GET_PROTOCOL_VERSION	= 0, 
	IPCCOMMAND_GET_INJECTION_STATUS	= 1, 
	IPCCOMMAND_UNINJECT_D3D			= 2,
	IPCCOMMAND_UNINJECT_OGL			= 3, 
	IPCCOMMAND_INJECT_D3D			= 4, 
	IPCCOMMAND_INJECT_OGL			= 5, 
	IPCCOMMAND_UPDATE				= 6
};
enum IPCAnswer  { IPCANSWER_OK, IPCANSWER_ERROR };

//----------------------------------- IPC commands --------------------------------------
struct IPCCommandHeader
{
	DWORD id;				// in:command index	/	out: error or success
};

struct IPCBasicCommand : IPCCommandHeader
{
	DWORD param1;			// in:				/	out: digital result (if any)
	WCHAR path1[MAX_PATH];	// in: path			/	out: error/success message  (always)
	WCHAR path2[MAX_PATH];	// in: path			/	out: 
};

TCHAR* SendIPCCommand(TCHAR* channelName, void *pMessageBuf, DWORD dwMessageLen, void* pAnswerBuf, DWORD* dwAnswerLen);
