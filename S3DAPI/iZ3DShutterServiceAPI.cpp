#include "stdafx.h"
#include "iZ3DShutterServiceAPI.h"
#include "tchar.h"
#include "iZ3DShutterService.h"
#include "ddraw.h"
#include "dwmapi.h"
#include "ShutterAPI.h"
#include "FTD2XX.H"

static HANDLE		g_hiZ3DDriver		= INVALID_HANDLE_VALUE;
static HANDLE		g_hiZ3DEvent		= INVALID_HANDLE_VALUE;
static BOOL			g_bIzAeroEnabled	= FALSE;

static BOOL			g_bDoSwitch			= FALSE;
static FT_HANDLE	g_hXpadDevice		= NULL;
static HANDLE		g_hViewSwitchThread = INVALID_HANDLE_VALUE;

S3DAPI_API ShutterAPI g_KMShutter;

//////////////////////////////////////////////////////////////////////////
// D2XX library types
//////////////////////////////////////////////////////////////////////////


/************************************************************************/
/* InternalIsWin64                                                      */
/************************************************************************/
BOOL InternalIsWin64()
{
#ifndef WIN64
	BOOL bIsWow64 = FALSE;
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(	GetModuleHandle( TEXT( "kernel32.dll" ) ),"IsWow64Process" );
	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64 ))
		{
			// handle error
		}
	}
	return bIsWow64;	
#else
	return TRUE;	
#endif
}


/************************************************************************/
/* DisableComposition													*/
/************************************************************************/
void DisableComposition()
{
	HMODULE hDwmapi	= LoadLibrary( _T( "dwmapi.dll" ) );

	if ( hDwmapi )
	{

		typedef HRESULT (WINAPI *PFNDWMISCOMPOSITIONENABLED)( BOOL* pEnabled_ );
		PFNDWMISCOMPOSITIONENABLED pfnDwmIsCompositionEnabled = ( PFNDWMISCOMPOSITIONENABLED )GetProcAddress( hDwmapi, "DwmIsCompositionEnabled" );

		typedef HRESULT (WINAPI *PFNDWMENABLECOMPOSITION)( UINT nCompositionAction_ );
		PFNDWMENABLECOMPOSITION pfnDwmEnableComposition = ( PFNDWMENABLECOMPOSITION )GetProcAddress( hDwmapi, "DwmEnableComposition" );

		pfnDwmIsCompositionEnabled( &g_bIzAeroEnabled );

		if ( g_bIzAeroEnabled )
			pfnDwmEnableComposition( DWM_EC_DISABLECOMPOSITION );
		
		FreeLibrary( hDwmapi );		
	}
}

/************************************************************************/
/* EnableComposition													*/
/************************************************************************/
void EnableComposition()
{
	HMODULE hDwmapi	= LoadLibrary( _T( "dwmapi.dll" ) );

	if ( hDwmapi )
	{
		typedef HRESULT (WINAPI *PFNDWMENABLECOMPOSITION)( UINT nCompositionAction_ );
		PFNDWMENABLECOMPOSITION pfnDwmEnableComposition = ( PFNDWMENABLECOMPOSITION )GetProcAddress( hDwmapi, "DwmEnableComposition" );
		
		if ( g_bIzAeroEnabled )
			pfnDwmEnableComposition( DWM_EC_ENABLECOMPOSITION );

		FreeLibrary( hDwmapi );		
	}
}

/************************************************************************/
/* pfnViewSwitchThreadProc                                              */
/************************************************************************/
DWORD WINAPI ViewSwitchThreadProc( LPVOID )
{
	while ( g_bDoSwitch )
	{
		WaitForSingleObject( g_hiZ3DEvent, INFINITE );
		ResetEvent( g_hiZ3DEvent );

		if ( g_hXpadDevice != INVALID_HANDLE_VALUE )
		{
			BYTE	nSyncData	= 0xff;
			DWORD	dwNumBytes	= 0;
			FT_Write( g_hXpadDevice, &nSyncData, sizeof(nSyncData), &dwNumBytes );
		}
	}	

	return 0;
}

/************************************************************************/

