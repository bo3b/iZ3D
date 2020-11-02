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
#include "BackgroundPresenterThread.h"
#include "BaseStereoRenderer.h"
#include "WDirect3DCreate9.h"

BackgroundPresenterThreadSCData::BackgroundPresenterThreadSCData( BackgroundPresenterThread* pPresenter, CBaseSwapChain* pSwapChain ) 
:	CPresenterSCData( pPresenter, pSwapChain )
,	m_pStagingSurface(NULL)
{
}

HRESULT BackgroundPresenterThreadSCData::UpdateSharedRenderTextures( HANDLE* pShared )
{
	_ASSERT( pShared );

	HRESULT	hr = S_OK;

	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i )
		m_SharedHandle[i] = pShared[i];
	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
	{
		SAFE_RELEASE( m_pSharedTexture[ i ] );
		SAFE_RELEASE( m_pSharedSurface[ i ] );
		SAFE_RELEASE( m_pViewSurface[ i ] );
	}

	return	hr;
}

HRESULT BackgroundPresenterThreadSCData::CreateSharedRenderTextures()
{
	HRESULT hResult = S_OK;
	// create shared textures
	for( DWORD	i = 0; i<SHARED_TEXTURE_COUNT; ++i	)
	{
		NSCALL(GetBackgroundPresenter()->m_pDev9Ex->CreateTexture(
			m_pBaseSwapChain->m_BackBufferSize.cx,
			m_pBaseSwapChain->m_BackBufferSize.cy,
			1,
			D3DUSAGE_RENDERTARGET,
			m_pPresenter->m_D3DPresentParameters.BackBufferFormat /*D3DFMT_X8R8G8B8*/,
			D3DPOOL_DEFAULT,
			&m_pSharedTexture	[i],
			&m_SharedHandle		[i] ));
		// sometimes fail
		if( FAILED(hResult) )
			return E_FAIL;

		NSCALL(m_pSharedTexture[ i ]->GetSurfaceLevel(0, &m_pSharedSurface[ i ]));
		if( FAILED(hResult) )
			return E_FAIL;

		NSCALL(GetBackgroundPresenter()->m_pDev9Ex->CreateRenderTarget(
			m_pBaseSwapChain->m_BackBufferSize.cx,
			m_pBaseSwapChain->m_BackBufferSize.cy,
			m_pPresenter->m_D3DPresentParameters.BackBufferFormat /*D3DFMT_X8R8G8B8*/,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pViewSurface	[i],
			NULL ));
		if( FAILED(hResult) )
			return E_FAIL;
	}

	NSCALL(GetBackgroundPresenter()->m_pDev9Ex->CreateRenderTarget(
		16,
		16,
		m_pPresenter->m_D3DPresentParameters.BackBufferFormat /*D3DFMT_X8R8G8B8*/,
		D3DMULTISAMPLE_NONE,
		0,
		FALSE,
		&m_pStagingSurface,
		NULL ));
	if( FAILED(hResult) )
		return E_FAIL;

	return S_OK;
}

void BackgroundPresenterThreadSCData::Cleanup()
{
	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
		SAFE_RELEASE(	m_pViewSurface[ i ] );
	SAFE_RELEASE(m_pStagingSurface);

	__super::Cleanup();
}

