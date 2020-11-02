#define _WIN32_WINNT 0x0502 // to enable SetDllDirectory/GetDllDirectory functions
#include "stdafx.h"

#include <Shlwapi.h>
#include <stdlib.h>
#include "tinyxml.h"
#include "S3DInjector.h"
#include "ExplicitLibraryLoad.h"
#include "../CommonUtils/CommonResourceFolders.h"
#include "../CommonUtils/StringUtils.h"

//////////////////////////////////////////////////////////////////////////
void GetCurrentModuleFileName( TCHAR* pRetFileName, DWORD size ){
	TCHAR fileExtension[MAX_PATH];
	TCHAR fileName[MAX_PATH];
	GetModuleFileName( 0, pRetFileName, size );
	_tsplitpath_s( pRetFileName,
		0, 0,	// drive
		0, 0,	// dir
		fileName, MAX_PATH,	// filename w/o extension
		fileExtension, MAX_PATH );	// file extension
	_tmakepath_s( pRetFileName, size, 0, 0, fileName, fileExtension );
}

//////////////////////////////////////////////////////////////////////////
void ExplicitLibaries::GetLibrariesToLoad( const TCHAR* fileName, std::vector< std::basic_string< TCHAR > >& libNames )
{
	libNames.clear();
	TCHAR BaseProfilePath[MAX_PATH];
	iz3d::resources::GetAllUsersDirectory( BaseProfilePath );
	TCHAR* commandLine = GetCommandLine();
	PathAppend(BaseProfilePath, TEXT("BaseProfile.xml") );
	TiXmlDocument Doc( common::utils::to_multibyte( BaseProfilePath ) );
	if(!Doc.LoadFile())
		return;
	TiXmlNode* node = Doc.FirstChild("Profiles");
	for( node = node->FirstChild( "Profile" ); node; node = node->NextSibling( "Profile" ) ){
		for( TiXmlElement* pFileElement = node->FirstChildElement("File"); pFileElement;
			pFileElement = pFileElement->NextSiblingElement("File") ){
				const char* attrFileName = pFileElement->Attribute("Name");
				if( !attrFileName )
					continue;
				std::basic_string<TCHAR> Name = common::utils::from_utf8( attrFileName );		
				std::transform(Name.begin(), Name.end(), Name.begin(), tolower);
				if( _tcsicmp( fileName, Name.c_str() ) != 0 )
					continue;
				std::basic_string<TCHAR> CmdLine;
				const char* attrCmdLine = pFileElement->Attribute("CmdLine");
				if( attrCmdLine )
					CmdLine = common::utils::from_utf8( attrCmdLine );
				if( !CmdLine.empty() && (_tcsicmp( CmdLine.c_str(), commandLine ) != 0) )
					continue;
				for( TiXmlElement* pExpLib = pFileElement->NextSiblingElement( "ExplLib"); pExpLib;
					pExpLib= pExpLib->NextSiblingElement() ){
					const char* attrLibName = pExpLib->Attribute("Name");
					if( attrLibName )
						libNames.push_back( common::utils::from_utf8( attrLibName ) );
				}
				break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
ExplicitLibaries::ExplicitLibaries( HMODULE hModule ):
m_hDllModule( hModule )
{
	TCHAR fileName[MAX_PATH];
	GetCurrentModuleFileName( fileName, MAX_PATH );
	std::vector< std::basic_string< TCHAR > > libNames;
	GetLibrariesToLoad( fileName, libNames );
#if 0
	GetCurrentDirectory( MAX_PATH, bufDirectory );
	TCHAR fakePath[MAX_PATH];
	TCHAR dirName[MAX_PATH];
	TCHAR folderPath[MAX_PATH];
	GetModuleFileName( m_hDllModule, fakePath, MAX_PATH );
	_tsplitpath_s( fakePath,
		dirName, MAX_PATH,		// drive
		folderPath, MAX_PATH,	// dir
		0, 0,					// file name
		0, 0 );					// file extension
	_tmakepath_s( fakePath, MAX_PATH, dirName, folderPath, 0, 0 );
	
	if( libNames.size() ){
		SetCurrentDirectory( L"d:/izBuf" );
	}
#endif
	for( auto i = libNames.begin(); i != libNames.end(); ++i ){
		HMODULE hMod = LoadLibrary( i->c_str() );
		m_modules.push_back(hMod);
	}
}

//////////////////////////////////////////////////////////////////////////
ExplicitLibaries::~ExplicitLibaries()
{
	for( auto i = m_modules.begin(); i != m_modules.end(); ++ i ){
		FreeLibrary( *i );
	}
#if 0
	if( m_modules.size() )
		SetCurrentDirectory( bufDirectory );
#endif
}

//////////////////////////////////////////////////////////////////////////
bool ReadProfile(IngectorSettings* iSettings)
{
	TCHAR fileName[MAX_PATH];
	GetCurrentModuleFileName( fileName, MAX_PATH );
	TCHAR BaseProfilePath[MAX_PATH];
	iz3d::resources::GetAllUsersDirectory( BaseProfilePath );
	TCHAR* commandLine = GetCommandLine();
	PathAppend(BaseProfilePath, TEXT("BaseProfile.xml") );
	TiXmlDocument Doc( common::utils::to_multibyte( BaseProfilePath ) );
	if(!Doc.LoadFile())
		return false;
	TiXmlNode* node = Doc.FirstChild("Profiles");
	for( node = node->FirstChild( "Profile" ); node; node = node->NextSibling( "Profile" ) ){
		for( TiXmlElement* pFileElement = node->FirstChildElement("File"); pFileElement;
			pFileElement = pFileElement->NextSiblingElement("File") ){
				const char* attrFileName = pFileElement->Attribute("Name");
				if( !attrFileName )
					continue;
				std::basic_string<TCHAR> Name = common::utils::from_utf8( attrFileName );		
				std::transform(Name.begin(), Name.end(), Name.begin(), tolower);
				if( _tcsicmp( fileName, Name.c_str() ) != 0 )
					continue;
				std::basic_string<TCHAR> CmdLine;
				const char* attrCmdLine = pFileElement->Attribute("CmdLine");
				if( attrCmdLine )
					CmdLine = common::utils::from_utf8( attrCmdLine );
				if( CmdLine.empty() || _tcsicmp( CmdLine.c_str(), commandLine ) == 0 )
				{
					TiXmlElement* pElement = pFileElement->NextSiblingElement("RouterType");
					if( pElement )
						pElement->QueryIntAttribute( "Value", (int*)&iSettings->m_RouterType );
					pElement = pFileElement->NextSiblingElement("RenewHookMode");
					if( pElement )
						pElement->QueryIntAttribute( "Value", (int*)&iSettings->m_RenewHookMode );
					return true;
				}
		}
	}
	return true;
}
