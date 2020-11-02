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

#include "stdafx.h"
#include "BaseStereoRenderer.h"
#include "BasePresenter.h"
#include <MMSystem.h>

CBasePresenter::CBasePresenter( CBaseStereoRenderer* pStereoRenderer ) 
:	m_pStereoRenderer( pStereoRenderer )
,	m_ThreadID	( 0 )
,	m_hBackgroundThread( NULL )
,	m_TaskIndex( 0 )
,	m_hAVRTLib( NULL )
,	m_hAvrt( NULL )
,	m_bEndThread( false )
,	m_hrReset	( 0xFFFFFFFF )
,	m_Action	( ptNone )
,	m_RefreshRate( 0 )
,	m_SuperSynchronizer(&m_ddSyncronizer)
,	m_pBaseSwapChain( NULL )
,	m_pPresentHistory( NULL )
{
	timeBeginPeriod(1);

	m_hAVRTLib = LoadLibrary (L"AVRT.dll");
	if (m_hAVRTLib)
	{
		pfAvSetMmThreadCharacteristicsW		= (PTR_AvSetMmThreadCharacteristicsW)	GetProcAddress (m_hAVRTLib, "AvSetMmThreadCharacteristicsW");
		pfAvSetMmThreadPriority				= (PTR_AvSetMmThreadPriority)			GetProcAddress (m_hAVRTLib, "AvSetMmThreadPriority");
		pfAvRevertMmThreadCharacteristics	= (PTR_AvRevertMmThreadCharacteristics)	GetProcAddress (m_hAVRTLib, "AvRevertMmThreadCharacteristics");
	}
	else
	{
		pfAvSetMmThreadCharacteristicsW		= NULL;
		pfAvSetMmThreadPriority				= NULL;
		pfAvRevertMmThreadCharacteristics	= NULL;
	}

	D3DDISPLAYMODE Mode;
	_ASSERT(pStereoRenderer);
	pStereoRenderer->m_pDirect3D->GetAdapterDisplayMode(pStereoRenderer->m_nAdapter[0], &Mode);
#ifndef FINAL_RELEASE
	m_pPresentHistory = DNew CPresentHistory(Mode.RefreshRate, Mode.Height);
#endif
	CreateBackgroundThread();
}

CBasePresenter::~CBasePresenter()
{
	SetAction(ptReleaseDevice);
	WaitForActionDone();
	KillBackgroundThread();
	ResumeThread();
	DWORD result = WaitForSingleObject( m_hBackgroundThread, 500 );
	TerminateThread( m_hBackgroundThread, 1 );
	delete m_pPresentHistory;
	timeEndPeriod(1);
	if (m_hAVRTLib)
		FreeLibrary(m_hAVRTLib);
}

HANDLE CBasePresenter::CreateBackgroundThread	()
{
	// create the thread
	m_hBackgroundThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		BackgroundThreadProc, 
		this, 
		CREATE_SUSPENDED,				// HIGH_PRIORITY_CLASS, REALTIME_PRIORITY_CLASS
		&m_ThreadID);

	if( m_hBackgroundThread )
	{
		// set the priority
		SetThreadPriority( m_hBackgroundThread, PRESENTER_CPU_PRIORITY );
		
		// Tell Vista Multimedia Class Scheduler we are a playback thread (increase priority)
		// http://msdn.microsoft.com/en-us/library/ms684247%28VS.85%29.aspx
		if (pfAvSetMmThreadCharacteristicsW)	// Playback priority higher that Games
			m_hAvrt = pfAvSetMmThreadCharacteristicsW (L"Playback", &m_TaskIndex);
		if (pfAvSetMmThreadPriority)			
			pfAvSetMmThreadPriority (m_hAvrt, AVRT_THREAD_PRIORITY);
	}

	return m_hBackgroundThread;
}

unsigned WINAPI CBasePresenter::BackgroundThreadProc		( LPVOID lpParam )
{
	return		((CBasePresenter*)lpParam)->BackgroundThreadProc();
}

void CBasePresenter::SetCurrentRefreshRate()
{
	D3DDISPLAYMODE Mode;
	m_pStereoRenderer->m_pDirect3D->GetAdapterDisplayMode(m_pStereoRenderer->m_nAdapter[0], &Mode);
	m_RefreshRate = Mode.RefreshRate;
	DEBUG_MESSAGE(_T("SetCurrentRefreshRate = %i\n"), m_RefreshRate);
}

CBasePresenterSCData::CBasePresenterSCData( CBasePresenter* pPresenter, CBaseSwapChain* pSwapChain ) 
:	m_pBasePresenter ( pPresenter )
,	m_pBaseSwapChain ( pSwapChain )
,	m_hrPresent	( S_OK )
{
}

CBasePresenterSCData::~CBasePresenterSCData()
{
}

void CBasePresenterSCData::Initialize()
{
}

void CBasePresenterSCData::Cleanup()
{
}

float CBasePresenterSCData::GetFPS()
{
	return m_AverageFPS.GetFPS();
}

int CBasePresenterSCData::GetLastSleepTime()
{
	if (gInfo.PresenterSleepTime == -1)
		return m_AverageFPS.GetLastSleepTime();
	else if (m_pBasePresenter && ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3)))
		return m_pBasePresenter->m_SuperSynchronizer.GetLastSleepTime();
	return gInfo.PresenterSleepTime;
}

UINT CBasePresenterSCData::GetLagsPerSecond()
{
	return (UINT)m_AverageFPS.GetLagCount();
}

int CBasePresenterSCData::GetSleepTime()
{
	if (gInfo.PresenterSleepTime == -1)
		return m_AverageFPS.GetSleepTime();
	else if ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3))
		return m_pBasePresenter->m_SuperSynchronizer.GetLastSleepTime();
	return gInfo.PresenterSleepTime;
}

bool CBasePresenterSCData::SkipRender()
{
	if (m_pBasePresenter->m_pStereoRenderer->SkipPresenterThread())
		return true;
	return false;
}