void BackgroundPresenterThreadSCData::RenderBackground()
{
	INSIDE;
	HRESULT	hResult	= S_OK;
	//if (SkipRender())
	//	return;
	CBaseStereoRenderer* pStereoRenderer = m_pPresenter->m_pStereoRenderer;
	PH_AfterSleep();

	bool bLeft	= (m_FrameCount & 0x01) == 0;
	NSCALL(GetBackgroundPresenter()->m_pDev9Ex->StretchRect( m_pSharedSurface[ 2 * m_PresentedPairIndex + (bLeft ? 0 : 1) ],  NULL, 
		m_pBaseSwapChain->GetViewRT(bLeft),  NULL, D3DTEXF_NONE ));

	NSCALL(pStereoRenderer->m_pOutputMethod->Output(bLeft, m_pBaseSwapChain, NULL));

	RECT rect = { 0, 0, 16, 16 };
	NSCALL(GetBackgroundPresenter()->m_pDev9Ex->StretchRect( m_pBaseSwapChain->GetViewRT(bLeft),  &rect, 
		m_pStagingSurface,  &rect, D3DTEXF_NONE ));

	// Attempt to lock the staging surface to see if the rendering
	// is complete.
	D3DLOCKED_RECT lockedRect;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	DWORD freqMS = (DWORD)(freq.QuadPart / 1000);
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	DWORD ms = 0;
	while ((hResult = m_pStagingSurface->LockRect(&lockedRect, NULL, D3DLOCK_DONOTWAIT | D3DLOCK_READONLY)) == D3DERR_WASSTILLDRAWING)
	{
		LARGE_INTEGER stop;
		QueryPerformanceCounter(&stop);
		ms = (DWORD)((stop.QuadPart - start.QuadPart) / freqMS);
		if (ms >= 5)
		{
			DEBUG_MESSAGE(_T("LockRect too long\n"));
			break;
		}
	}
	if (SUCCEEDED(hResult))
		hResult = m_pStagingSurface->UnlockRect();

#if !defined( USE_PRESENTER_ACTIVE_WAIT )
	int sleepTime = GetSleepTime() - ms;

	if (sleepTime >= 0)
		Sleep(sleepTime);
#endif

#ifdef USE_PRESENTER_ACTIVE_WAIT
	// Wait for V-Sync
	do
	{
		D3DRASTER_STATUS	rStatus;
		if( m_bReadScanLineSupported )				// If polling VSync is available
		{
			// V-Blank time is about 600 uS (0.6 mS):
			//---------------------------------------
			// V.Front Porch		~  30 uS
			// V.Sync				~  60 uS
			// V.Back Porch			~ 500 uS (0.5 mS)

			// Wait for non V-Blank!
			do {
				NSCALL(m_pDev9->GetRasterStatus( 0, &rStatus ));
				_ASSERT( hResult == S_OK );
				if( rStatus.InVBlank )
				{
					Sleep( 1 );
					//break;											// go out here after the sleep
				}
			} while( rStatus.InVBlank );

			// Wait for last VSYNC_SCAN_LINES lines
			const float	fScanLinesRange	  =	1.0f / 64.0f;			// 16 scan lines for HRes == 1024
			const float fVBlankTime		  = 0.635f;					// milliseconds
			//unsigned 	VSYNC_SCAN_LINES  =	(unsigned)(m_D3DPresentParameters.BackBufferHeight * fScanLinesRange);
			unsigned	SCAN_LINES_PER_MS =	(unsigned)(m_D3DPresentParameters.BackBufferHeight / (1000.0f/(float)m_RefreshRate - fVBlankTime));
			unsigned 	VSYNC_SCAN_LINES  =	SCAN_LINES_PER_MS>>2;	// 0.25 mS

			// CMO 3D display 1920x1080 100+ Hz 3D mode optimization
			if( (m_RefreshRate > 99) &&
				(m_D3DPresentParameters.BackBufferWidth  == 1920) &&
				(m_D3DPresentParameters.BackBufferHeight == 1080) )
			if( (m_RefreshRate == 100) 
				//&& (m_D3DPresentParameters.BackBufferWidth  == 1920) 
				//&& (m_D3DPresentParameters.BackBufferHeight == 1080)
			)
			{
				// Tweak timing parameters for 100 Hz 3D mode with pixel ID:
				// The EDID with pixel ID mechanism is	(1920 + 160)*(1080 +  500), pixel clock: 328.64 MHz
				// without pixel ID mechanism is		(1920 +  70)*(1080 +  578), pixel clock: 329.94 MHz
				const float fVBlankTime	= 0.500f;					// milliseconds
				SCAN_LINES_PER_MS =	(unsigned)((m_D3DPresentParameters.BackBufferHeight*1.5) / (1000.0f/(float)m_RefreshRate - fVBlankTime));
				VSYNC_SCAN_LINES  =	SCAN_LINES_PER_MS>>4;			// 0.25 mS
			}

			do {
				NSCALL(m_pDev9Ex->GetRasterStatus( 0, &rStatus ));
				_ASSERT( hResult == S_OK );

#ifdef	DEBUG_SHOW_WAIT_LINES
				// DEBUG only!
				const	D3DCOLOR	markColor[2] = {
					D3DCOLOR_XRGB( 0xFF, 0x3F, 0x3F ),
					D3DCOLOR_XRGB( 0x3F, 0xFF, 0x3F )
				};

				RECT		markRect;								// LONG    left, top, right, bottom;
				markRect.left	= 0;
				markRect.right	= m_D3DPresentParameters.BackBufferWidth;
				markRect.top	= rStatus.ScanLine;
				markRect.bottom = rStatus.ScanLine + 1;

				NSCALL(m_pDev9Ex->ColorFill( pBackBuffer, &markRect, markColor[ m_VSyncCount & 1 ] ));
#endif

				if( rStatus.InVBlank ) break;
				if( rStatus.ScanLine < (m_D3DPresentParameters.BackBufferHeight - SCAN_LINES_PER_MS) )
					Sleep( 1 );
 				else
 					Sleep( 0 );

			} while( rStatus.ScanLine <(m_D3DPresentParameters.BackBufferHeight - VSYNC_SCAN_LINES) );
			break;
		}
	} while( 0 );
#endif

	PH_BeforePresent();
	NSCALL(m_hrPresent = CallPresent(true));
	m_pPresenter->m_SuperSynchronizer.AfterPresent();
	PH_SetVSyncTime(m_pPresenter->m_SuperSynchronizer.GetVSyncTime());
	if (gInfo.PresenterSleepTime == -2)
		m_pPresenter->m_SuperSynchronizer.AfterPresentOne();
	PH_AfterPresent(/*itCount*/);
	pStereoRenderer->m_pOutputMethod->AfterPresent(bLeft);
	CalculateFPS();
}

