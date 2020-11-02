/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"
#include <tinyxml.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include "Injector.h"
#include "ProductNames.h"
#include "..\..\CommonUtils\StringUtils.h"
#include "..\..\CommonUtils\CommonResourceFolders.h"

//--- some utilities functions ---
BOOL GetServicePath(CHAR fileName[MAX_PATH], BOOL removeLastSlash, BOOL returnExeName)
{
	if(HMODULE hMod = GetModuleHandle(NULL))
	{
		if(GetModuleFileNameA(hMod, fileName, MAX_PATH))
		{
			if(returnExeName)
				return TRUE;

			CHAR* s = strrchr(fileName, '\\');
			if(s)
			{
				if(!removeLastSlash)
					s++;
				*s = 0;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL GetServicePath(WCHAR fileName[MAX_PATH], BOOL removeLastSlash, BOOL returnExeName)
{
	if(HMODULE hMod = GetModuleHandle(NULL))
	{
		if(GetModuleFileNameW(hMod, fileName, MAX_PATH))
		{
			if(returnExeName)
				return TRUE;

			WCHAR* s = wcsrchr(fileName, _T('\\'));
			if(s)
			{
				if(!removeLastSlash)
					s++;
				*s = 0;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL ReadConfigValue(LPCSTR paramName)
{
	BOOL result = FALSE;
	TCHAR configFileName[MAX_PATH];
	BOOL res = FALSE;
	if(iz3d::resources::GetAllUsersDirectory( configFileName ))
		res = TRUE;
	else
		res = GetServicePath(configFileName, FALSE);
	if(res)
	{
		_tcscat_s(configFileName, MAX_PATH, CONFIG_XML_NAME);

		TiXmlDocument config( common::utils::to_multibyte( configFileName ) );
		if (config.LoadFile())
		{
			TiXmlElement* itemElement;
			TiXmlNode* rootNode  = config.FirstChild("Config");
			if(rootNode)
			{
				TiXmlNode* childNode = rootNode->FirstChild("GlobalSettings");
				if(childNode)
				{
					TiXmlNode* finalNode = childNode->FirstChild(paramName);
					if(finalNode)
					{
						itemElement = finalNode->ToElement();
						if(itemElement)
							itemElement->QueryIntAttribute("Value", (int*)&result);
					}
				}
			}
		}
		else
			TRACE(_T("File %s not found or corrupted\n"), configFileName);
	}

	return result;
}

// How many services are currently running
DWORD GetProcessCount()
{
	DWORD count = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE CTH = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	for(BOOL res = Process32First(CTH, &pe); res; res = Process32Next(CTH, &pe))
		if(_tcsicmp(pe.szExeFile, SERVICE_EXE_NAME) == 0)
			count++;
	CloseHandle(CTH);
	return count;
}

// Useful for service close by counting processes
BOOL WaitForProcessCount(DWORD expectedCount, DWORD maxWaitTime)
{
	if(expectedCount >= 0)
	{
		DWORD timeLimit = timeGetTime() + maxWaitTime;
		for(DWORD count = GetProcessCount(); count != expectedCount; count = GetProcessCount())
		{
			if(timeGetTime() > timeLimit)
				return FALSE;
			Sleep(0);
		}
	}
	return TRUE;
}

void GetShellPathName(TCHAR* path)
{
	_tcscpy_s(path, MAX_PATH, _T("c:\\windows\\explorer.exe"));
	//_tcscpy_s(path, MAX_PATH, "c:\\windows\\system32\\notepad.exe");
}

void GetProcessHandleList(const TCHAR* path, std::vector<HANDLE>& handleList)
{
	PROCESSENTRY32 pe;
	TCHAR ImageFileName[MAX_PATH];
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE CTH = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	for(BOOL res = Process32First(CTH, &pe); res; res = Process32Next(CTH, &pe))
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
		if(hProcess)
		{
			if(GetProcessImageFileName(hProcess, ImageFileName, _countof(ImageFileName)))
			{
				if(_tcsicmp(path, ImageFileName) == 0)
					handleList.push_back(hProcess);
			}
			CloseHandle(hProcess);
		}
	}
	CloseHandle(CTH);
}
