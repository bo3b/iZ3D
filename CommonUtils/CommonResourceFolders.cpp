#include "StdAfx.h"
#include "CommonResourceFolders.h"
#include <Shlobj.h>
#include <Shlwapi.h>
#include <string.h>
#include <tchar.h>
#include "..\Shared\ProductNames.h"

namespace iz3d
{

namespace resources
{

bool GetAllUsersDirectory(TCHAR path[MAX_PATH])
{
	bool ret = true;
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) 
		PathAppend(path, _T(PRODUCT_NAME) _T("\\") );
	else
		ret = false;
	return ret;
}

bool GetLanguageFilesPath(TCHAR path[MAX_PATH])
{
	if(GetAllUsersDirectory(path)) 
	{
		PathAppend(path, _T("Language") _T("\\") );
		return true;
	}
	return false;
}

bool GetCurrentUserDirectory(TCHAR path[MAX_PATH])
{
	bool ret = true;
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path))) 
		PathAppend(path, _T(PRODUCT_NAME) _T("\\") );
	else
		ret = false;
	return ret;
}

}
}