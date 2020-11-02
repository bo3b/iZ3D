//--------------------------------------------------------------------------------------
// File:	Presenter.cpp
// Wrapper:	DX10
//
//
//--------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Presenter.h"
#include "D3DSwapChain.h"
#include <Mmsystem.h>			/* timeBeginPeriod */
#include "..\OutputMethods\OutputLib\OutputMethod_dx10.h"		// odShutterMode definition
#include "ResourceWrapper.h"
#include "Commands\Blt.h"
#include "DXGISwapChainWrapper.h"

CBasePresenterX::CBasePresenterX() 
	:	m_pDeviceWrapper	( NULL	)
	,	m_ThreadID			( 0		)	
	,	m_hBackgroundThread	( NULL	)
	,	m_TaskIndex			( 0		)
	,	m_hAVRTLib			( NULL	)
	,	m_hAvrt				( NULL	)
	,	m_bEndThread		( false )
	,	m_hrReset	( 0xFFFFFFFF )
	,	m_Action	( ptNone )
	,	m_FrameNumber( 0 )
	,	m_RefreshRate( )
	,	m_SuperSynchronizer( &m_ddSyncronizer )	// will it work for UM?!
	,	m_pSwapChainWrapper( NULL )
	,	m_pPresentHistory( NULL )
	,	m_BltData()								// initialize with zeros
	,	m_PresentData()							// initialize with zeros
	,	m_BackbufferCount	( 0 )				// FIXME! (triple buffering?)
	,   m_bUsePresent(true)
	,   m_bUseSimplePresenter(true)
{
	timeBeginPeriod( 1 );

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

// #ifndef FINAL_RELEASE
// 	m_pPresentHistory = DNew CPresentHistory(Mode.RefreshRate, Mode.Height);
// #endif
	CreateBackgroundThread();
}

CBasePresenterX::~CBasePresenterX()
{
	SetActionAndWait(ptReleaseDevice);
	KillBackgroundThread();
	ResumeThread();
	DWORD result = WaitForSingleObject( m_hBackgroundThread, 500 );
	TerminateThread( m_hBackgroundThread, 1 );
	timeEndPeriod(1);
	if (m_hAVRTLib)
		FreeLibrary(m_hAVRTLib);
#ifndef FINAL_RELEASE
	SAFE_DELETE( m_pPresentHistory );
#endif
}

void CBasePresenterX::Init( D3DDeviceWrapper* pDeviceWrapper_ )
{
	m_pDeviceWrapper	= pDeviceWrapper_;
	m_pSwapChainWrapper	= m_pDeviceWrapper->GetD3DSwapChain();
	
#ifndef FINAL_RELEASE
	if (m_pPresentHistory)
		delete m_pPresentHistory;

	_ASSERT( m_pSwapChainWrapper );
	UINT	RefreshRate	= m_pSwapChainWrapper->m_Description.BufferDesc.RefreshRate.Numerator;
	UINT	Height		= m_pSwapChainWrapper->m_Description.BufferDesc.Height;
	if( 1 != m_pSwapChainWrapper->m_Description.BufferDesc.RefreshRate.Denominator )
		RefreshRate		= (UINT)(0.5 + (double)RefreshRate/(double)m_pSwapChainWrapper->m_Description.BufferDesc.RefreshRate.Denominator);	

	m_pPresentHistory = DNew CPresentHistory( RefreshRate, Height );
#endif
}

HANDLE CBasePresenterX::CreateBackgroundThread	()
{
	// create the thread
	m_hBackgroundThread = (HANDLE)_beginthreadex( NULL, 0, BackgroundThreadProc, 
		this, CREATE_SUSPENDED, &m_ThreadID);

/*
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
	*/
	return m_hBackgroundThread;
}

unsigned WINAPI CBasePresenterX::BackgroundThreadProc		( LPVOID lpParam )
{
	return		((CBasePresenterX*)lpParam)->BackgroundThreadProc();
}

void	CBasePresenterX::AddBackBuffer( DXGI_DDI_HRESOURCE hResource, UINT	SubResourceCount )
{
	CDDIResourceHandle		rh;
	rh.hResource		= hResource;
	rh.SubResourceIndex	= SubResourceCount;
	m_Backbuffer.push_back( rh );
	m_BackbufferCount = std::max<UINT>( 1, (UINT)m_Backbuffer.size()-1 );
}

void	CBasePresenterX::AddResourceIdentities( DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData  )
{
	if( !m_ResourceIdentities.empty() )
		return;

	for( UINT i=0; i<pRotateData->Resources; i++ )
		m_ResourceIdentities.push_back( pRotateData->pResources[i] );
}

