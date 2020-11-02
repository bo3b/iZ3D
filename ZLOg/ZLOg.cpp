#define ZLOG_ALLOW_TRACING
#include "Detail/Utils.h"
#include "ZLOg.h"
#include <atlbase.h>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <io.h>
#include <stdarg.h>
#include <stdio.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// TCHAR antipod
#ifdef UNICODE
#	define NONTCHAR CHAR
#else
#	define NONTCHAR WCHAR
#endif

namespace 
{
	typedef boost::lock_guard<boost::mutex>	mutex_lock;
	
	/** Every operation with file is in critical section */
	class thread_safe_file
	{
	friend class log_message;
	public:
		thread_safe_file()
		:	hFile(INVALID_HANDLE_VALUE)
		{}
		
		~thread_safe_file()
		{
			//CloseHandle(hFile);
		}

		bool open(const char* fileName, bool append, bool writeThrough)
		{
			UINT mode = FILE_ATTRIBUTE_NORMAL;
			if (writeThrough) {
				mode |= FILE_FLAG_WRITE_THROUGH;
			}

			mutex_lock lock(mutex);
			hFile = CreateFileA(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, mode, NULL);
			return (hFile != INVALID_HANDLE_VALUE);
		}

		void close()
		{
			mutex_lock lock(mutex);
			if (hFile != INVALID_HANDLE_VALUE) 
			{
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
		}

		bool is_open() const 
		{
			mutex_lock lock(mutex);
			return (hFile != INVALID_HANDLE_VALUE); 
		}

	private:
		HANDLE					hFile;
		mutable boost::mutex	mutex;
	};
	
	const size_t MAX_MESSAGE_LENGTH = 8096;

	/** Helper for constructing log message */
	class log_message :
		public boost::noncopyable
	{
	public:
		log_message(thread_safe_file& file)
		:	lock(file.mutex)
		,	hFile(file.hFile)
		,	offset(0)
		{}

		~log_message()
		{
			if (offset > 0)
			{
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD written;
					WriteFile(hFile, buffer, offset * sizeof(TCHAR), &written, NULL);
				}
				OutputDebugString(buffer);
			}
		}

		int puttc(TCHAR c)
		{
			if (offset < MAX_MESSAGE_LENGTH)
			{
				buffer[offset++] = c;
				buffer[offset] = '\0';
				return 1;
			}

			return 0;
		}

		int printf(const TCHAR* format, ...)
		{
			va_list args;
			va_start(args, format);
			int written = this->vprintf(format, args);
			va_end(args);
			return written;
		}

		int vprintf(const TCHAR* format, va_list args)
		{
			int written = 0;
			if (offset < MAX_MESSAGE_LENGTH) 
			{
				written = _vsntprintf(buffer + offset, MAX_MESSAGE_LENGTH - offset, format, args);
				offset += written;
			}

			return written;
		}
		
		int printf(const NONTCHAR* format, ...)
		{
			va_list args;
			va_start(args, format);
			int written = this->vprintf(format, args);
			va_end(args);
			return written;
		}

		int vprintf(const NONTCHAR* format, va_list args)
		{
			if (offset < MAX_MESSAGE_LENGTH) 
			{
				// write to non TCHAR buffer
			#ifdef UNICODE
				vsnprintf(transferBuffer, MAX_MESSAGE_LENGTH, format, args);
			#else 
				_vsnwprintf(transferBuffer, MAX_MESSAGE_LENGTH, format, args);
			#endif

				// write as non TCHAR string '%S'
				return this->printf(_T("%S"), transferBuffer);
			}

			return 0;
		}

		operator bool () const { return hFile != INVALID_HANDLE_VALUE; }

	private:
		mutex_lock		lock;
		HANDLE			hFile;
		static TCHAR	buffer[MAX_MESSAGE_LENGTH];
		static NONTCHAR	transferBuffer[MAX_MESSAGE_LENGTH];
		DWORD			offset;
	};

	TCHAR				log_message::buffer[MAX_MESSAGE_LENGTH];
	NONTCHAR			log_message::transferBuffer[MAX_MESSAGE_LENGTH];

	thread_safe_file	g_File;
	zlog::string		g_LogFileName;
	zlog::SEVERITY		g_Severity;
	bool				g_WriteThrough = false;
	bool				g_Buffering = false;
	
	const wchar_t* SEVERITY_TO_WSTR[] =
	{
		L"FATAL_ERROR",
		L"ERROR",
		L"WARNING",
		L"MESSAGE",
		L"NOTICE",
		L"FLOOD"
	};

