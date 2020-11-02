#pragma once

#include <map>
#include <string>

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include "tchar.h"
namespace iz3d
{
	namespace resources
	{
		class LanguageStringResourceManager
		{
			struct ltstr
			{
				bool operator() ( const std::wstring s1, const std::wstring s2 ) const
				{
					return _tcscmp( s1.c_str(), s2.c_str() ) < 0;
				}
			};
		public:
			typedef std::map< std::wstring, std::wstring, ltstr > TDictionary;

			LanguageStringResourceManager( const wchar_t* languageFileName );
			~LanguageStringResourceManager();
			const wchar_t* getString( const wchar_t* resName );
			const std::wstring getAllLettersString() const;
		private:
			void FillDictionary( xercesc::DOMElement* pRootElement );
			void ProcessSubnode( const wchar_t* name, xercesc::DOMNode* pNode );
			const wchar_t* languageFileName_;
			TDictionary* pDictionary_;
		};
	}
}