void CBasePresenterX::SetRefreshRate( DXGI_RATIONAL refreshRate )
{
	m_RefreshRate = refreshRate; 
	if (m_RefreshRate.Denominator == 0)
	{
		m_RefreshRate.Numerator = 120;
		m_RefreshRate.Denominator = 1;
	}
}

void CBasePresenterX::ReInit( DXGI_RATIONAL newRefreshRate )
{
	SetRefreshRate( newRefreshRate );
	SetActionAndWait(ptClear);
	SetActionAndWait(ptInitialize);
}

DWORD WINAPI	CPresenterX::BackgroundThreadProc		()
{
	SetThreadName(-1, "DX10ShutterPresenter");	

	if (m_bEndThread)
		return 3;

	while( !m_bEndThread )
	{
		switch( m_Action )
		{
		case ptInitialize:
			//CreatePresenterWindow( m_hSrcWnd );
			m_ddSyncronizer.Initialize();

			m_AverageFPS.SetRefreshRate(m_RefreshRate);
			m_SuperSynchronizer.Initialize(m_RefreshRate, 2);

			m_Action = ptNone;
			break;
		case ptInitializeSCData:
			m_Action = ptNone;
			break;
		case ptUpdateWindow:
			m_Action = ptNone;
			break;
		case ptSetFullscreenState:
			{
				g_SwapChainMode = scResizing;
				SetCurrentSwapChain setSwapChain(m_pSwapChainWrapper->m_pSwapChain);
				m_pSwapChainWrapper->m_pSwapChain->SetFullscreenState(m_DXGIParametrs.Fullscreen, m_DXGIParametrs.pTarget);
				g_SwapChainMode = scNormal;
				m_Action = ptNone;
			}
			break;
		case ptResizeTarget:
			{
				SetCurrentSwapChain setSwapChain(m_pSwapChainWrapper->m_pSwapChain);
				m_pSwapChainWrapper->m_pSwapChain->ResizeTarget(m_DXGIParametrs.pNewTargetParameters);
				m_Action = ptNone;
			}
			break;
		case ptRender:
			RenderBackground();
			break;
		case ptClearSCData:
			m_Action = ptNone;
			break;
		case ptClear:
			CleanupBackground();
			m_Action = ptNone;
			break;
		case ptReleaseSC:
			m_pSwapChainWrapper = NULL;
			m_Action = ptNone;
			break;
		case ptReleaseDevice:
			// Cleanup
			m_ddSyncronizer.Reset();
			m_Action = ptNone;
			break;
		default:
			Sleep(1);
			break;
		}
	}	

	// Cleanup
	return 0;
}

bool			CPresenterX::SkipRender()
{
	if (m_pDeviceWrapper->SkipPresenterThread())
		return true;
	return false;
}

