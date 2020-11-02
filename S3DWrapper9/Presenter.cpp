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
#include <fstream>
#include <intrin.h>
#include "Presenter.h"
#include "BaseStereoRenderer.h"

size_t						CPresenter::m_NumCopies			= 0;

CPresenterSCData::CPresenterSCData( CPresenter* pPresenter, CBaseSwapChain* pSwapChain ) 
:	CBasePresenterSCData( pPresenter, pSwapChain )
,	m_pPresenter( pPresenter )
,	m_pSwapChain( 0 )
,	m_FrameCount( 0 )
,	m_VSyncCount( 0 )
,	m_PresentedPairIndex( 0 )
,	m_RenderingPairIndex( 1 % SHARED_TEXTURE_PAIR_COUNT )
{
	m_FrameTime.QuadPart = 0;
	m_LastFrameTime.QuadPart = 0;
	m_RenderingTime.QuadPart = 0;

	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
	{
		m_SharedHandle[i]	= NULL;
		m_pSharedTexture[i]	= NULL;
		m_pSharedSurface[i]	= NULL;
		m_pViewSurface[i]	= NULL;
	}
}

CPresenterSCData::~CPresenterSCData()
{
	SAFE_RELEASE(m_pSwapChain);
}

void CPresenterSCData::Initialize()
{
	//SetForegroundWindow(m_pStereoRenderer->GetBaseSC()->GetAppWindow());
	HRESULT hResult = m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pPrimaryBackBuffer );
	m_FrameTime.QuadPart = m_pPresenter->m_pStereoRenderer->m_nFreq.QuadPart / m_pPresenter->m_RefreshRate;
	m_FrameCount = 0;
	m_VSyncCount = 0;
	m_LastFrameTime.QuadPart = 0;
	m_RenderingTime.QuadPart = 0;
	CreateSharedRenderTextures();
}

void CPresenterSCData::Cleanup()
{
	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
	{
		SAFE_RELEASE(	m_pSharedTexture[ i ] );
		SAFE_RELEASE(	m_pSharedSurface[ i ] );
		m_pViewSurface[ i ] = NULL;
		m_SharedHandle[ i ] = NULL;
	}
	m_pPrimaryBackBuffer.Release();
}

void CPresenterSCData::SetPresentPairIndex( long ind )
{
	_ASSERT( ind < SHARED_TEXTURE_PAIR_COUNT );
	_InterlockedExchange(&m_PresentedPairIndex,	ind);
}

DWORD CPresenterSCData::GetPresentPairIndex( )
{
	return	m_PresentedPairIndex;
}

DWORD CPresenterSCData::GetRenderingPairIndex( )
{
	m_RenderingPairIndex++;
	if (m_RenderingPairIndex >= SHARED_TEXTURE_PAIR_COUNT)
		m_RenderingPairIndex = 0;
	return	m_RenderingPairIndex;
}

HRESULT CPresenterSCData::WaitForVBlank	( )
{
	UINT SwapChainIndx = 0;
	D3DRASTER_STATUS	rStatus;
	HRESULT				hResult;

	if( m_pPresenter->m_bReadScanLineSupported )				// If polling VSync is available
	{
		do {
			NSCALL(m_pPresenter->m_pDev9->GetRasterStatus( SwapChainIndx, &rStatus ));
			_ASSERT( hResult == S_OK );
		} while( !rStatus.InVBlank );
		return	S_OK;
	}

	return	E_FAIL;												// Can't read the scan line
}

HRESULT CPresenterSCData::CallPresent(bool bExMode)
{
	HRESULT hPresent = S_OK;
	if (m_pBaseSwapChain->m_bUseSwapChains)
	{
		hPresent = m_pSwapChain->Present(m_pBaseSwapChain->m_pSourceRect, m_pBaseSwapChain->m_pDestRect, 
			m_pBaseSwapChain->m_hDestWindowOverride, m_pBaseSwapChain->m_pDirtyRegion, 0);
	}
	else
	{
		//cppcheck warns of unused variable
		//bool bFullscreen = !m_pBaseSwapChain->m_PresentationParameters[0].Windowed;
		if (!bExMode)
		{
			hPresent = m_pPresenter->m_pDev9->Present(m_pBaseSwapChain->m_pSourceRect, m_pBaseSwapChain->m_pDestRect, 
				0, m_pBaseSwapChain->m_pDirtyRegion);
		}
		else
		{
			CComQIPtr<IDirect3DDevice9Ex> pD3D9DevEx = m_pPresenter->m_pDev9;
			hPresent = pD3D9DevEx->PresentEx(m_pBaseSwapChain->m_pSourceRect, m_pBaseSwapChain->m_pDestRect, 
				0, m_pBaseSwapChain->m_pDirtyRegion, m_pBaseSwapChain->m_dwFlags);
		}
	}
	return hPresent;
}

