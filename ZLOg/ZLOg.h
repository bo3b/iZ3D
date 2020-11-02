#pragma once
#include "API.h"
#include <tchar.h>
#include <string>
#include <vadefs.h>

#ifdef ZLOG_ALLOW_TRACING
namespace zlog {
	
enum SEVERITY
{
	SV_FATAL_ERROR,
	SV_ERROR,
	SV_WARNING,
	SV_MESSAGE,
	SV_NOTICE,
	SV_FLOOD,
	SV_COUNT
};

// ZLOg string
typedef std::basic_string<char>		string;
typedef std::basic_string<wchar_t>	wstring;
typedef std::basic_string<TCHAR>	tstring;

/** Open file for logging or create if one not exists.
 * @param file - log file name.
 * @param append - append messages at the end of file.
 * @return true if succeeded. 
 */
bool ZLOG_API ReopenLogger(const string& file, bool append = false);

/** Open file for logging or create if one not exists.
 * @param file - log file name.
 * @param append - append messages at the end of file.
 * @return true if succeeded. 
 */
bool ZLOG_API ReopenLogger(const wstring& file, bool append = false);

/** Check whether logger is opened. */
bool ZLOG_API IsLoggerOpened();

/** Close log file */
void ZLOG_API CloseLogger();

/** Read ini config file, open logger. Start logger if necessary.
 * Example of config file:
 * \code
 * Severity      = FLOOD
 * Output        = log.txt
 * WriteThrought = 0
 * Buffering     = 0
 * \uncode
 */
void ZLOG_API InitializeFromINI(const string& configFile);

/** Read ini config file, open logger. Start logger if necessary.
 * Example of config file:
 * \code
 * Severity      = FLOOD
 * Output        = log.txt
 * WriteThrought = 0
 * Buffering     = 0
 * \uncode
 */
void ZLOG_API InitializeFromINI(const wstring& configFile);

/** Read xml config file, open logger. Start logger if necessary.
 * Example of config file:
 * \code
 * <Severity>ERROR</Severity>
 * <Output>log.txt</Severity>
 * <WriteThrough>1</WriteThrough>
 * <Buffering>0</Buffering>
 * \uncode
 * @param configFile - name of the xml config file.
 * @param section - section containing config. E.g. "Config.Logging.ZLOg" means the following 
 * config structure:
 * \code
 * <Config>
 *   ...
 *   <Logging>
 *     ...
 *     <ZLOg>
 *       ...
 *     </ZLOg>
 *   </Logging>
 * </Config>
 * \uncode
 */
void ZLOG_API InitializeFromXML(const string& configFile, const string& section = string());

/** Read xml config file, open logger. Start logger if necessary.
 * Example of config file:
 * \code
 * <Severity>ERROR</Severity>
 * <Output>log.txt</Severity>
 * <WriteThrough>1</WriteThrough>
 * <Buffering>0</Buffering>
 * \uncode
 * @param configFile - name of the xml config file.
 * @param section - section containing config. E.g. "Config.Logging.ZLOg" means the following 
 * config structure:
 * \code
 * <Config>
 *   ...
 *   <Logging>
 *     ...
 *     <ZLOg>
 *       ...
 *     </ZLOg>
 *   </Logging>
 * </Config>
 * \uncode
 */
void ZLOG_API InitializeFromXML(const wstring& configFile, const wstring& section = wstring());

/** Disable/enable system buffering. */
void ZLOG_API ToggleWriteThrough(bool toggle);

/** Disable/enable buffering */
void ZLOG_API ToggleBuffering(bool toggle);

/** Setup severity of logging */
void ZLOG_API SetupSeverity(SEVERITY severity);

/** Get severity of logging */
SEVERITY ZLOG_API GetSeverity();

/** Get pointer to severity of logging */
void ZLOG_API GetSeverityPtr(SEVERITY** ppSeverity);

/** Log printf-style formatted message. The format of the message is following: [Module]<severity>: Message.
 * End of line if automatically added after each message if it doesn't contains one.
 * @param severity - message severity. If global severity level is lesser than provided, ignore message.
 * @param module - name of the module for printing.
 * @param format - format for printf.
 */
template<typename Char>
void ZLOG_API LogMessage(SEVERITY severity, const Char* module, const Char* format, ...);

/** Log printf-style formatted message. The format of the message is following: [Module]<severity>: Message.
 * End of line if automatically added after each message if it doesn't contains one.
 * @param severity - message severity. If global severity level is lesser than provided, ignore message.
 * @param module - name of the module for printing.
 * @param format - format for printf.
 */
template<typename Char>
void ZLOG_API LogMessage(SEVERITY severity, const std::basic_string<Char>& module, const std::basic_string<Char>& format, ...);

/** Log printf-style formatted message. The format of the message is following: [Module]<severity>: Message. End of line is not added.
 * @param severity - message severity. If global severity level is lesser than provided, ignore message.
 * @param module - name of the module for printing.
 * @param format - format for printf.
 */
template<typename Char>
void ZLOG_API LogMessageEx(SEVERITY severity, const Char* module, bool addEndl, bool addMarker, const Char* format, ...);

/** Log printf-style formatted message. The format of the message is following: [Module]<severity>: Message. End of line is not added.
 * @param severity - message severity. If global severity level is lesser than provided, ignore message.
 * @param module - name of the module for printing.
 * @param format - format for printf.
 */
template<typename Char>
void ZLOG_API LogMessageEx(SEVERITY severity, const std::basic_string<Char>& module, bool addEndl, bool addMarker, const std::basic_string<Char>& format, ...);

int ZLOG_API __cdecl VldReportHook(int reportType, wchar_t *message, int *returnValue);

} // namespace zlog

