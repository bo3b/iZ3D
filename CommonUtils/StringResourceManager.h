#pragma once

#include <map>
#include <string>
#include "Singleton.h"

namespace iz3d
{
	namespace resources
	{
		class StringResourceManager;
		class LanguageStringResourceManager;
		
		typedef StringResourceManager TStringResourceSingleton;
		class StringResourceManager : public iz3d::Singleton<StringResourceManager>
		{
			friend class iz3d::Singleton<StringResourceManager>;
		public:
			typedef std::map< std::wstring, iz3d::resources::LanguageStringResourceManager* > TManagers;
			void setLanguage( const wchar_t* languageStr );
			const wchar_t* getString( const wchar_t*  );
			const wchar_t* getString( const std::wstring& stringResourceName )
			{	return getString(stringResourceName.c_str());	}
			bool addLanguage( const wchar_t* languageStr );
			const std::wstring getAllLettersString() const;
			
		private:
			StringResourceManager();
			~StringResourceManager();
			TManagers* pManagers_;
			const wchar_t* folderPath;
			iz3d::resources::LanguageStringResourceManager* pCurManager_;
			iz3d::resources::LanguageStringResourceManager* pDefManager_;
		};
	}
}