BackgroundPresenterThread::BackgroundPresenterThread( CBaseStereoRenderer* pStereoRenderer, IDirect3D9Ex* pD3D9, UINT adapter, HWND hWnd ) 
:	CPresenter( pStereoRenderer, pD3D9, adapter, hWnd )
,	m_pD3DEx	( pD3D9 )
,	m_pDev9Ex	( NULL )
{
	DEBUG_MESSAGE(_T("Initialized multi-device presenter\n"));
}

void BackgroundPresenterThread::SetPresentationParameters( D3DPRESENT_PARAMETERS* params )
{
	m_D3DPresentParameters = *params;

	// turn off automatic depth stencil surface creation
	m_D3DPresentParameters.EnableAutoDepthStencil = FALSE;
	m_D3DPresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	m_D3DPresentParameters.Flags &= ~D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	m_D3DPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_D3DPresentParameters.MultiSampleQuality = 0;

	m_D3DPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3DPresentParameters.BackBufferCount = 2; // Triple buffering
#ifdef USE_PRESENTER_ACTIVE_WAIT
	m_D3DPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE /*D3DPRESENT_INTERVAL_IMMEDIATE*/;
#else
	m_D3DPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif

	if (!params->Windowed)
		m_D3DPresentParameters.FullScreen_RefreshRateInHz = m_pStereoRenderer->GetShutterRefreshRate(m_D3DPresentParameters.FullScreen_RefreshRateInHz);
}

