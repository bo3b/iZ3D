/*-----------------------------------------------------------------------------
 * iwrstdrv.h
 *
 * Copyright (c) 2007 Icuiti Corporation.  All rights reserved.
 *
 * Header file for the Icuiti iWear Stereo SDK library 
 *
 * Revision History:
 * $Log: /227-VR920/IWEARSDK/inc/iwrstdrv.h $
 * 
 * 4     12/11/07 4:30p Steves
 * 
 * 3     11/08/07 10:37a Steves
 * 
 * 2     11/01/07 5:07p Steves
 * 
 * 1     6/05/07 5:18p Steves
 * 
 */
#ifndef _IWRSTDRV_H
#define _IWRSTDRV_H

#define IWRSTEREO_LEFT_EYE  0
#define IWRSTEREO_RIGHT_EYE 1

#ifdef __cplusplus
extern "C" {
#endif

#include "iweardrv.h"

#ifndef IWRSTDRV_EXPLICIT
#ifdef IWRSTDRV_EXPORTS
__declspec( dllexport ) HANDLE __cdecl IWRSTEREO_Open(void);
__declspec( dllexport ) void __cdecl IWRSTEREO_Close(HANDLE hDev);
__declspec( dllexport ) BOOL __cdecl IWRSTEREO_SetStereo(HANDLE hDev, BOOL on);
__declspec( dllexport ) BYTE __cdecl IWRSTEREO_WaitForAck(HANDLE hDev, BOOL eye);
__declspec( dllexport ) BOOL __cdecl IWRSTEREO_SetLR(HANDLE hDev, BOOL eye);
__declspec( dllexport ) void __cdecl IWRSTEREO_SetLREx(HANDLE hDev, BOOL eye, BOOL wait);
__declspec( dllexport ) BOOL __cdecl IWRSTEREO_GetVersion(PIWRVERSION ver);
#else
__declspec( dllimport ) extern HANDLE IWRSTEREO_Open(void);
__declspec( dllimport ) extern void IWRSTEREO_Close(HANDLE hDev);
__declspec( dllimport ) extern BOOL IWRSTEREO_SetStereo(HANDLE hDev, BOOL on);
__declspec( dllimport ) extern BYTE IWRSTEREO_WaitForAck(HANDLE hDev, BOOL eye);
__declspec( dllimport ) extern BOOL IWRSTEREO_SetLR(HANDLE hDev, BOOL eye);
__declspec( dllimport ) extern void IWRSTEREO_SetLREx(HANDLE hDev, BOOL eye, BOOL wait);
__declspec( dllimport ) extern BOOL IWRSTEREO_GetVersion(PIWRVERSION);
#endif
#else
typedef HANDLE	(__cdecl *PIWRSTEREOOPEN)( void );
typedef void	(__cdecl *PIWRSTEREOCLOSE)(HANDLE hDev);
typedef BOOL	(__cdecl *PIWRSTEREOSETSTEREO)(HANDLE hDev, BOOL on);
typedef BYTE	(__cdecl *PIWRSTEREOWACK)(HANDLE hDev, BOOL eye);
typedef BOOL	(__cdecl *PIWRSTEREOSETLR)(HANDLE hDev, BOOL eye);
typedef void	(__cdecl *PIWRSTEREOSETLREX)(HANDLE hDev, BOOL eye, BOOL wait);
typedef BOOL	(__cdecl *PIWRSTEREOGETVERSION)(PIWRVERSION);

extern PIWRSTEREOOPEN IWRSTEREO_Open;
extern PIWRSTEREOCLOSE IWRSTEREO_Close;
extern PIWRSTEREOSETSTEREO IWRSTEREO_SetStereo;
extern PIWRSTEREOWACK IWRSTEREO_WaitForAck;
extern PIWRSTEREOSETLR IWRSTEREO_SetLR;
extern PIWRSTEREOSETLREX IWRSTEREO_SetLREx;
extern PIWRSTEREOGETVERSION IWRSTEREO_GetVersion;
#endif

#ifdef __cplusplus
}
#endif

#endif // _IWEARSTDRV_H