void			CPresenterX::RenderBackground			()
{
	HRESULT			hResult		= S_OK;
	if ( SkipRender() )
	{
		m_hrPresent = S_OK;
		return;
	}

	if (gInfo.PresenterSleepTime == -2)
	{
		if (gInfo.PresenterFlushTime >= 0)
			m_pDeviceWrapper->m_PresenterFlushTime = gInfo.PresenterFlushTime;
		else if (m_pDeviceWrapper->IsStereoActive() && !m_pSwapChainWrapper->m_Description.Windowed)
			m_pDeviceWrapper->m_PresenterFlushTime = m_AverageFPS.GetFlushTime();
	}
	int sleepTime = GetSleepTime();

	if (sleepTime >= 0)
		Sleep(sleepTime); //AccurateSleep(sleepTime);
	PH_AfterSleep();

#ifndef	FINAL_RELEASE
	//--- data for x0ras-like research and analysis ---
	if( m_pSwapChainWrapper )
		PH_SaveGameFps	( (float) m_pSwapChainWrapper->m_fFPS );
	PH_SaveSleepTime	( sleepTime );
	PH_SaveSkipPerSecond( GetLagsPerSecond() );
	PH_SavePresenterFps	( GetFPS() );
	PH_SaveFlushCount	( m_pDeviceWrapper->m_nFlushCounter );
	m_pDeviceWrapper->m_nFlushCounter = 0;
	PH_SaveGetDataCount	( m_pDeviceWrapper->m_nGetDataCounter );
	m_pDeviceWrapper->m_nGetDataCounter = 0;
#endif

	_ASSERT( m_BackbufferCount );
	DWORD			BufferIndx	= (m_FrameNumber + 1) % m_BackbufferCount;		// N.B. We're starting from second buffer!
	DWORD			ViewIndx	= m_FrameNumber & 0x0001;
	bool			bIsLeft		= (ViewIndx == 0);								// zero means left image

	PH_BeforePresent();
	if (gInfo.PresenterSleepTime == -3)
	{
		m_SuperSynchronizer.BeforePresent();
	}

	SetCurrentSwapChain setSwapChain(m_pSwapChainWrapper->m_pSwapChain);
	m_pSwapChainWrapper->m_bPresenterCall = true;
	m_pSwapChainWrapper->m_bResourceCopyRegionCalled = false;
	m_pSwapChainWrapper->m_bPresenterThreadId = GetCurrentThreadId();

	/*
	//------------  In fullscreen mode use UM commands instead of m_pSwapChain->Present() ---------
	//---  не получилсь.  :-(   ---
	if(m_pSwapChainWrapper->m_Description.Windowed == FALSE)
	{
		static int i = 0;
		if(!i)
		{
			NSCALL(m_pSwapChainWrapper->m_pSwapChain->Present( 1, 0 ));
			i++;
		}
		else
		{
			CriticalSectionLocker cs(m_pDeviceWrapper->m_CSUMCall);
			UINT frame = CPresenterX::Get()->GetFrameCount();
			bool isLeft = !( frame & 0x0001 );
			SwapChainResources* pRes = m_pSwapChainWrapper->GetCurrentPrimaryResource();
			D3D10DDI_HRESOURCE hPresenterResource = pRes->GetPresenterResource(isLeft);
			if (hPresenterResource.pDrvPrivate == NULL)
			{
				DEBUG_MESSAGE(_T("WARNING: m_hPresenterResourceLeft == NULL, probably presenter bug\n"));
				return;
			}

			D3D10DDI_HDEVICE hDevice = m_pDeviceWrapper->hDevice;
			D3D10DDI_HRESOURCE hDstResource = pRes->GetBackBufferPrimary();//pRes->GetBackBufferLeft();//
			UINT DstSubresource = 0;
			UINT DstX = 0;
			UINT DstY = 0;
			UINT DstZ = 0;
			D3D10DDI_HRESOURCE hSrcResource = hPresenterResource;
			UINT SrcSubresource = 0;
			m_pDeviceWrapper->OriginalDeviceFuncs.pfnResourceCopyRegion(hDevice, hDstResource, DstSubresource, DstX, DstY, DstZ, hSrcResource, SrcSubresource, NULL);

			DXGI_DDI_ARG_PRESENT presentArg;
			presentArg.hDevice = (UINT_PTR)m_pDeviceWrapper->GetHandle().pDrvPrivate;
			presentArg.hSurfaceToPresent = (UINT_PTR)hDstResource.pDrvPrivate;	
			presentArg.SrcSubResourceIndex = 0;
			presentArg.hDstResource = 0;
			presentArg.DstSubResourceIndex = 0;
			presentArg.pDXGIContext = m_PresentData.pDXGIContext;
			presentArg.Flags.Value = 2;
			presentArg.FlipInterval =  DXGI_DDI_FLIP_INTERVAL_ONE;
			m_pDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnPresent(&presentArg);

			DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES rotateData;
			rotateData.hDevice = (UINT_PTR)m_pDeviceWrapper->GetHandle().pDrvPrivate;
			rotateData.pResources = &m_ResourceIdentities.front();
			rotateData.Resources = m_ResourceIdentities.size();
			m_pDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnRotateResourceIdentities(&rotateData);
		}
	}
	else
	{
		NSCALL(m_pSwapChainWrapper->m_pSwapChain->Present( 1, 0 ));
	}
	*/
	
	NSCALL(m_pSwapChainWrapper->m_pSwapChain->Present( 1, 0 ));

	m_pSwapChainWrapper->m_bPresenterCall = false;
	m_pSwapChainWrapper->m_bPresenterThreadId = 0;

	m_SuperSynchronizer.AfterPresent();
	LARGE_INTEGER* pVSyncTime = m_SuperSynchronizer.GetVSyncTime();
	PH_SetVSyncTime(pVSyncTime);

	m_pDeviceWrapper->m_pOutputMethod->AfterPresent(m_pDeviceWrapper, ViewIndx == 0);

	switch(gInfo.PresenterSleepTime)
	{
	case -2:
		{
			m_SuperSynchronizer.AfterPresentOne();
#ifndef FINAL_RELEASE
			LARGE_INTEGER CurrentTime = *pVSyncTime;
			LARGE_INTEGER Diff;
			Diff.QuadPart	= CurrentTime.QuadPart - m_LastFrameTime.QuadPart;
			float frameTime	= 1000.0f * Diff.QuadPart/m_Frequency.QuadPart;
			DXGI_RATIONAL refreshRate = m_AverageFPS.GetRefreshRate();
			float vblankTime = 1000.0f * refreshRate.Denominator / refreshRate.Numerator;
			if (frameTime / vblankTime > 1.5f) // lag happens
			{
				//ctdata.maxTime += (DWORD)(m_Frequency.QuadPart / 10);
			}
#endif
		}
		break;
	case -3:
		m_SuperSynchronizer.AfterPresentTwo();
		break;
	case -4:
		break;
	}
	PH_AfterPresent();

	CalculateFPS();

	m_hrPresent = hResult;
}