	const char* SEVERITY_TO_CSTR[] =
	{
		"FATAL_ERROR",
		"ERROR",
		"WARNING",
		"MESSAGE",
		"NOTICE",
		"FLOOD"
	};

#ifdef UNICODE
#	define SEVERITY_TO_TSTR SEVERITY_TO_WSTR
#else
#	define SEVERITY_TO_TSTR SEVERITY_TO_CSTR
#endif

	zlog::SEVERITY STR_TO_SEVERITY(const std::string& severity)
	{
		if (severity == "FLOOD") return zlog::SV_FLOOD;
		else if (severity == "NOTICE") return zlog::SV_NOTICE;
		else if (severity == "MESSAGE") return zlog::SV_MESSAGE;
		else if (severity == "WARNING") return zlog::SV_WARNING;
		else if (severity == "ERROR") return zlog::SV_ERROR;
		else if (severity == "FATAL_ERROR") return zlog::SV_FATAL_ERROR;
		
		assert(!"can't get here");
		return zlog::SEVERITY();
	}

	zlog::string w_to_a(const zlog::wstring& str)
	{
		CW2AEX<MAX_PATH> conv(str.c_str());
		return zlog::string(conv.m_psz);
	}

} // anonymous namespace

namespace zlog {
	
bool ReopenLogger(const string& fileName, bool append)
{
	if ( !g_File.open(fileName.c_str(), append, g_WriteThrough) ) {
		return false;
	}
		
	g_LogFileName = fileName;
	return true;
}

bool ReopenLogger(const wstring& fileName, bool append)
{
	return ReopenLogger( w_to_a(fileName), append );
}

bool IsLoggerOpened()
{
	return g_File.is_open();
}

void CloseLogger()
{
	g_File.close();
	g_LogFileName.clear();
}

string GetLoggerFileName()
{
	return g_LogFileName;
}

void InitializeFromINI(const string& configFile)
{
	CloseLogger();
	{ 
		boost::property_tree::ptree properties;
		boost::property_tree::read_ini(configFile, properties);

		g_LogFileName	= properties.get("Output", g_LogFileName);
		g_Severity		= STR_TO_SEVERITY( properties.get("Severity", SEVERITY_TO_CSTR[g_Severity]) );
		g_WriteThrough	= properties.get("WriteThrough", g_WriteThrough);
	}
	
	if ( !g_LogFileName.empty() ) {
		ReopenLogger(g_LogFileName, true);
	}
}

void InitializeFromINI(const wstring& configFile)
{
	InitializeFromINI( w_to_a(configFile) );
}

void InitializeFromXML(const string& configFile, const string& section)
{
	CloseLogger();
	{
		boost::property_tree::ptree properties;
		boost::property_tree::read_xml(configFile, properties);

		string prefix;
		if ( !section.empty() ) {
			prefix = section + ".";
		}
		
		g_LogFileName	= properties.get(prefix + "Output", g_LogFileName);
		g_Severity		= STR_TO_SEVERITY( properties.get(prefix + "Severity", SEVERITY_TO_CSTR[g_Severity]) );
		g_WriteThrough	= properties.get(prefix + "WriteThrough", g_WriteThrough);
	}
	
	if ( !g_LogFileName.empty() ) {
		ReopenLogger(g_LogFileName, true);
	}
}

void InitializeFromXML(const wstring& configFile, const wstring& section)
{
	InitializeFromXML( w_to_a(configFile), w_to_a(section) );
}

void ToggleWriteThrough(bool toggle)
{
	if (g_File.is_open() && g_WriteThrough != toggle) 
	{
		g_WriteThrough = toggle;
		ReopenLogger(g_LogFileName, true);
	}
	else {
		g_WriteThrough = toggle;
	}
}

void ToggleBuffering(bool toggle)
{
	g_Buffering = toggle;
}

void SetupSeverity(SEVERITY severity)
{
	g_Severity = severity;
}

SEVERITY GetSeverity()
{
	return g_Severity;
}

void GetSeverityPtr(SEVERITY** ppSeverity)
{
	if (ppSeverity == NULL)
		return;
	*ppSeverity = &g_Severity;
}

void LogMessage(SEVERITY severity, const char* module, bool addEndl, bool addMarker, const char* format, va_list args)
{
	if (g_Severity < severity || severity >= SV_COUNT) {
		return;
	}

	log_message message(g_File);
	
	if (addMarker) 
	{
		DWORD dThreadId = GetCurrentThreadId();

	#ifdef UNICODE
		message.printf(L"[%S:%04x]<%s> ", module, dThreadId, SEVERITY_TO_TSTR[severity]);
	#else
		message.printf(L"[%s:%04x]<%s> ", module, dThreadId, SEVERITY_TO_TSTR[severity]);
	#endif
	}
	message.vprintf(format, args);

	// check for eoln
	if (addEndl)
	{
		bool hasEndl = false;
		if (format)
		{
			for (; *format != 0; ++format) 
			{
				if (*format == '\n') {
					hasEndl = true;
				}
				else if ( !isspace(*format) ) {
					hasEndl = false;
				}
			}
		}
		
		if (!hasEndl) {
			message.puttc(_T('\n'));
		}
	}
}

void LogMessage(SEVERITY severity, const wchar_t* module, bool addEndl, bool addMarker, const wchar_t* format, va_list args)
{
	if (g_Severity < severity || severity >= SV_COUNT) {
		return;
	}

	log_message message(g_File);

	if (addMarker) 
	{
		DWORD dThreadId = GetCurrentThreadId();

	#ifdef UNICODE
		message.printf(L"[%s:%04x]<%s> ", module, dThreadId, SEVERITY_TO_TSTR[severity]);
	#else
		message.printf(L"[%S:%04x]<%s> ", module, dThreadId, SEVERITY_TO_TSTR[severity]);
	#endif
	}
	message.vprintf(format, args);

	// check for eoln
	if (addEndl)
	{
		bool hasEndl = false;
		if (format)
		{
			for (; *format != 0; ++format) 
			{
				if (*format == L'\n') {
					hasEndl = true;
				}
				else if ( !iswspace(*format) ) {
					hasEndl = false;
				}
			}
		}
		
		if (!hasEndl) {
			message.puttc(_T('\n'));
		}
	}
}

template<typename Char>
void LogMessage(SEVERITY severity, const Char* module, const Char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessage(severity, module, true, true, format,  args);
	va_end(args);
}

template<typename Char>
void LogMessage(SEVERITY severity, const std::basic_string<Char>& module, const std::basic_string<Char>& format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessage(severity, module.c_str(), true, true, format.c_str(), args);
	va_end(args);
}

template<typename Char>
void LogMessageEx(SEVERITY severity, const Char* module, bool addEndl, bool addMarker, const Char* format, ...)
{	
	va_list args;
	va_start(args, format);
	LogMessage(severity, module, addEndl, addMarker, format, args);
	va_end(args);
}

template<typename Char>
void LogMessageEx(SEVERITY severity, const std::basic_string<Char>& module, bool addEndl, bool addMarker, const std::basic_string<Char>& format, ...)
{
	va_list args;
	va_start(args, format);
	LogMessage(severity, module.c_str(), addEndl, addMarker, format.c_str(), args);
	va_end(args);
}

int ZLOG_API __cdecl VldReportHook( int /*reportType*/, wchar_t *message, int* /*returnValue*/ )
{
	zlog::LogMessageEx(SV_NOTICE, _T("VLD"), false, false, message);
	return 1;
}

template void ZLOG_API LogMessage<char>(SEVERITY severity, const char* module, const char* format, ...);
template void ZLOG_API LogMessage<wchar_t>(SEVERITY severity, const wchar_t* module, const wchar_t* format, ...);
template void ZLOG_API LogMessage<char>(SEVERITY severity, const std::basic_string<char>& module, const std::basic_string<char>& format, ...);
template void ZLOG_API LogMessage<wchar_t>(SEVERITY severity, const std::basic_string<wchar_t>& module, const std::basic_string<wchar_t>& format, ...);

template void ZLOG_API LogMessageEx<char>(SEVERITY severity, const char* module, bool addEndl, bool addMarker, const char* format, ...);
template void ZLOG_API LogMessageEx<wchar_t>(SEVERITY severity, const wchar_t* module, bool addEndl, bool addMarker, const wchar_t* format, ...);
template void ZLOG_API LogMessageEx<char>(SEVERITY severity, const std::basic_string<char>& module, bool addEndl, bool addMarker, const std::basic_string<char>& format, ...);
template void ZLOG_API LogMessageEx<wchar_t>(SEVERITY severity, const std::basic_string<wchar_t>& module, bool addEndl, bool addMarker, const std::basic_string<wchar_t>& format, ...);

} // namespace zlog

#ifdef BOOST_NO_EXCEPTIONS

// to support compilation without exceptions /EHsc
void boost::throw_exception(const stdext::exception& ex)
{
	zlog::LogMessage( zlog::SV_ERROR, "ZLOg", "exception: %s", ex.what() );
}

#endif // BOOST_NO_EXCEPTIONS
