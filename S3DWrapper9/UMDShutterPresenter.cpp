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
#include "UMDShutterPresenter.h"
#include "UMDDeviceWrapper.h"
#include "BaseStereoRenderer.h"

UMDShutterPresenter::UMDShutterPresenter( UMDDeviceWrapper * pUMDWrapper )
:	CBasePresenter	( pUMDWrapper->m_pStereoRenderer )
,	m_pDeviceFuncs	( &pUMDWrapper->m_DeviceFunctions )
,	m_hDevice		( pUMDWrapper->m_hDevice )
,   m_pCS			( &pUMDWrapper->m_CS )
,	m_hResult		( S_OK )
,	m_pUMDWrapper	( pUMDWrapper )
//,	m_pfnWaitForVBlank( NULL )
//,	m_pfnPresent	( NULL )
{
	DEBUG_MESSAGE(_T("Initialize UM presenter...\n"));

	//DEBUG_MESSAGE(_T("Load GDI32.DLL...\n"));
	//HMODULE hm = LoadLibrary(L"gdi32.dll");
	//m_pfnWaitForVBlank	= (PFND3DKMT_WAITFORVERTICALBLANKEVENT)	GetProcAddress(hm, "D3DKMTWaitForVerticalBlankEvent");
	//m_pfnPresent		= (PFND3DKMT_PRESENT)					GetProcAddress(hm, "D3DKMTPresent");
	m_Action = ptInitialize;
	ResumeThread();

	DEBUG_MESSAGE(_T("Initialize UM presenter done\n"));
}

DWORD WINAPI	UMDShutterPresenter::BackgroundThreadProc	()
{
	SetThreadName(-1, "UMDShutterPresenter");

	if (m_bEndThread)
		return 3;

	CBaseStereoRenderer* pRenderer;

	// MAIN LOOP
	while( !m_bEndThread )
	{
		switch( m_Action )
		{
		case ptInitialize:
			m_ddSyncronizer.Initialize();
			m_Action = ptNone;
			break;
		case ptInitializeSCData:
			m_pBaseSwapChain->m_pBasePresenterData->Initialize();
			m_Action = ptNone;
			break;
		case ptRender:
			pRenderer = m_pUMDWrapper->GetStereoRenderer();
			if (pRenderer->m_bEngineStarted && pRenderer->GetBaseSC()->m_pBasePresenterData)
			{
				CBasePresenterSCData*	pPresenterData = pRenderer->GetBaseSC()->m_pBasePresenterData;
				pPresenterData->RenderBackground();
				m_hResult = pPresenterData->GetLastPresentResult();
			}
			Sleep( 1 );
			break;
		case ptReset:
			m_Action = ptNone;
			break;
		case ptClearSCData:
			m_pBaseSwapChain->m_pBasePresenterData->Cleanup();
			m_Action = ptNone;
			break;
		case ptClear:
			if ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3))
				m_SuperSynchronizer.Clear();
			m_Action = ptNone;
			break;
		case ptReleaseSC:
			m_Action = ptNone;
			break;
		case ptReleaseDevice:
			// Cleanup
			m_Action = ptNone;
			break;
		default:
			SuspendThread();
			break;
		}
	}

	// Cleanup
	return 0;
}

CBasePresenterSCData* UMDShutterPresenter::CreateSwapChainData( CBaseSwapChain* pSwapChain )
{
	return DNew UMDShutterPresenterSCData( this, pSwapChain );
}

UMDShutterPresenter::~UMDShutterPresenter()
{
	if (!m_pStereoRenderer->m_SwapChains.empty())
		m_pStereoRenderer->GetBaseSC()->m_pBasePresenterData->ResetPresenter();
}