//////////////////////////////////////////////////////////////////////////

CPresenterX*	CPresenterX::m_pPresenterX = NULL;

CPresenterX::CPresenterX( )
	: CBasePresenterX	( )
	, m_bReadScanLineSupported( false )
	, m_PresentedPairIndex( 0 )
	, m_RenderingPairIndex( 0 )
	, m_VSyncCount		( 0 )
	, m_hrPresent		( S_OK )
{
	const D3D10_DDI_BOX zerobox = { 0 };
	m_SourceRect[0]	= zerobox;
	m_SourceRect[1]	= zerobox;
	m_DestRect		= zerobox;
	m_Frequency.QuadPart = 0;
	m_hSrcResourceLeft.pDrvPrivate = NULL;
	m_hSrcResourceRight.pDrvPrivate = NULL;
}

CPresenterX::~CPresenterX()
{
	CleanupBackground();
}

void CPresenterX::CalculateFPS()
{
	if( !m_Frequency.QuadPart )						// first time initialize
	{
		QueryPerformanceFrequency( &m_Frequency );
		QueryPerformanceCounter( &m_LastFrameTime );
		DEBUG_MESSAGE(_T("Updated SwapChain refresh rate\n"));
		m_AverageFPS.SetRefreshRate( m_RefreshRate );
	}
	else
	{
		LARGE_INTEGER	CurrentTime = *m_SuperSynchronizer.GetVSyncTime();
		m_FrameTime.QuadPart	= CurrentTime.QuadPart - m_LastFrameTime.QuadPart;
		m_LastFrameTime			= CurrentTime;
		float	frametime		= 1000.0f * m_FrameTime.QuadPart/m_Frequency.QuadPart; 
		m_AverageFPS.PushFrameTime( frametime );
	}
	++m_FrameNumber;
}

void	CPresenterX::Init( D3DDeviceWrapper* pDeviceWrapper_ )
{
	CBasePresenterX::Init( pDeviceWrapper_ );
}

void CPresenterX::InitResources()
{
	if ( m_hSrcResourceLeft.pDrvPrivate )
		return;

	_ASSERT( m_pSwapChainWrapper );
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= m_pDeviceWrapper->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& GET_ORIG11	= m_pDeviceWrapper->OriginalDeviceFuncs11;
	D3D10DDI_HDEVICE hDevice			= m_pDeviceWrapper->hDevice;
	SwapChainResources* pRes			= m_pSwapChainWrapper->GetCurrentPrimaryResource();

	ResourceWrapper* pWrp				= pRes->GetBackBufferPrimaryWrap();
	D3D10DDI_HRTRESOURCE  hRTResource	= { NULL };

	D3D11DDIARG_CREATERESOURCE CreateResource;
	CreateResource = pWrp->m_CreateResource11;
	CreateResource.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET;
	CreateResource.SampleDesc.Count = 1;
	CreateResource.SampleDesc.Quality = 0;
	CreateResource.pPrimaryDesc = NULL;

	m_pDeviceWrapper->CreateResource(m_hSrcResourceLeft, &CreateResource, pWrp->m_hRTResource);
	m_pDeviceWrapper->CreateResource(m_hSrcResourceRight, &CreateResource, pWrp->m_hRTResource);
}

void		CPresenterX::CleanupBackground			()
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= m_pDeviceWrapper->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= m_pDeviceWrapper->hDevice;

	// delete "shared" resources
	if( m_hSrcResourceLeft.pDrvPrivate )
	{
		GET_ORIG.pfnDestroyResource( hDevice, m_hSrcResourceLeft );
		DELETE_HANDLE( m_hSrcResourceLeft );
	}

	if( m_hSrcResourceRight.pDrvPrivate )
	{
		GET_ORIG.pfnDestroyResource( hDevice, m_hSrcResourceRight );
		DELETE_HANDLE( m_hSrcResourceRight );
	}
}

void CPresenterX::Create( DXGI_RATIONAL refreshRate )
{
	if( m_pPresenterX || !USE_UM_PRESENTER )
		return;

	m_pPresenterX = DNew CPresenterX();
	m_pPresenterX->SetRefreshRate( refreshRate );
	m_pPresenterX->SetActionAndWait( ptInitialize );	
}

void CPresenterX::Delete()
{
	if( m_pPresenterX )
	{
		m_pPresenterX->SetActionAndWait( ptClear );
		SAFE_DELETE( m_pPresenterX );
	}
}

//////////////////////////////////////////////////////////////////////////