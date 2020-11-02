// DX10GenerateCodeFromDump.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Streamer\CmdFlowStreamers.h"
#include "shellapi.h"
#include "Streamer\CodeGenerator.h"
#include "EnumToString.h"
#include "..\CommonUtils\StringUtils.h"

using namespace std;
//////////////////////////////////////////////////////////////////////////

#define SRC_DIRNAME				L"./src"
#define SRC_RENDER_DIRNAME		SRC_DIRNAME//L"./src/render"
#define SRC_GLOBAL_DIRNAME		SRC_DIRNAME//L"./src/global"

#define SRC_FILENAME			"./src/%s"
#define SRC_RENDER_FILENAME		SRC_FILENAME//"./src/render/%s"
#define SRC_GLOBAL_FILENAME		SRC_FILENAME//"./src/global/%s"


/************************************************************************/
/* SearchFile                                                           */
/************************************************************************/
bool SearchFile( TCHAR* szDir_, TCHAR* szFileName_ )
{
	return ( SearchPath( szDir_, szFileName_, NULL, 0, NULL, NULL ) ) ? true : false;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CreateDir( TCHAR* szDirName_ )
{
	return ( CreateDirectory( szDirName_, NULL ) )? true : false;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const char*	GetFullFileName( const char* szDirMask_, const char* szFileName_ )
{
	static char szBuff[ MAX_PATH ];
	sprintf( szBuff, szDirMask_, szFileName_ );
	return szBuff;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool ExtractFileFromResource( const char* szOutputFile_, UINT nRes_ )
{	
	HMODULE  hModule = GetModuleHandle( NULL );	
	HRSRC hRes = FindResource( hModule, MAKEINTRESOURCE( nRes_ ), _T("FILE") );
	if( !hRes )
	{
		_ASSERT( true );
		return false;
	}
	DWORD dwSize = SizeofResource( hModule, hRes );
	if ( !dwSize )
	{
		_ASSERT( true );
		return false;
	}

	HGLOBAL hData = LoadResource( hModule, hRes );
	if ( !hData )
	{
		_ASSERT( true );
		return false;
	}

	LPVOID pData = LockResource( hData );
	if ( !pData )
	{
		_ASSERT( true );
		return false;
	}

	FILE* pFile = fopen( szOutputFile_, "wb" );
	if (!pFile )
		return false;

	fwrite( pData, dwSize, 1, pFile );
	fclose( pFile );
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*                                                                      */
/************************************************************************/
int _tmain(int argc_, _TCHAR* argv_[])
{
	cout << "Code-from-dump Generator" << endl;

	TCHAR* szLogDir = L".";//\\logs_dx10\\BasicHLSL10\\";
	
	if ( !SearchFile( szLogDir, L"StereoCommandFlow.xml" ) && !SearchFile( szLogDir, L"CommandFlow.xml" ) )
	{
		cerr << "not found [Stereo]CommandFlow.xml" << endl;
		return -1;
	}

	/*if ( !SearchFile( szLogDir, L"ResourceHistory.xml" ) )
	{
		cerr << "not found ResourceHistory.xml" << endl;
		return 0;
	}*/

	if ( !SetCurrentDirectory( szLogDir ) )
	{
		cerr << "not found directory:" << szLogDir << endl;
		return -2;
	}

	CodeGenerator codeGen;	
	cout << "parsing begin.." << endl;
	
	if (!ReadStreamer::Get().OpenFile(L"./StereoCommandFlow.xml","iZ3DCommandFlow") &&
		!ReadStreamer::Get().OpenFile(L"./CommandFlow.xml","iZ3DCommandFlow"))
	{
		cerr << "failed open file.." << endl;
		return -3;
	}

	cout << "[Stereo]CommandFlow.xml loaded..." << endl;

	std::wstring gameName;
	if (!ReadStreamer::Get().m_appName.empty())
	{
		std::string s = ReadStreamer::Get().m_appName;
		size_t pos = s.find_last_of(".");
		gameName = common::utils::to_unicode_simple(s.substr(0,pos));
	}
	else
	{
		TCHAR dir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH,dir);
		TCHAR *s = dir + wcslen(dir);
		for(;;)
		{
			s--;
			if (*s == L'\\')
			{
				s++;
				break;
			}

			if (s <= dir) __debugbreak();
		}
		gameName = s;
	}
	printf("Game executable name: %S\n",gameName.c_str());

#ifndef FINAL_RELEASE
	Commands::Command::InitCommandsCreateFunc();
#else
	__debugbreak();
#endif
	Commands::Command* pCmd = NULL;

	ResourceManager	resourceManager;
	ReadStreamer::Get().SetCurrentRM(&resourceManager);

	std::string sError;
	do
	{
		pCmd = ReadStreamer::Get().ReadNextCommand( sError );			
		if ( pCmd )
		{
			/*if (pCmd->EventID_ == 15400)
			{
				printf("STOP!");
			}*/

			//cout << '\x0d' << pCmd->EventID_ << ": " << pCmd->getCmdName();
			printf("\x0d%7d: %-70s",pCmd->EventID_,CommandIDToString(pCmd->CommandId));

			pCmd->SendToCodeGenerator( &codeGen );
			//delete pCmd;
		}
	}
	while( pCmd != NULL );
	cout << endl;

	if ( sError != "" )
	{
		cerr << sError << endl;
		return -4;
	}
	
	cout << "ok" << endl;	

	if ((argc_ >= 2 && _tcscmp(argv_[1],_T("-noseteid")) == 0) ||
		(argc_ >= 3 && _tcscmp(argv_[2],_T("-noseteid")) == 0))
	{
		codeGen.DontNeedSetEidCommands();
	}

	CreateDir( SRC_DIRNAME );
	CreateDir( SRC_RENDER_DIRNAME );
	CreateDir( SRC_GLOBAL_DIRNAME );
	ExtractFileFromResource( GetFullFileName( SRC_FILENAME,			"main.cpp"			),	IDR_MAIN_CPP );
	ExtractFileFromResource( GetFullFileName( SRC_GLOBAL_FILENAME,	"global.cpp"		),	IDR_GLOBAL_CPP );
	ExtractFileFromResource( GetFullFileName( SRC_GLOBAL_FILENAME,	"global.h"			),	IDR_GLOBAL_H );
	//ExtractFileFromResource( GetFullFileName( SRC_GLOBAL_FILENAME,	"pcmp.cpp"			),	IDR_PCMP_CPP );
	ExtractFileFromResource( GetFullFileName( SRC_GLOBAL_FILENAME,	"pcmp.h"			),	IDR_PCMP_H );
	ExtractFileFromResource( GetFullFileName( SRC_RENDER_FILENAME,	"render.cpp"		),	IDR_RENDER_CPP );
	ExtractFileFromResource( GetFullFileName( SRC_RENDER_FILENAME,	"render.h"			),	IDR_RENDER_H );
	ExtractFileFromResource( GetFullFileName( SRC_FILENAME,			"build_project.cmd" ),	IDR_BUILD_PROJECT_CMD );	
	codeGen.Save( GetFullFileName( SRC_RENDER_FILENAME, "frame" ) );
	ExtractFileFromResource( GetFullFileName( SRC_FILENAME,			"IStereoAPI.h"			),	IDR_ISTEREOAPI_H );
	ExtractFileFromResource( GetFullFileName( SRC_FILENAME,			"IStereoAPIInternal.h"  ),	IDR_ISTEREOAPIINTERNAL_H );

	if (argc_ >= 2 && _tcscmp(argv_[1],_T("-noautobuild")) == 0)
	{
		std::wstring game_name = L".\\src\\" + gameName + L".name";
		FILE* pFile = _wfopen(game_name.c_str(), L"wb");
		if (!pFile) return -5;
		/*int n = 0;
		fwrite(&n,1,1,pFile);*/
		fclose(pFile);
	}
	else
	{
		SHELLEXECUTEINFO shellInfo;
		memset( &shellInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		shellInfo.cbSize		= sizeof( SHELLEXECUTEINFO );
		shellInfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
		shellInfo.nShow			= SW_SHOWNORMAL;
		shellInfo.lpDirectory	= L".\\src";
		shellInfo.lpFile		= L".\\build_project.cmd";
		shellInfo.lpParameters	= gameName.c_str();

		if( !ShellExecuteEx(&shellInfo) )
		{
			cerr << "internal error" << endl;
		}
	}

	ReadStreamer::Get().FreeAllocatedMem();		

	//DeleteFile(L".\\src\\build_project.cmd");
	return 0;
}

//////////////////////////////////////////////////////////////////////////
