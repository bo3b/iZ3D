#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#if !defined(_WIN32_WINNT) && !defined(_CHICAGO_)
#define  _WIN32_WINNT   0x0600
#endif

#define NTDDI_VERSION_FROM_WIN32_WINNT2(ver)    ver##0000
#define NTDDI_VERSION_FROM_WIN32_WINNT(ver)     NTDDI_VERSION_FROM_WIN32_WINNT2(ver)

#ifndef NTDDI_VERSION
// set NTDDI_VERSION based on _WIN32_WINNT
#define NTDDI_VERSION   NTDDI_VERSION_FROM_WIN32_WINNT(_WIN32_WINNT)
#endif

#ifndef WINVER
#define WINVER          _WIN32_WINNT
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
#endif
