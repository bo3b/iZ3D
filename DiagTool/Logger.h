/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#ifndef __DIAGTOOL_LOGGER_H__
#define __DIAGTOOL_LOGGER_H__

#include <windows.h>
#include <string>
#include <fstream>

#include "Types.h"
#include "..\CommonUtils\Singleton.h"
#include <Shlobj.h>
#include <Shlwapi.h>

class Logger : public iz3d::Singleton<Logger>
{
public:
	void open();
	void close();
	bool Fail();
	void Flush();
	void GetFileName(TCHAR szPath[MAX_PATH]);

	Logger& operator << (const std::string& str);
	Logger& operator << (const std::wstring& str);
	Logger& operator << (const dword &val);
	Logger& operator << (const int &val);
	Logger& operator << (const qword &val);
	Logger& operator << (const float &val);

private:
	friend class iz3d::Singleton<Logger>;
	Logger();
	~Logger();
	std::ofstream file_;
};



#endif