void CPresenterSCData::CalculateFPS()
{
	LARGE_INTEGER CurrentTime, Freq;
	Freq = m_pPresenter->m_pStereoRenderer->m_nFreq;
	if (m_LastFrameTime.QuadPart)
	{
		CurrentTime = *m_pBasePresenter->m_SuperSynchronizer.GetVSyncTime();
		LARGE_INTEGER Diff;
		Diff.QuadPart = CurrentTime.QuadPart - m_LastFrameTime.QuadPart;
		float	frametime = 1000.0f * Diff.QuadPart / Freq.QuadPart;
		m_AverageFPS.PushFrameTime( frametime );
		ULONG Frame = (ULONG)(m_RenderingTime.QuadPart / m_FrameTime.QuadPart);
		m_VSyncCount = m_FrameCount < 10 ? m_FrameCount : Frame;

		m_FrameCount++;
		m_RenderingTime.QuadPart += Diff.QuadPart;
	}
	else
	{
		WaitForVBlank();
		QueryPerformanceCounter(&CurrentTime);
		m_AverageFPS.SetRefreshRate( m_pPresenter->m_RefreshRate );	
#ifndef FINAL_RELEASE
		DEBUG_MESSAGE(_T("Acceptable FPS: "));
		for (int i = m_pPresenter->m_RefreshRate; i > 0; i--)
		{
			if (m_pPresenter->m_RefreshRate % i == 0) {
				DEBUG_MESSAGE(_T("%d; "), i);
			}
		}
		DEBUG_MESSAGE(_T("\n"));
#endif
	}
	m_LastFrameTime = CurrentTime;
}

CPresenter::CPresenter( CBaseStereoRenderer* pStereoRenderer, IDirect3D9* pD3D9, UINT adapter, HWND hWnd ) 
:	CBasePresenter(pStereoRenderer)
,	m_pD3D		( pD3D9 )
,	m_pDev9		( NULL )
,	m_hWnd		( hWnd )
,	m_hPresenterWnd	( 0 )
,	m_Adapter	( adapter )
,	m_bReadScanLineSupported( false )
{
}

CPresenter::~CPresenter()
{
	--m_NumCopies;
}

D3DDISPLAYMODEEX* CPresenter::PrepareDisplayMode( D3DDISPLAYMODEEX &DisplayMode )
{
	if( FALSE == m_D3DPresentParameters.Windowed )
	{
		// Let's create the new window for the fullscreen mode
		CreatePresenterWindow();
	}

	DisplayMode.Size				= sizeof( DisplayMode );
	DisplayMode.Width				= m_D3DPresentParameters.BackBufferWidth;
	DisplayMode.Height				= m_D3DPresentParameters.BackBufferHeight;
	DisplayMode.RefreshRate			= m_D3DPresentParameters.FullScreen_RefreshRateInHz; // PRESENTER_REFRESH_RATE;
	DisplayMode.Format				= m_D3DPresentParameters.BackBufferFormat;
	DisplayMode.ScanLineOrdering	= D3DSCANLINEORDERING_UNKNOWN;						// D3DSCANLINEORDERING_PROGRESSIVE;

	return m_D3DPresentParameters.Windowed ? NULL : &DisplayMode;
}

void CPresenter::Initialize()
{
	HRESULT hResult = S_OK;
	D3DCAPS9 D3DCaps;
	NSCALL( m_pD3D->GetDeviceCaps( m_Adapter, D3DDEVTYPE_HAL, &D3DCaps ) );
	m_bReadScanLineSupported = (D3DCaps.Caps & D3DCAPS_READ_SCANLINE) != 0;
	SetCurrentRefreshRate();
}

void CPresenter::DestroyFullscreenWindow()
{
	if( m_hPresenterWnd )
	{
		DestroyWindow( m_hPresenterWnd );
	}
}

HRESULT CPresenter::CreatePresenterWindow()
{
	const LPCWSTR	PRESENTER_WINDOW_CLASS_NAME = _T("CMOShutter");

	WNDCLASS	WindowClass;
	ZeroMemory( &WindowClass, sizeof( WindowClass ) );
	WindowClass.lpfnWndProc		= DefWindowProc;  
	WindowClass.hInstance		= GetModuleHandle(NULL);
	WindowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);   
	WindowClass.lpszClassName	= PRESENTER_WINDOW_CLASS_NAME;
	if( 0 == RegisterClass( &WindowClass ) )
	{
		//DWORD	error = GetLastError();
		//return	E_FAIL;
	}

	// second window must be synchronously minimized and maximized this main window 
	// HWND hParent = m_hFocusWindow ? m_hFocusWindow : D3DPresentParameters_.hDeviceWindow;
	DWORD	dwStyle = WS_POPUP;			// second window must be visible for swapchain drawing
	//if(m_DeviceMode == DEVICE_MODE_FORCEFULLSCREEN) 
	HWND	hParent = HWND_MESSAGE;

	m_hPresenterWnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, PRESENTER_WINDOW_CLASS_NAME, TEXT("S3D Driver Window"), 
		dwStyle, 0, 0, 1, 1, hParent, 0, WindowClass.hInstance , 0);

	//	m_S3DSecondWindow = CreateWindowEx(WS_EX_TOOLWINDOW, PRESENTER_WINDOW_CLASS_NAME, TEXT("S3D Driver Window"), 
	//		dwStyle, 0, 0, 1, 1, hParent, 0, (HINSTANCE)gInfo.hModule, 0);
	if( !m_hPresenterWnd )
	{
		DWORD	error = GetLastError();
		return	E_FAIL;
	}

	SetWindowLongPtr(m_hPresenterWnd, GWLP_USERDATA, (LONG_PTR)m_pStereoRenderer->GetBaseSC()->GetAppWindow()); // for PostMessage()

	m_hWnd = m_D3DPresentParameters.hDeviceWindow = m_hPresenterWnd;		// for CreateDeviceEx()
	return		S_OK;
}

