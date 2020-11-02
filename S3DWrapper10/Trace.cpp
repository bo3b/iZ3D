#include "stdafx.h"
#include "Trace.h"
#include <StrSafe.h>

TCHAR* FormatStr( const TCHAR* szFormat, ... )
{	
	static TCHAR szBuf[ 1024 ];
	va_list vList;
	va_start( vList, szFormat );
	StringCchVPrintf( szBuf, 1024, szFormat, vList );
	va_end( vList );
	return szBuf;
}

const TCHAR* GetErrorMessage( DWORD nError, const TCHAR* szFunction )
{
	static TCHAR szTempMsg[ MAX_PATH ];
	static TCHAR szOutMsg[ MAX_PATH ];

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, nError, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPTSTR)szTempMsg, MAX_PATH, NULL );

	HRESULT hr = StringCchPrintf( szOutMsg, MAX_PATH, _T( "\"%s\" failed with error %d: %s" ), szFunction, nError, szTempMsg );
	_ASSERT( hr == S_OK );

	return szOutMsg;
}