UMDShutterPresenterSCData::UMDShutterPresenterSCData( CBasePresenter* pPresenter, CBaseSwapChain* pSwapChain )
:	CBasePresenterSCData( pPresenter, pSwapChain )
,	m_pUMDPresenter( (UMDShutterPresenter*)pPresenter )
,	m_bUsePresent	( true )
,	m_FrameNumber	( 0 )
,	m_hSrcResource	( 0 )
,	m_hDstResource	( 0 )
,	m_LastUsedQueryIndex( 0 )
{
	DEBUG_MESSAGE(_T("Initialize UM presenter SC Data...\n"));
	m_FrameTime.QuadPart		= 0;
	m_LastFrameTime.QuadPart	= 0;
	m_RenderingTime.QuadPart	= 0;
	m_Frequency.QuadPart		= 0;

	// don't forget Zero Memory or shit happens
	ZeroMemory(&m_PresentData, sizeof(m_PresentData));
}

void UMDShutterPresenterSCData::SetData( UMDShutterPresenter* pPresenter, RECT * pSourceRect, /* Left and Right */ RECT * pDestRect, HANDLE hSrcResource, /* source surface with over/under stereo fields */ const CResourceHandleVector & hDstResource )
{
	m_pBasePresenter = pPresenter;
	m_pUMDPresenter = pPresenter;
	m_hSrcResource	= hSrcResource;
	m_hDstResource	= hDstResource;
	m_SourceRect[0]	= pSourceRect[0];		// Left...
	m_SourceRect[1]	= pSourceRect[1];		// ... and Right
	m_DestRect		= *pDestRect;
	m_BackbufferCount = (DWORD)m_hDstResource.size();

	if( gInfo.PresenterSleepTime == -4 )
	{
		// Create queries for type -4 synchronizer
		for( UINT i=0; i<m_BackbufferCount; i++ )
		{
			D3DDDIARG_CREATEQUERY	query;
			query.hQuery	= NULL;
			query.QueryType	= D3DDDIQUERYTYPE_EVENT;

			HRESULT hr		= m_pUMDPresenter->m_pDeviceFuncs->pfnCreateQuery( m_pUMDPresenter->m_hDevice, &query );

			if( SUCCEEDED( hr ) )
			{
				// store the query
				m_Query.push_back( query );

				// issue the query
				D3DDDIARG_ISSUEQUERY	iq;
				iq.hQuery		= query.hQuery;
				iq.Flags.Value	= 0;
				iq.Flags.End	= 1;

				hr		= m_pUMDPresenter->m_pDeviceFuncs->pfnIssueQuery( m_pUMDPresenter->m_hDevice, &iq );
			}
		}
		_ASSERT( m_Query.size() == m_BackbufferCount );
	}
}

void UMDShutterPresenterSCData::Cleanup()
{
	m_Frequency.QuadPart = NULL;
	m_AverageFPS.SetRefreshRate( 0 );
	if( gInfo.PresenterSleepTime == -4 )
	{
		// Delete queries
		_ASSERT( m_Query.size() == m_BackbufferCount );

		for( UINT i=0; i<m_BackbufferCount; i++ )
		{
			HRESULT hr		= m_pUMDPresenter->m_pDeviceFuncs->pfnDestroyQuery( m_pUMDPresenter->m_hDevice, m_Query[i].hQuery );
		}
		m_Query.clear();
		m_LastUsedQueryIndex = 0;
	}
}

void UMDShutterPresenterSCData::CalculateFPS()
{
	if( !m_Frequency.QuadPart )						// first time initialize
	{
		QueryPerformanceFrequency( &m_Frequency );
		QueryPerformanceCounter( &m_LastFrameTime );
		DEBUG_MESSAGE(_T("Updated SwapChain refresh rate\n"));
		m_AverageFPS.SetRefreshRate( m_pBasePresenter->m_RefreshRate );
	}
	else
	{
		LARGE_INTEGER	CurrentTime = *m_pBasePresenter->m_SuperSynchronizer.GetVSyncTime();
		m_FrameTime.QuadPart	= CurrentTime.QuadPart - m_LastFrameTime.QuadPart;
		m_LastFrameTime			= CurrentTime;
		float	frametime		= 1000.0f * m_FrameTime.QuadPart/m_Frequency.QuadPart; 
		m_AverageFPS.PushFrameTime( frametime );
	}
	++m_FrameNumber;
}