/************************************************************************/
/* InitXpandEmitter                                                     */
/************************************************************************/
BOOL InitXpandEmitter()
{
	if (!InitFTD2XX())
		return FALSE;

	DWORD dwNumDevices = 0;
	if ( FT_CreateDeviceInfoList( &dwNumDevices ) != FT_OK || dwNumDevices == 0 )
		return FALSE;

	FT_DEVICE_LIST_INFO_NODE* pDevInfo = new FT_DEVICE_LIST_INFO_NODE[ dwNumDevices ];
	memset( pDevInfo, 0, sizeof( FT_DEVICE_LIST_INFO_NODE ) * dwNumDevices );

	FT_GetDeviceInfoList( pDevInfo, &dwNumDevices );	
	
	FT_DEVICE_LIST_INFO_NODE& tEmitterInfo	= pDevInfo[ 0 ];	

	FT_STATUS res =  FT_OpenEx( (PVOID)tEmitterInfo.SerialNumber, FT_OPEN_BY_SERIAL_NUMBER, &g_hXpadDevice );
	delete[] pDevInfo;

	if ( res != FT_OK )
		return FALSE;
	
	FT_SetBaudRate( g_hXpadDevice, FT_BAUD_19200 );
	FT_SetDataCharacteristics ( g_hXpadDevice, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE );
	FT_SetFlowControl( g_hXpadDevice, FT_FLOW_NONE, 0, 0 );

	/*
	BYTE	nSyncData	= 0xff;
	DWORD	dwNumBytes	= 0;
	res = FT_Write( g_hXpadDevice, &nSyncData, sizeof(nSyncData), &dwNumBytes );
	*/

	g_bDoSwitch = TRUE;
	unsigned dwThreadID = 0;
	g_hViewSwitchThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))ViewSwitchThreadProc, 
		NULL, 0, &dwThreadID);
	if ( g_hViewSwitchThread == INVALID_HANDLE_VALUE )	
		return FALSE;

	return TRUE;
}


/************************************************************************/
/* ReleaseXpandEmitter                                                  */
/************************************************************************/
void ReleaseXpandEmitter()
{
	if ( g_hXpadDevice != INVALID_HANDLE_VALUE )
	{
		g_bDoSwitch = FALSE;
		WaitForSingleObject( g_hViewSwitchThread, INFINITE );

		CloseHandle( g_hViewSwitchThread );
		g_hViewSwitchThread = INVALID_HANDLE_VALUE;

		FT_Close( g_hXpadDevice );
		g_hXpadDevice = ( FT_HANDLE )INVALID_HANDLE_VALUE;
	}	

	ReleaseFTD2XX();
}

