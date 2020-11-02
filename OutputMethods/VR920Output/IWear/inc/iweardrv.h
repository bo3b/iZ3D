/*-----------------------------------------------------------------------------
 * iweardrv.h
 *
 * Copyright (c) 2007 Icuiti Corporation.  All rights reserved.
 *
 * Header file for the Icuiti iWear SDK 
 *
 * Revision History:
 * $Log: /227-VR920/IWEARSDK/inc/iweardrv.h $
 * 
 * 4     9/25/08 10:44a Steves
 * 
 * 3     6/19/07 4:14p Steves
 * 
 * 2     5/24/07 2:31p Steves
 * 
 * 1     4/10/07 9:52a Steves
 * 
 * 1     4/10/07 9:33a Steves
 * 
 */
#ifndef _IWEARDRV_H
#define _IWEARDRV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_IWRVERSION {
	unsigned short DLLMajor;
	unsigned short DLLMinor;
	unsigned short DLLSubMinor;
	unsigned short DLLBuildNumber;
	char USBFirmwareMajor;
	char USBFirmwareMinor;
	char TrackerFirmwareMajor;
	char TrackerFirmwareMinor;
	char VideoFirmware;
} IWRVERSION, *PIWRVERSION;

#ifndef IWEARDRV_EXPLICIT
#ifdef IWEARDRV_EXPORTS
__declspec( dllexport ) DWORD __cdecl IWROpenTracker( void );
__declspec( dllexport ) void __cdecl IWRCloseTracker( void );
__declspec( dllexport ) void __cdecl IWRZeroSet( void );
__declspec( dllexport ) void __cdecl IWRZeroClear( void );
__declspec( dllexport ) DWORD __cdecl IWRGetTracking(LONG *yaw, LONG *pitch, LONG *roll);
__declspec( dllexport ) DWORD __cdecl IWRBeginCalibrate(void);
__declspec( dllexport ) void __cdecl IWREndCalibrate(BOOL save);
__declspec( dllexport ) DWORD __cdecl IWRGetVersion(PIWRVERSION ver);
__declspec( dllexport ) void __cdecl IWRSetFilterState(BOOL on);
__declspec( dllexport ) BOOL __cdecl IWRGetFilterState(void);
#else
__declspec( dllimport ) extern DWORD IWROpenTracker( void );
__declspec( dllimport ) extern void IWRCloseTracker( void );
__declspec( dllimport ) extern void IWRZeroSet( void );
__declspec( dllimport ) extern void IWRZeroClear( void );
__declspec( dllimport ) extern DWORD IWRGetTracking(LONG *yaw, LONG *pitch, LONG *roll);
__declspec( dllexport ) extern DWORD IWRBeginCalibrate(void);
__declspec( dllexport ) extern void IWREndCalibrate(BOOL save);
__declspec( dllimport ) extern DWORD IWRGetVersion(PIWRVERSION ver);
__declspec( dllimport ) extern void IWRSetFilterState(BOOL on);
__declspec( dllimport ) extern BOOL IWRGetFilterState(void);
#endif
#else
typedef DWORD	(__cdecl *PIWROPENTRACKER)( void );
typedef void	(__cdecl *PIWRCLOSETRACKER)( void );
typedef void	(__cdecl *PIWRZEROSET)( void );
typedef void	(__cdecl *PIWRZEROCLEAR)( void );
typedef DWORD	(__cdecl *PIWRGETTRACKING)(LONG *, LONG *, LONG *);
typedef DWORD	(__cdecl *PIWRGETVERSION)(PIWRVERSION);
typedef void	(__cdecl *PIWRSETFILTERSTATE)( BOOL );
typedef BOOL	(__cdecl *PIWRGETFILTERSTATE)(void);

extern PIWROPENTRACKER IWROpenTracker;
extern PIWRCLOSETRACKER IWRCloseTracker;
extern PIWRZEROSET IWRZeroSet;
extern PIWRGETTRACKING IWRGetTracking;
extern PIWRGETVERSION IWRGetVersion;
extern PIWRSETFILTERSTATE IWRSetFilterState;
extern PIWRGETFILTERSTATE IWRGetFilterState;
#endif

#ifdef __cplusplus
}
#endif

#endif