void AccurateSleep(DWORD ms)
{
	LARGE_INTEGER end, freq;
	QueryPerformanceCounter(&end);
	QueryPerformanceFrequency(&freq);
	end.QuadPart += (ms * 10 + 5) * freq.QuadPart / 10000;
	for (DWORD i = 0; i < ms; i++)
	{
		Sleep(1);
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		if (time.QuadPart >= end.QuadPart)
			break;
	}
}

void UMDShutterPresenterSCData::RenderBackground		()
{	
	if ( SkipRender() || !m_bUsePresent || !m_PresentData.Flags.Flip )	// fullscreen mode with triple buffering
	{
		m_hrPresent = S_OK;
		return;
	}

	UMDDeviceWrapper* pUMWrapper = m_pUMDPresenter->m_pUMDWrapper;
	CBaseStereoRenderer* pStereoRenderer = m_pBaseSwapChain->GetBaseDevice();
	CBaseSwapChain* sc = pStereoRenderer->GetBaseSC();

	if (gInfo.PresenterSleepTime == -2)
	{
		if (gInfo.PresenterFlushTime >= 0)
			pStereoRenderer->m_PresenterFlushTime = gInfo.PresenterFlushTime;
		else if (m_pBaseSwapChain->IsStereoActive() && !m_pBaseSwapChain->m_PresentationParameters[0].Windowed)
			pStereoRenderer->m_PresenterFlushTime = m_AverageFPS.GetFlushTime();
	}
	int sleepTime = GetSleepTime();

	if (sleepTime >= 0)
		Sleep(sleepTime); //AccurateSleep(sleepTime);

	//--- data for x0ras-like research and analysis ---
	if(sc)
		PH_SaveGameFps((float)sc->m_fFPS);
	PH_SaveSleepTime(sleepTime);
	PH_SaveSkipPerSecond(GetLagsPerSecond());
	PH_SavePresenterFps(GetFPS());
	PH_SaveFlushCount(pStereoRenderer->m_nFlushCounter);
	pStereoRenderer->m_nFlushCounter = 0;
	PH_SaveGetDataCount(pStereoRenderer->m_nGetDataCounter);
	pStereoRenderer->m_nGetDataCounter = 0;

	HRESULT			hResult		= S_OK;
	DWORD			BufferIndx	= (m_FrameNumber + 1) % m_BackbufferCount;		// N.B. We're starting from second buffer!
	DWORD			ViewIndx	= m_FrameNumber & 0x0001;

	PH_AfterSleep();

#ifndef FINAL_RELEASE
	LARGE_INTEGER liBefore, liAfter;
	QueryPerformanceCounter(&liBefore);
#endif
	m_pUMDPresenter->m_pCS->Enter();

#ifndef FINAL_RELEASE
	QueryPerformanceCounter(&liAfter);
	CallTimeData& ctdata = g_Profiler[g_CurrentUMFunctionName];
	g_CurrentUMFunctionName = L"No wait";
	ctdata.totalTime += DWORD(liAfter.QuadPart - liBefore.QuadPart);
	ctdata.maxTime = std::max(ctdata.maxTime, DWORD(liAfter.QuadPart - liBefore.QuadPart));
	ctdata.callCount++;
#endif

	if( gInfo.PresenterSleepTime == -4 )
	{
		// check the query
		D3DDDIARG_GETQUERYDATA	query;
		DWORD			queryData = 0;
		query.hQuery	= m_Query[ m_LastUsedQueryIndex ].hQuery;
		query.pData		= &queryData /*NULL*/;

		HRESULT		hr	= m_pUMDPresenter->m_pDeviceFuncs->pfnGetQueryData( m_pUMDPresenter->m_hDevice, &query );
		_ASSERT( E_OUTOFMEMORY != hr );

		if( S_FALSE == hr )
		{
			m_pUMDPresenter->m_pCS->Leave();
			m_hrPresent = E_PENDING;	// the query result is not ready yet (or GetQueryData has failed)
			return;
		}
	}	
	
	//////////////////////////////////////////////////////////////////////////
	// update primary buffers (front + back) from the doubled backbuffer resource
	CONST RECT *	pRect		= GetStereoResourceParameters();	
	D3DDDIARG_BLT	blt;
	blt.hSrcResource			= m_hSrcResource;		
	blt.SrcSubResourceIndex		= 0;
	blt.SrcRect					= m_SourceRect[ ViewIndx ];
	blt.hDstResource			= m_hDstResource[ BufferIndx ].hResource;
	blt.DstSubResourceIndex		= m_hDstResource[ BufferIndx ].SubResourceIndex;
	blt.DstRect					= m_DestRect;
	blt.ColorKey				= 0;
	blt.Flags.Value				= 0;		
	NSCALL( m_pUMDPresenter->m_pDeviceFuncs->pfnBlt(m_pUMDPresenter->m_hDevice, &blt ));
	NSCALL(	m_pUMDPresenter->m_pDeviceFuncs->pfnFlush	( m_pUMDPresenter->m_hDevice )	);

	PH_BeforePresent();
	if (gInfo.PresenterSleepTime == -3)
	{
		m_pBasePresenter->m_SuperSynchronizer.BeforePresent();
	}

	D3DDDIARG_PRESENT	prsnt = m_PresentData;
	prsnt.hSrcResource			= m_hDstResource[ BufferIndx ].hResource;
	prsnt.SrcSubResourceIndex	= m_hDstResource[ BufferIndx ].SubResourceIndex;
	prsnt.FlipInterval			= D3DDDI_FLIPINTERVAL_ONE;

	NSCALL(	m_pUMDPresenter->m_pDeviceFuncs->pfnPresent	( m_pUMDPresenter->m_hDevice, &prsnt )	);

	m_pBasePresenter->m_SuperSynchronizer.AfterPresent();
	LARGE_INTEGER* pVSyncTime = m_pBasePresenter->m_SuperSynchronizer.GetVSyncTime();
	PH_SetVSyncTime(pVSyncTime);

	m_pUMDPresenter->m_pCS->Leave();
	pStereoRenderer->m_pOutputMethod->AfterPresent(ViewIndx == 0);

	switch(gInfo.PresenterSleepTime)
	{
	case -2:
		{
			m_pBasePresenter->m_SuperSynchronizer.AfterPresentOne();
#ifndef FINAL_RELEASE
			LARGE_INTEGER CurrentTime = *pVSyncTime;
			LARGE_INTEGER Diff;
			Diff.QuadPart	= CurrentTime.QuadPart - m_LastFrameTime.QuadPart;
			float frameTime	= 1000.0f * Diff.QuadPart / m_Frequency.QuadPart;
			DXGI_RATIONAL refreshRate = m_AverageFPS.GetRefreshRate();
			float vblankTime = 1000.0f * refreshRate.Denominator / refreshRate.Numerator;
			if (frameTime / vblankTime > 1.5f) // lag happens
			{
				ctdata.maxTime += (DWORD)(m_Frequency.QuadPart / 10);
			}
#endif
		}
		break;
	case -3:
		m_pBasePresenter->m_SuperSynchronizer.AfterPresentTwo();
		break;
	case -4:
		{
			// issue the query
			D3DDDIARG_ISSUEQUERY	query;
			query.hQuery	= m_Query[ m_LastUsedQueryIndex ].hQuery;
			query.Flags.Value = 0;
			query.Flags.End	= 1;

			HRESULT		hr	= m_pUMDPresenter->m_pDeviceFuncs->pfnIssueQuery( m_pUMDPresenter->m_hDevice, &query );

			m_LastUsedQueryIndex = (m_LastUsedQueryIndex + 1) % m_BackbufferCount;	// process the next backbuffer
		}
		break;
	}
	PH_AfterPresent();

	CalculateFPS();

	m_hrPresent = hResult;
}