//-----------------------------------------------------------------------------
// Provide for a method in (Windowed Mode) to poll the adapters V-Sync function.
// This is a requirement for frame synchronization process AND MUST
// be available for highest possible performance in windowed mode.
//-----------------------------------------------------------------------------
HRESULT CPresenter::D3DWindowedWaitForVsync	()
{
	D3DRASTER_STATUS	rStatus;
	HRESULT				hr = E_FAIL;

	if( m_bReadScanLineSupported )
	{
		// IF Polling VSync is available:
		hr = m_pDev9->GetRasterStatus( 0, &rStatus );
		if( hr == S_OK )
		{
			// In rare case when in vblank, Wait for vblank to drop.
			while( rStatus.InVBlank )
				m_pDev9->GetRasterStatus( 0, &rStatus );
			while( rStatus.ScanLine < (m_D3DPresentParameters.BackBufferHeight - 1) /*g_bottomLine*/ )	// FIXME!
			{
				m_pDev9->GetRasterStatus( 0, &rStatus );
				// if scan ever crossed vblank again; break. could be issue with window transitioning modes.
				if( rStatus.InVBlank ) break;
			}
		}
	}
	// IF Polling VSync is NOT available:
	if( hr != S_OK )
		Sleep( 10 );
	return S_OK;
}

DWORD WINAPI CPresenter::BackgroundThreadProc		()
{
	if (m_bEndThread)
		return 3;

	// Create a D3D9 device
	if( FAILED( CreateD3D9Device() ) )
	{
		CleanupBackground();
		return 1;
	}


	SuspendThread( ); // wait first data

	// MAIN LOOP
	while( !m_bEndThread )
	{
		switch(m_Action)
		{
		case ptInitialize:
			m_SuperSynchronizer.Initialize(m_RefreshRate, 3);
			_ASSERT(m_pStereoRenderer->m_pOutputMethod);
			m_pStereoRenderer->m_pOutputMethod->Initialize(m_pDev9, GET_DEBUG_VALUE(gInfo.TwoPassRendering) != 0);
			m_Action = ptNone;
			break;
		case ptInitializeSCData:
			m_pBaseSwapChain->m_pBasePresenterData->Initialize();
			m_Action = ptNone;
			break;
		case ptRender:
			if (m_pStereoRenderer->m_bEngineStarted)
				m_pStereoRenderer->GetBaseSC()->m_pBasePresenterData->RenderBackground();
			// Uncomment next line to throttle rendering to no more then the refresh rate of the monitor.
			// pDev9Ex_->WaitForVBlank( 0 );	
#if	defined( USE_PRESENTER_SLEEP ) && defined( PRESENTER_SLEEP_TIME )
			Sleep( PRESENTER_SLEEP_TIME );
#endif
			break;
		case ptReset:
			ResetDevice();
			m_Action = ptNone;
			break;
		case ptTestCooperativeLevel:
			TestCooperativeLevel();
			m_Action = ptNone;
			break;
		case ptClearSCData:
			m_pBaseSwapChain->m_pBasePresenterData->Cleanup();
			m_Action = ptNone;
			break;
		case ptClear:
			if ((gInfo.PresenterSleepTime == -2) || (gInfo.PresenterSleepTime == -3))
				m_SuperSynchronizer.Clear();
			_ASSERT(m_pStereoRenderer->m_pOutputMethod);
			m_pStereoRenderer->m_pOutputMethod->Clear();
			m_Action = ptNone;
			break;
		case ptReleaseSC:
			m_pBaseSwapChain->m_pPresenterData->DetachSwapChain();
			m_Action = ptNone;
			break;
		case ptReleaseDevice:
			// Cleanup
			CleanupBackground();
			m_Action = ptNone;
			break;
		default:
			SuspendThread( );
			break;
		}
	}

	return 0;
}

void CPresenter::TestCooperativeLevel()
{
	HRESULT hResult = S_OK;
	NSCALL_TRACE1(m_pDev9->TestCooperativeLevel(),
		DEBUG_MESSAGE(_T("Presenter::TestCooperativeLevel()")));
	m_hrReset = hResult;
}
