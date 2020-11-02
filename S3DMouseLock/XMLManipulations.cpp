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
#include <tchar.h>
#include <vector>
#include <string>
#include <algorithm>
#include <tinyxml.h>
#include <shlwapi.h>
#include "version.h"
#include "..\CommonUtils\StringUtils.h"
#include "..\CommonUtils\CommonResourceFolders.h"
#define  DONT_LOCK_FLAG_NAME		"DontLockMouse" 

using namespace std;
extern vector<string> g_ExcludedProcess;

extern HMODULE g_hModule;

void ParseXML()
{
	TCHAR szBaseProfile[MAX_PATH];
	TCHAR szUserProfile[MAX_PATH];

	//--- Build Directory ---
	TCHAR szDriverDirectory[MAX_PATH];
	GetModuleFileName(g_hModule, szDriverDirectory, _countof(szDriverDirectory));
	TCHAR* s = _tcsrchr(szDriverDirectory, '\\');
	if(s) 
		*s = 0;

	if(iz3d::resources::GetAllUsersDirectory( szBaseProfile ))
		PathAppend(szBaseProfile, TEXT("BaseProfile.xml"));

	if(iz3d::resources::GetCurrentUserDirectory( szUserProfile ))
		PathAppend(szUserProfile, TEXT("UserProfile.xml"));

	//--- parse base Profile XML ---
	TiXmlDocument baseProfile;
	if(baseProfile.LoadFile(common::utils::to_multibyte(szBaseProfile)))
	{
		TiXmlNode* node = 0;
		for(node = baseProfile.FirstChild("Profiles")->FirstChild("Profile"); node; node = node->NextSibling())
		{
			TiXmlElement* nameElement = node->FirstChildElement("File");
			if(nameElement)
			{
				const char* appName = nameElement->Attribute("Name");
				TiXmlElement* lockElement = node->FirstChildElement(DONT_LOCK_FLAG_NAME);
				if(lockElement)
				{
					int i = 0;
					const char* lockName = lockElement->Attribute("Value", &i);
					if(i) g_ExcludedProcess.push_back(appName);
				}
			}
		}
	}
	//--- parse user Profile XML ---
	TiXmlDocument userProfile;
	if(userProfile.LoadFile(common::utils::to_multibyte(szUserProfile)))
	{
		TiXmlNode* node = 0;
		for(node = userProfile.FirstChild("Profiles")->FirstChild("Profile"); node; node = node->NextSibling())
		{
			TiXmlElement* nameElement = node->FirstChildElement("File");
			if(nameElement)
			{
				//const char* appName = nameElement->Attribute("Name"); 
				TiXmlElement* lockElement = node->FirstChildElement(DONT_LOCK_FLAG_NAME);
				if(lockElement)
				{
					int i = 0;
					const char* appName = nameElement->Attribute("Name");
					const char* lockName = lockElement->Attribute("Value", &i);
					bool found = false;
					vector<string>::iterator it = find(g_ExcludedProcess.begin(), g_ExcludedProcess.end(), appName);
					if(it != g_ExcludedProcess.end())	found = true;
					if( found && i == 1) continue;
					if(!found && i == 0) continue;
					//--- add new founded ---
					if(!found && i == 1) g_ExcludedProcess.push_back(appName);
					//--- remove previously added ---
					if( found && i == 0) g_ExcludedProcess.erase(it);
				}
			}
		}
	}
}

void BuildExcludedProcessList(vector<string>& excludedProcess)
{
	ParseXML();
}
