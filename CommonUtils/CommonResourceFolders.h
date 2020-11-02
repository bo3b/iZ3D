#pragma once

namespace iz3d
{
	namespace resources
	{
		bool GetAllUsersDirectory(TCHAR path[MAX_PATH]);
		bool GetCurrentUserDirectory(TCHAR path[MAX_PATH]);
		bool GetLanguageFilesPath(TCHAR path[MAX_PATH]);
	}
}