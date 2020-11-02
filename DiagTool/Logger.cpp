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
/************************************************************************/
/* Project  -> Diagnostic Tool                                          */
/* Authors  -> Andrew Kolpakov aka x0ras                                */
/* E-Mail   -> x0ras@neurok.ru                                          */
/************************************************************************/
/* Copyright (c) 2007 iZ3D Inc.                                          */
/************************************************************************/

#include "Error.h"
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include "..\CommonUtils\System.h"
#include "..\CommonUtils\CommonResourceFolders.h"


using namespace std;

Logger::Logger()
{
}

Logger::~Logger()
{
	close();
}

void Logger::open()
{
	TCHAR szPath[MAX_PATH];
	GetFileName(szPath);
	file_.open(szPath, ios_base::out | ios_base::trunc);
}

void Logger::close()
{
	if (file_.is_open())
	{
		file_.close();
		TCHAR szPath[MAX_PATH];
		GetFileName(szPath);
		SetUserSecurityLevel(szPath);
	}
}

void Logger::GetFileName(TCHAR szPath[MAX_PATH])
{
	iz3d::resources::GetAllUsersDirectory( szPath );
	PathAppend(szPath, TEXT("Report.txt"));
}

Logger& Logger::operator <<(const std::string& str)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	file_ << str;

	return (*this);
}

Logger& Logger::operator <<(const std::wstring& str)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	string sAnsi( str.begin(), str.end());

	file_ << sAnsi;

	return (*this);
}

Logger& Logger::operator <<(const dword &val)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	file_ << val;

	return (*this);
}

Logger& Logger::operator <<(const qword &val)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	file_ << val;

	return (*this);
}

Logger& Logger::operator <<(const int &val)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	file_ << val;

	return (*this);
}

Logger& Logger::operator <<(const float &val)
{
	if (!file_.is_open())
		throw SystemException("Trying to write into unopened file.");

	file_ << std::setprecision(4) << val;

	return (*this);
}

bool Logger::Fail()
{
	return file_.fail();
}

void Logger::Flush()
{
	file_.flush();
}