/// Define this in your project to specify logging module 
// #define ZLOG_MODULE
#	ifdef ZLOG_PRINT_FILE_AND_LINE
#		define ZLOG_MESSAGE(severity, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, true, _T("%s(%d): "), _T(__FILE__), __LINE__);\
			zlog::LogMessageEx(severity, ZLOG_MODULE, true, false, __VA_ARGS__);\
		}
#		define ZLOG_MESSAGE_C(severity, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, true, "%S(%d): ", __FILE__, __LINE__);\
			zlog::LogMessageEx(severity, ZLOG_MODULE, true, false, __VA_ARGS__);\
		}
#		define ZLOG_MODULE_MESSAGE(severity, module, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, module, false, true, _T("%s(%d): "), _T(__FILE__), __LINE__);\
			zlog::LogMessageEx(severity, module, true, false, __VA_ARGS__);\
		}
#		define ZLOG_MODULE_MESSAGE_C(severity, module, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, module, false, true, "%S(%d): ", __FILE__, __LINE__);\
			zlog::LogMessageEx(severity, module, true, false, __VA_ARGS__);\
		}
#	elif defined(ZLOG_PRINT_FUNCTION)
#		define ZLOG_MESSAGE(severity, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, true, __VA_ARGS__);\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, false, _T("; %s\n"), _T(__FUNCTION__));\
		}
#		define ZLOG_MESSAGE_C(severity, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, true, __VA_ARGS__);\
			zlog::LogMessageEx(severity, ZLOG_MODULE, false, false, "; %s\n", __FUNCTION__);\
		}
#		define ZLOG_MODULE_MESSAGE(severity, module, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, module, false, true, __VA_ARGS__);\
			zlog::LogMessageEx(severity, module, false, false, _T("; %s\n"), __FUNCTION__));\
		}
#		define ZLOG_MODULE_MESSAGE_C(severity, module, ...)\
		if (zlog::GetSeverity() >= severity) {\
			zlog::LogMessageEx(severity, module, false, true, __VA_ARGS__);\
			zlog::LogMessageEx(severity, module, false, false, "; %s\n", __FUNCTION__);\
		}
#	else // !ZLOG_PRINT_FILE_AND_LINE && !ZLOG_PRINT_FUNCTION
#		define ZLOG_MESSAGE(severity, ...)						{ if (zlog::GetSeverity() >= severity) zlog::LogMessage(severity, ZLOG_MODULE, __VA_ARGS__); }
#		define ZLOG_MODULE_MESSAGE(severity, module, ...)		{ if (zlog::GetSeverity() >= severity) zlog::LogMessage(severity, module, __VA_ARGS__); }

#		define ZLOG_MESSAGE_C(severity, ...)					{ if (zlog::GetSeverity() >= severity) zlog::LogMessage(severity, ZLOG_MODULE, __VA_ARGS__); }
#		define ZLOG_MODULE_MESSAGE_C(severity, module, ...)		{ if (zlog::GetSeverity() >= severity) zlog::LogMessage(severity, module, __VA_ARGS__); }
#	endif // ZLOG_PRINT_FILE_AND_LINE

#else // !ALLOW_TRACING
#	define ZLOG_MESSAGE(severity, ...)						__noop
#	define ZLOG_MODULE_MESSAGE(severity, module, ...)		__noop

#	define ZLOG_MESSAGE_C(severity, ...)					__noop
#	define ZLOG_MODULE_MESSAGE_C(severity, module, ...)		__noop
#endif // ALLOW_TRACING
