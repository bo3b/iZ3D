/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <malloc.h>

static GUID g_magicTag =  { 0x5f9fd868, 0xb2a7, 0xbf42, 0xaa, 0x13, 0xe3, 0x39, 0x34, 0xe1, 0x2a, 0x20  };

// find g_magicTag in each ddl's
inline bool NoCheat(_TCHAR* dllName)
{
	bool b_NoCheat = false;
	HANDLE dllFile = CreateFile(dllName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(dllFile)
	{
		DWORD numberOfBytesRead;
		DWORD fileSize = GetFileSize(dllFile ,0);
		byte* p = (byte*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
		if(p)
		{
			BOOL readRes = ReadFile(dllFile, p, fileSize, &numberOfBytesRead, 0);
			if(readRes && numberOfBytesRead == fileSize)
			{
				for(DWORD i=0; i< fileSize-sizeof(g_magicTag); i++)
				{
					GUID* g = (GUID*)&p[i];
					if (InlineIsEqualGUID(*g, g_magicTag))
					{
						b_NoCheat = 1;
						break;
					}
				}	
				CloseHandle(dllFile);
			}
			HeapFree(GetProcessHeap(), 0, p);
		}
	}
	return b_NoCheat;
}