void BackgroundPresenterThread::ResetDevice()
{
	D3DDISPLAYMODEEX DisplayMode;
	HRESULT hResult = S_OK;
	D3DDISPLAYMODEEX* pDisplayMode = PrepareDisplayMode(DisplayMode);
	NSCALL_TRACE1(m_pDev9Ex->ResetEx(&m_D3DPresentParameters, pDisplayMode),
		DEBUG_MESSAGE(_T("Presenter::ResetEx()")));
	m_hrReset = hResult;
	Initialize();
}

HRESULT BackgroundPresenterThread::CreateD3D9Device		()
{
	HRESULT	hResult		= S_OK;
	m_hrReset = 0xFFFFFFFF;

	// Create a D3D9Ex device
	DWORD	dwFlags	= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_ENABLE_PRESENTSTATS | D3DCREATE_MULTITHREADED;
	// D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_ENABLE_PRESENTSTATS;

	// DEBUG ONLY!
	//_ASSERT( false );
	D3DDISPLAYMODEEX	DisplayMode = { 0 };
	D3DDISPLAYMODEEX*	pDisplayMode = PrepareDisplayMode(DisplayMode);

	pfDirect3DCreateEx pDirect3DCreate9Ex = GetD3D9CreatorEx(); 
	if(!pDirect3DCreate9Ex)
	{
		m_hrReset = E_FAIL;
		return m_hrReset;
	}

	CComPtr<IDirect3D9Ex>	pD3D;
	if( FAILED( pDirect3DCreate9Ex(D3D_SDK_VERSION, &pD3D )))
	{
		m_hrReset = E_FAIL;
		return m_hrReset;
	}

	NSCALL(pD3D->CreateDeviceEx(
		m_pStereoRenderer->m_nAdapter[0] /*D3DADAPTER_DEFAULT*/, 
		D3DDEVTYPE_HAL, 
		m_hWnd,
		dwFlags,
		&m_D3DPresentParameters,
		pDisplayMode,	// pFullscreenDisplayMode
		&m_pDev9Ex
	));
	m_pDev9 = m_pDev9Ex;

	if( m_pDev9 == NULL )
	{
		m_hrReset = E_FAIL;
		return m_hrReset;
	}

	while(hResult == D3DERR_DEVICELOST)
	{
		DEBUG_MESSAGE(_T("%s: D3DERR_DEVICELOST. Trying to Reset.\n"), _T( __FUNCTION__ ) );
		hResult = m_pDev9->TestCooperativeLevel();
	}
	if (hResult == D3DERR_DEVICELOST)
	{
		DEBUG_MESSAGE(_T("%s: D3DERR_DEVICENOTRESET\n"), _T( __FUNCTION__ ) );
		hResult = m_pDev9->Reset(&m_D3DPresentParameters);
	}

	m_hrReset = hResult;

	_ASSERT(hResult == S_OK);

	CPresenterSCData* pData = (CPresenterSCData*)(m_pStereoRenderer->GetBaseSC()->m_pPresenterData);
	pData->m_pSwapChain = &m_pStereoRenderer->GetBaseSC()->m_SwapChain;

	// Set the GPU thread priority
	m_pDev9Ex->SetGPUThreadPriority( PRESENTER_GPU_PRIORITY );			// -7 == LOWEST, 0 == NORMAL, +7 == HIGEST

	Initialize();

	return hResult;
}

BackgroundPresenterThread*	BackgroundPresenterThread::CreateBackgroundPresenterThread(
	CBaseStereoRenderer*		pStereoRenderer,
	IDirect3D9Ex*				pD3D9,
	UINT						adapter,
	HWND						hFocusWindow
)
{
	BackgroundPresenterThread*	retValue = DNew BackgroundPresenterThread( pStereoRenderer, 
		pD3D9, adapter, hFocusWindow );
	++m_NumCopies;
	return	retValue;
}

CBasePresenterSCData* BackgroundPresenterThread::CreateSwapChainData( CBaseSwapChain* pSwapChain )
{
	return DNew BackgroundPresenterThreadSCData( this, pSwapChain);
}
