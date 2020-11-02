#include "StdAfx.h"
#include "StringResourceManager.h"
#include "LanguageStringResourceManager.h"
#include <algorithm>

#pragma warning (disable : 4530)
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#pragma warning (default : 4530)

#include <ShlObj.h>
#include <Shlwapi.h>
using namespace std;
using namespace iz3d::resources;
using namespace boost::filesystem;

namespace internalspace
{
bool GetLanguageFilesPath(TCHAR path[MAX_PATH])
{
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) 
	{
		PathAppend(path, L"iZ3D Driver" );
		PathAppend(path, L"Language" );
		return true;
	}
	return false;
}
}

iz3d::resources::StringResourceManager::StringResourceManager():
pManagers_( DNew TManagers() )
{
	TCHAR strPath[MAX_PATH];
	internalspace::GetLanguageFilesPath( strPath );
	folderPath = strPath;
	setLanguage( L"English" );
	pDefManager_ = pCurManager_;
}

iz3d::resources::StringResourceManager::~StringResourceManager()
{
	for ( TManagers::iterator i = pManagers_->begin(); i != pManagers_->end(); ++i )
	{
		delete i->second;
		i->second = 0;
	}
	pManagers_->clear();
	delete pManagers_;
	pManagers_ = 0;
}

const wchar_t* iz3d::resources::StringResourceManager::getString( const wchar_t* stringResourceName )
{
	const wchar_t* retStr = pCurManager_->getString( stringResourceName );
	if( !retStr )
		retStr = pDefManager_->getString( stringResourceName );
	_ASSERT_EXPR(retStr, _T("Localization message missing"));
	return retStr;
}

iz3d::resources::LanguageStringResourceManager*
initializeManager( const wchar_t* fileName )
{
	return DNew LanguageStringResourceManager( fileName );
}

void iz3d::resources::StringResourceManager::setLanguage( const wchar_t* languageStr )
{
	if(!languageStr)
		MessageBox(0, L"WTF?", L"wtf?", MB_OK );
	wstring lang = languageStr;
	lang = boost::to_upper_copy( lang );
	TManagers::iterator f = pManagers_->find( lang );
	if( f != pManagers_->end() )
	{
		pCurManager_ = f->second;
		return;
	}

	wchar_t myPath[MAX_PATH];
	internalspace::GetLanguageFilesPath( myPath );
	wstring fileName = myPath;
	fileName += L"\\";
	fileName += languageStr;
	fileName += L".xml";
	iz3d::resources::LanguageStringResourceManager* pLSTR =
		initializeManager( fileName.c_str() );
	std::pair<TManagers::iterator, bool> p = 
		pManagers_->insert( TManagers::value_type( lang, pLSTR ) );
	pCurManager_ = p.first->second;
}

const std::wstring iz3d::resources::StringResourceManager::getAllLettersString() const
{
	return pCurManager_->getAllLettersString();
}
