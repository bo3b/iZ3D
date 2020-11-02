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
// File Name:	IWRsdk.c		
// Description:	Functions to load the iWearDrv.dll and receive tracking information.
#include "stdafx.h"
#include "IWRsdk.h"

//PIWROPENTRACKER IWROpenTracker;
//PIWRCLOSETRACKER IWRCloseTracker;
//PIWRZEROSET IWRZeroSet;
//PIWRGETTRACKING IWRGetTracking;
//PIWRGETVERSION IWRGetVersion;

//HMODULE IWRTrackerInstance=NULL;

PIWRSTEREOOPEN IWRSTEREO_Open;
PIWRSTEREOCLOSE IWRSTEREO_Close;
PIWRSTEREOSETSTEREO IWRSTEREO_SetStereo;
PIWRSTEREOWACK IWRSTEREO_WaitForAck;
PIWRSTEREOSETLR IWRSTEREO_SetLR;
PIWRSTEREOSETLREX IWRSTEREO_SetLREx;
PIWRSTEREOGETVERSION IWRSTEREO_GetVersion;

HMODULE IWRStereoInstance=NULL;

// Name:	IWRLoadDll
// Desc:	loading the iWear tracker driver and initializes the tracking system
// Returns:	IWR_OK
//			IWR_NO_TRACKER 
//			IWR_TRACKER_CORRUPT	
//
long	IWRLoadDll(void){
	/*IWRTrackerInstance = LoadLibrary( _T("IWEARDRV.DLL") );
	if( IWRTrackerInstance == NULL) 
		return IWR_NO_TRACKER;
	IWROpenTracker = (PIWROPENTRACKER) GetProcAddress(IWRTrackerInstance, "IWROpenTracker" );
	IWRCloseTracker = (PIWRCLOSETRACKER) GetProcAddress(IWRTrackerInstance, "IWRCloseTracker" );
	IWRZeroSet = (PIWRZEROSET) GetProcAddress(IWRTrackerInstance, "IWRZeroSet" );
	IWRGetTracking = (PIWRGETTRACKING) GetProcAddress(IWRTrackerInstance, "IWRGetTracking" );
	IWRGetFilterState  = (PIWRGETFILTERSTATE) GetProcAddress(IWRTrackerInstance, "IWRGetFilterState" );
	IWRSetFilterState  = (PIWRSETFILTERSTATE) GetProcAddress(IWRTrackerInstance, "IWRSetFilterState" );*/
	
	IWRStereoInstance = LoadLibrary( _T("IWRSTDRV.DLL") );
	if( IWRStereoInstance == NULL) 
		return IWR_NO_STEREO;
	IWRSTEREO_Open = (PIWRSTEREOOPEN) GetProcAddress(IWRStereoInstance, "IWRSTEREO_Open" );
	IWRSTEREO_Close = (PIWRSTEREOCLOSE) GetProcAddress(IWRStereoInstance, "IWRSTEREO_Close" );
	IWRSTEREO_SetLR = (PIWRSTEREOSETLR) GetProcAddress(IWRStereoInstance, "IWRSTEREO_SetLR" );
	IWRSTEREO_SetStereo = (PIWRSTEREOSETSTEREO) GetProcAddress(IWRStereoInstance, "IWRSTEREO_SetStereo" );
	IWRSTEREO_WaitForAck = (PIWRSTEREOWACK) GetProcAddress(IWRStereoInstance, "IWRSTEREO_WaitForAck" );
	if( /*!IWRGetTracking || !IWROpenTracker || !IWRCloseTracker || !IWRZeroSet || */
		!IWRSTEREO_Open || !IWRSTEREO_Close || !IWRSTEREO_SetLR || !IWRSTEREO_SetStereo )
		return IWR_STEREO_CORRUPT;

	//IWROpenTracker();
	return  IWR_OK;
}

// Name:	IWRFreeDll
// Desc:	Frees the dynamically loaded tracker driver.
// Retu:    IWR_OK
//			IWR_NO_INSTANCE
//
long	IWRFreeDll( void ){
	if( IWRStereoInstance ) {
		FreeLibrary(IWRStereoInstance);
		IWRStereoInstance = NULL;
	/*}
	if( IWRTrackerInstance ) {
		if( IWRCloseTracker )
			IWRCloseTracker();	
		FreeLibrary( IWRTrackerInstance );
		IWRTrackerInstance = NULL;*/
		return IWR_OK;		
	}
	return IWR_NOTRACKER_INSTANCE;
}