/************************************************************************/
/* iZ3DInitShutter                                                      */
/************************************************************************/
BOOL iZ3DInitShutter()
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
		return FALSE;

	g_hiZ3DEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	if ( g_hiZ3DEvent == INVALID_HANDLE_VALUE )
		return FALSE;		

	g_hiZ3DDriver= CreateFile( _T("\\\\.\\iZ3DShutterService"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

	if ( g_hiZ3DDriver == INVALID_HANDLE_VALUE )
	{
		CloseHandle( g_hiZ3DEvent );
		g_hiZ3DEvent = INVALID_HANDLE_VALUE;
		return FALSE;
	}	

	TSHUTTERINITDATA tData	= { 0 };	
	tData.hEvent			= g_hiZ3DEvent;	
	DWORD dwBytesReturned	= 0;
	DWORD dwInitRes			= FALSE;

	DeviceIoControl( g_hiZ3DDriver, IOCTL_INITIALIZESHUTTER, &tData, sizeof( TSHUTTERINITDATA), &dwInitRes, sizeof( DWORD ), &dwBytesReturned, 0 );

	HRESULT hRes		= E_FAIL;
	LPDIRECTDRAW pDD	= NULL;
	HMODULE hDDraw		= LoadLibrary( _T( "ddraw.dll" ) );

	if ( hDDraw && dwInitRes == TRUE )
	{
		typedef HRESULT (WINAPI *PFNDIRECTDRAWCREATE)( GUID* lpGUID, IDirectDraw** lplpDD, IUnknown* pUnkOuter );
		PFNDIRECTDRAWCREATE pfnDirectDrawCreate = ( PFNDIRECTDRAWCREATE )GetProcAddress( hDDraw, "DirectDrawCreate" );
		if ( pfnDirectDrawCreate )
			hRes = pfnDirectDrawCreate( NULL, &pDD, NULL );
	}

	if( SUCCEEDED( hRes ) )
	{
		DeviceIoControl( g_hiZ3DDriver, IOCTL_STARTHOOKING_SCANLINE, NULL, 0, NULL, 0, &dwBytesReturned, 0 );

		DWORD dwScanLine = 0;
		pDD->GetScanLine( &dwScanLine );

		DeviceIoControl( g_hiZ3DDriver, IOCTL_STOPHOOKING_SCANLINE, NULL, 0, NULL, 0, &dwBytesReturned, 0 );

		pDD->Release();
		pDD = NULL;
	}
	else
		iZ3DReleaseShutter();

	if ( hDDraw )
		FreeLibrary( hDDraw );

	if ( g_hiZ3DDriver == INVALID_HANDLE_VALUE )
		return FALSE;

	if ( InternalIsWin64() )
		DisableComposition();

	InitXpandEmitter();

	return TRUE;
}

/************************************************************************/
/* iZ3DReleaseShutter                                                   */
/************************************************************************/
void iZ3DReleaseShutter()
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
	{
		ReleaseXpandEmitter();

		DWORD dwBytesReturned	= 0;
		DeviceIoControl( g_hiZ3DDriver, IOCTL_RELEASESHUTTER, NULL, 0, NULL, 0, &dwBytesReturned, 0 );

		CloseHandle( g_hiZ3DEvent );
		g_hiZ3DEvent = INVALID_HANDLE_VALUE;

		CloseHandle( g_hiZ3DDriver );
		g_hiZ3DDriver = INVALID_HANDLE_VALUE;

		if ( InternalIsWin64() )
			EnableComposition();
	}
}

/************************************************************************/
/* iZ3DWaitForPresent                                                   */
/************************************************************************/
BOOL iZ3DWaitForPresent( DWORD dwMilliseconds_ )
{
	if ( g_hiZ3DDriver == INVALID_HANDLE_VALUE )
		return FALSE;
	
	return WaitForSingleObject( g_hiZ3DEvent, dwMilliseconds_ ) == WAIT_OBJECT_0 ? TRUE : FALSE;
}

/************************************************************************/
/* iZ3DStartShutter                                                     */
/************************************************************************/
void iZ3DStartShutter()
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
	{
		DWORD dwBytesReturned = 0;
		DeviceIoControl( g_hiZ3DDriver, IOCTL_STARTSHUTTER, NULL, 0, NULL, 0, &dwBytesReturned, 0 );
	}
}

/************************************************************************/
/* iZ3DStopShutter                                                      */
/************************************************************************/
void iZ3DStopShutter()
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
	{
		DWORD dwBytesReturned = 0;
		DeviceIoControl( g_hiZ3DDriver, IOCTL_STOPSHUTTER, NULL, 0, NULL, 0, &dwBytesReturned, 0 );
	}
}


/************************************************************************/
/* iZ3DStartTest                                                        */
/************************************************************************/
void iZ3DStartTest( UINT nTestID_ )
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
	{
		DWORD dwBytesReturned = 0;
		DeviceIoControl( g_hiZ3DDriver, IOCTL_STARTTEST, &nTestID_, sizeof( UINT ), NULL, 0, &dwBytesReturned, 0 );
	}
}

/************************************************************************/
/* iZ3DOutputDebugString                                                */
/************************************************************************/
void iZ3DOutputDebugString( const char* szOutputStr_ )
{
	if ( g_hiZ3DDriver != INVALID_HANDLE_VALUE )
	{
		static char szOutputStr[ MAX_PATH ];	
		strncpy_s( szOutputStr, sizeof( szOutputStr ), szOutputStr_,  _TRUNCATE );
		szOutputStr[ MAX_PATH - 1 ] = '\0';

		DWORD dwBytesReturned	= 0;

		DeviceIoControl( g_hiZ3DDriver, IOCTL_OUTPUTDEBUGSTRING, szOutputStr, sizeof( szOutputStr ), NULL, 0, &dwBytesReturned, 0 );
	}
}