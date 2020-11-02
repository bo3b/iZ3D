#include "stdafx.h"
#include "ftd2xx.h"

TPFN_FT_OpenEx					FT_OpenEx					= NULL;
TPFN_FT_Close					FT_Close					= NULL;
TPFN_FT_CreateDeviceInfoList	FT_CreateDeviceInfoList		= NULL;
TPFN_FT_GetDeviceInfoList		FT_GetDeviceInfoList		= NULL;
TPFN_FT_SetBaudRate				FT_SetBaudRate				= NULL;
TPFN_FT_SetDataCharacteristics	FT_SetDataCharacteristics	= NULL;
TPFN_FT_SetFlowControl			FT_SetFlowControl			= NULL;
TPFN_FT_Write					FT_Write					= NULL;
TPFN_FT_Read					FT_Read						= NULL;


FT_HANDLE						g_hXpandDll				= NULL;

/************************************************************************/
/* InitFTD2XX                                                           */
/************************************************************************/
BOOL	InitFTD2XX()
{
	g_hXpandDll = LoadLibrary( _T("Ftd2xx.dll") );
	if ( !g_hXpandDll )
		return FALSE;

	FT_OpenEx					= (TPFN_FT_OpenEx)GetProcAddress( g_hXpandDll, "FT_OpenEx" );
	FT_Close					= (TPFN_FT_Close)GetProcAddress( g_hXpandDll, "FT_Close" );
	FT_CreateDeviceInfoList		= (TPFN_FT_CreateDeviceInfoList)GetProcAddress( g_hXpandDll, "FT_CreateDeviceInfoList" );
	FT_GetDeviceInfoList		= (TPFN_FT_GetDeviceInfoList)GetProcAddress( g_hXpandDll, "FT_GetDeviceInfoList" );
	FT_SetBaudRate				= (TPFN_FT_SetBaudRate)GetProcAddress( g_hXpandDll, "FT_SetBaudRate" );
	FT_SetDataCharacteristics	= (TPFN_FT_SetDataCharacteristics)GetProcAddress( g_hXpandDll, "FT_SetDataCharacteristics" );
	FT_SetFlowControl			= (TPFN_FT_SetFlowControl)GetProcAddress( g_hXpandDll, "FT_SetFlowControl" );
	FT_Write					= (TPFN_FT_Write)GetProcAddress( g_hXpandDll, "FT_Write" );
	FT_Read						= (TPFN_FT_Read)GetProcAddress( g_hXpandDll, "FT_Read" );
	
	return TRUE;
}


/************************************************************************/
/* ReleaseFTD2XX                                                        */
/************************************************************************/
VOID	ReleaseFTD2XX()
{
	if ( g_hXpandDll )
		FreeLibrary( g_hXpandDll );

	FT_OpenEx					= NULL;
	FT_Close					= NULL;
	FT_CreateDeviceInfoList		= NULL;
	FT_GetDeviceInfoList		= NULL;
	FT_SetBaudRate				= NULL;
	FT_SetDataCharacteristics	= NULL;
	FT_SetFlowControl			= NULL;
	FT_Write					= NULL;
	FT_Read						= NULL;

	g_hXpandDll = NULL;
}