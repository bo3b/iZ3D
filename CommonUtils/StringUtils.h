/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#ifndef __CC_GUILIB_STRING_UTILS_H__
#define __CC_GUILIB_STRING_UTILS_H__

#include <string>
#include <locale>
#include <cwchar>
#include <boost/shared_ptr.hpp>
namespace common {
	namespace utils {

		typedef boost::shared_ptr<std::wstring> wstring_ptr;

		template<size_t sizeOfUnicode>
		void UTF8ToUCS2(CONST CHAR* utf8, WCHAR (&unicode)[sizeOfUnicode])
		{
			MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, _countof(unicode));
		}

		template<size_t sizeOfUnicode>
		void UCS2ToUTF8(CONST WCHAR* unicode, CHAR (&utf8)[sizeOfUnicode])
		{
			WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, _countof(utf8), NULL, NULL);
		}

		std::string to_multibyte(const std::wstring& wide, const std::locale& loc = std::locale(""));
		std::wstring to_unicode(const std::string& narrow, const std::locale& loc = std::locale(""));
		std::wstring to_unicode_simple(const std::string& narrow);
		std::string to_utf8(const std::wstring& wide);
		std::wstring from_utf8(const std::string& narrow);
		bool to_bool(const boost::shared_ptr<std::wstring> value);
		int to_int(const boost::shared_ptr<std::wstring> value);

		class wruntime_error
			: public std::runtime_error
		{
		public:
			// constructors:
			wruntime_error( const std::wstring& errorMsg );

			// copy/assignment:
			wruntime_error( const wruntime_error& rhs );
			wruntime_error&     operator=( const wruntime_error& rhs );
			// destructor:

			virtual             ~wruntime_error();

			// exception methods:
			const std::wstring& wruntime_error::errorMsg() const { return mErrorMsg ; }

		private:
			// data members:
			std::wstring        mErrorMsg ; ///< Exception error message.
		};
	}
}

#ifdef _UNICODE
#define truntime_error common::utils::wruntime_error
#else 
#define truntime_error std::runtime_error
#endif // _UNICODE

#endif
