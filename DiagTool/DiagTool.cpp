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

#include "Windows.h"

#include "Info.h"

#include <fstream>
#include "Logger.h"
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

Info info;

try
{
info.WriteSystemInformation();
}
catch (const std::exception& e)
{
MessageBox(0, L"An error occurred while getting system information", 0, 0);

std::fstream file("error.log", std::ios::out|std::ios::trunc);
if (file.is_open())
{
file << "Error: " << e.what() << std::endl;
file.flush();
file.close();
}

return 1;
}
catch (...)
{
MessageBox(0, L"An unspecified error occurred while getting system information", 0, 0);

return 2;
}

MessageBox(0, L"Operation successful", L"", 0);

return 0;
}*/

int WINAPI DllMain(HMODULE _HDllHandle,unsigned _Reason,void * _Reserved)
{
	switch (_Reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(_HDllHandle);
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(82291);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		_RPT0(0, "DiagTool memory leaks:\n");
		break;
	}
	return TRUE;
}


__declspec(dllexport) bool CALLBACK CreateDXDiagLog()
{
	Info info;

    _TRY_BEGIN
		Logger::Get().open();
		if (Logger::Get().Fail())
			return false;
		info.WriteSystemInformation();
        info.WriteDisplayInformation();
	_CATCH_ALL
		Logger::Get().close();
		return false;
	_CATCH_END

	Logger::Get().close();
	return true;
}

int main( int argc , char *argv[] , char *envp[])
{
	CreateDXDiagLog();
	return 0;
}
