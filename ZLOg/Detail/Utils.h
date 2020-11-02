#pragma once
#include "../API.h"

#ifdef ZLOG_ALLOW_TRACING

namespace zlog {
namespace detail {

template<typename Char> 
inline const Char* switch_literal(const char* str, const wchar_t* wstr);

template<> inline const char*    switch_literal<char>(const char* str, const wchar_t* wstr)    { return str; }
template<> inline const wchar_t* switch_literal<wchar_t>(const char* str, const wchar_t* wstr) { return wstr; }

template<typename Char> inline Char switch_literal(char str, wchar_t wstr);

template<> inline char    switch_literal<char>(char str, wchar_t wstr)    { return str; }
template<> inline wchar_t switch_literal<wchar_t>(char str, wchar_t wstr) { return wstr; }

/** Printf to debug buffers. Thread-safe, but result of the operation can be used in no more than than 128 scopes. */
ZLOG_API const char* dbg_sprintf(const char* format, ...);

/** Printf to debug buffers. Thread-safe, but result of the operation can be used in no more than than 128 scopes. */
ZLOG_API const wchar_t* dbg_sprintf(const wchar_t* format, ...);

} // namespace detail
} // namespace zlog

#define ZLOG_DBG_SPRINTF(...) zlog::detail::dbg_sprintf(__VA_ARGS__)

#define AUTO_LITERAL(VALUE) zlog::detail::switch_literal<Char>(VALUE, L##VALUE)
#define UNPACK_VALUE(VALUE) case VALUE: return AUTO_LITERAL(#VALUE);

#define MERGE_VALUE(Value, VALUE) \
if (Value & VALUE)\
{\
	if ( str.empty() ) {\
		str = std::basic_string<Char>( AUTO_LITERAL(#VALUE) );\
	}\
	else {\
		str += std::basic_string<Char>( AUTO_LITERAL("|") ) + AUTO_LITERAL(#VALUE);\
	}\
}

#define UNPACK_MERGE_VALUE(VALUE) \
case VALUE:\
{\
	if ( str.empty() ) {\
		str = std::basic_string<Char>( AUTO_LITERAL(#VALUE) );\
	}\
	else {\
		str += std::basic_string<Char>( AUTO_LITERAL("|") ) + AUTO_LITERAL(#VALUE);\
	}\
}

#define UNPACK_VALUE_C(VALUE) case VALUE: return #VALUE;

#define MERGE_VALUE_C(Value, VALUE) \
if (Value & VALUE)\
{\
	if ( str.empty() ) {\
		str = std::string(#VALUE);\
	}\
	else {\
		str += std::string("|") + #VALUE;\
	}\
}

#define UNPACK_MERGE_VALUE_C(VALUE) \
case VALUE:\
{\
	if ( str.empty() ) {\
		str = std::string(#VALUE);\
	}\
	else {\
		str += std::string("|") + #VALUE;\
	}\
}

#define UNPACK_VALUE_T(VALUE) case VALUE: return _T(#VALUE);

#define MERGE_VALUE_T(Value, VALUE) \
if (Value & VALUE)\
{\
	if ( str.empty() ) {\
		str = std::basic_string<TCHAR>( _T(#VALUE) );\
	}\
	else {\
		str += std::basic_string<TCHAR>( _T("|") ) + _T(#VALUE);\
	}\
}

#define UNPACK_MERGE_VALUE_T(VALUE) \
case VALUE:\
{\
	if ( str.empty() ) {\
		str = std::basic_string<TCHAR>( _T(#VALUE) );\
	}\
	else {\
		str += std::basic_string<TCHAR>( _T("|") ) + _T(#VALUE);\
	}\
}

#else // !ZLOG_ALLOW_TRACING

#define ZLOG_DBG_SPRINTF(...)		__noop
#define AUTO_LITERAL(VALUE)			0

#define UNPACK_VALUE(VALUE)			case VALUE: return 0;
#define MERGE_VALUE(Value, VALUE)	__noop
#define UNPACK_MERGE_VALUE(VALUE)	case VALUE: __noop;

#define UNPACK_VALUE_C(VALUE)		case VALUE: return 0;
#define MERGE_VALUE_C(Value, VALUE)	__noop
#define UNPACK_MERGE_VALUE_C(VALUE)	case VALUE: __noop;

#define UNPACK_VALUE_T(VALUE)		case VALUE: return 0;
#define MERGE_VALUE_T(Value, VALUE)	__noop
#define UNPACK_MERGE_VALUE_T(VALUE)	case VALUE: __noop;

#endif // ZLOG_ALLOW_TRACING