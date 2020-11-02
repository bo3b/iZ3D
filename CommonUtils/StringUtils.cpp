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

#include "stdafx.h"
#include "StringUtils.h"

namespace std{
	using ::type_info;
}
#include <boost/lexical_cast.hpp>
#include <boost/tr1/memory.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

using namespace boost::filesystem::detail;

namespace common {
	namespace utils {

		std::string to_multibyte(const std::wstring& wide, const std::locale& loc)
		{
			if (wide.empty())
				return std::string();

			typedef std::wstring::traits_type::state_type state_type;
			typedef std::codecvt<wchar_t, char, state_type> CVT;
			const CVT& cvt = std::use_facet<CVT>(loc);

			std::string narrow(cvt.max_length()*wide.size(), '\0');
			state_type state = state_type();

			const wchar_t* from_beg = &wide[0];
			const wchar_t* from_end = from_beg + wide.size();
			const wchar_t* from_nxt;
			char* to_beg = &narrow[0];
			char* to_end = to_beg + narrow.size();
			char* to_nxt;

			std::string::size_type sz = 0;
			std::codecvt_base::result r;
			do
			{
				r = cvt.out(state, from_beg, from_end, from_nxt,
					to_beg,   to_end,   to_nxt);
				switch (r)
				{
				case std::codecvt_base::error:
					throw std::runtime_error("error converting wstring to string");

				case std::codecvt_base::partial:
					sz += to_nxt - to_beg;
					narrow.resize(2*narrow.size());
					to_beg = &narrow[sz];
					to_end = &narrow[0] + narrow.size();
					break;

				case std::codecvt_base::noconv:
					narrow.resize(sz + (from_end-from_beg)*sizeof(wchar_t));
					std::memcpy(&narrow[sz], from_beg,(from_end-from_beg)*sizeof(wchar_t));
					r = std::codecvt_base::ok;
					break;

				case std::codecvt_base::ok:
					sz += to_nxt - to_beg;
					narrow.resize(sz);
					break;
				}
			} while (r != std::codecvt_base::ok);

			return narrow;
		}

		std::wstring to_unicode(const std::string& narrow, const std::locale& loc)
		{
			if (narrow.empty())
				return std::wstring();

			typedef std::string::traits_type::state_type state_type;
			typedef std::codecvt<wchar_t, char, state_type> CVT;
			const CVT& cvt = std::use_facet<CVT>(loc);
			std::wstring wide(narrow.size(), '\0');
			state_type state = state_type();
			const char* from_beg = &narrow[0];
			const char* from_end = from_beg + narrow.size();
			const char* from_nxt;
			wchar_t* to_beg = &wide[0];
			wchar_t* to_end = to_beg + wide.size();
			wchar_t* to_nxt;
			std::wstring::size_type sz = 0;
			std::codecvt_base::result r;
			do
			{
				r = cvt.in(state, from_beg, from_end, from_nxt,
					to_beg,   to_end,   to_nxt);
				switch (r)
				{
				case std::codecvt_base::error:
					throw std::runtime_error("error converting string to wstring");
				case std::codecvt_base::partial:
					sz += to_nxt - to_beg;
					wide.resize(2*wide.size());
					to_beg = &wide[sz];
					to_end = &wide[0] + wide.size();
					break;
				case std::codecvt_base::noconv:
					wide.resize(sz + (from_end-from_beg));
					std::memcpy(&wide[sz], from_beg, (std::size_t)(from_end-from_beg));
					r = std::codecvt_base::ok;
					break;
				case std::codecvt_base::ok:
					sz += to_nxt - to_beg;
					wide.resize(sz);
					break;
				}
			} while (r != std::codecvt_base::ok);

			return wide;
		}

		std::string to_utf8(const std::wstring& wide)
		{
			std::locale old_locale;
			std::locale utf8_locale( old_locale, new utf8_codecvt_facet());

			return to_multibyte(wide, utf8_locale);
		}

		std::wstring from_utf8(const std::string& narrow)
		{
			std::locale old_locale;
			std::locale utf8_locale(old_locale, new utf8_codecvt_facet());

			return to_unicode(narrow, utf8_locale);
		}

		std::wstring to_unicode_simple(const std::string& narrow)
		{
			std::wstring wide( narrow.begin(), narrow.end() );
			//std::wstring wide(narrow.length(), L' '); // Make room for characters
			// Copy string to wstring.
			//std::copy(narrow.begin(), narrow.end(), wide.begin());
			return wide;
		}

		bool to_bool(const boost::shared_ptr<std::wstring> value)
		{
			if ((*value) == L"false")
				return false;
			else
				return true;
		}

		int to_int(const boost::shared_ptr<std::wstring> value)
		{
			int val;
			std::wstringstream stream(*value.get());
			stream >> val;

			return val;
		}

		std::string toNarrowString( const wchar_t* pStr , size_t len )
		{
			// figure out how many narrow characters we are going to get 

			int nChars = WideCharToMultiByte( CP_ACP, 0, 
				pStr, (int)len, NULL, 0, NULL, NULL ); 
			if ( len == -1 )
				-- nChars; 
			if ( nChars == 0 )
				return "";

			// convert the wide string to a narrow string

			// nb: slightly naughty to write directly into the string like this

			std::string buf;
			buf.resize( nChars );
			WideCharToMultiByte( CP_ACP, 0, pStr, (int)len, 
				const_cast<char*>(buf.c_str()), nChars, NULL, NULL ) ; 

			return buf ; 
		}

		inline std::string toNarrowString( const std::wstring& str )
		{
			return toNarrowString(str.c_str(), str.length());
		}

		wruntime_error::wruntime_error( const std::wstring& errorMsg )
			: runtime_error( toNarrowString(errorMsg) )
			, mErrorMsg(errorMsg)
		{

		}

		wruntime_error::wruntime_error( const wruntime_error& rhs )
			: runtime_error( toNarrowString(rhs.errorMsg()) )
			, mErrorMsg(rhs.errorMsg())
		{

		}

		wruntime_error& wruntime_error::operator=( const wruntime_error& rhs )
		{
			// copy the wruntime_error
			runtime_error::operator=( rhs ) ; 
			mErrorMsg = rhs.mErrorMsg ; 

			return *this ;
		}

		wruntime_error::~wruntime_error()
		{

		}
	}
}
