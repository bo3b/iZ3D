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
#include "PresenterMultiThread.h"
#include "BaseStereoRenderer.h"

PresenterMultiThreadSCData::PresenterMultiThreadSCData(  PresenterMultiThread* pPresenter, CBaseSwapChain* pSwapChain ) 
:	CPresenterSCData( pPresenter, pSwapChain )
{
}

HRESULT PresenterMultiThreadSCData::UpdateSharedRenderTextures( HANDLE* pShared )
{
	_ASSERT( pShared );
	HRESULT	hr = S_OK;

 	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i )
 		m_SharedHandle[i] = pShared[i];
// 	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
// 		SAFE_RELEASE( m_pSharedTexture[ i ] );
	for( DWORD i = 0; i<SHARED_TEXTURE_COUNT; ++i ) 
	{
		m_pSharedTexture[ i ] = m_pBaseSwapChain->m_pSharedTexture[ i ];
		m_pSharedTexture[ i ]->AddRef();
		m_pSharedTexture[ i ]->GetSurfaceLevel(0, &m_pSharedSurface[ i ]);

		m_pViewSurface[ i ] = m_pSharedSurface[ i ];
	}

	return	hr;
}

void PresenterMultiThreadSCData::RenderBackground			()
{
	HRESULT	hResult	= S_OK;
	//if (SkipRender())
	//	return;
#if !defined( USE_PRESENTER_ACTIVE_WAIT )
	int sleepTime = GetSleepTime();

	if (sleepTime >= 0)
		Sleep(sleepTime);
#endif
	CBaseStereoRenderer* pStereoRenderer = m_pPresenter->m_pStereoRenderer;
	PH_AfterSleep();
	bool bLeft	= (m_FrameCount & 0x01) == 0;
	NSCALL(pStereoRenderer->m_pOutputMethod->Output(bLeft, m_pBaseSwapChain, NULL));

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
			unsigned 	VSYNC_SCAN_LINES  =	SCAN_LINES_PER_MS>>1;	// 0.5 mS

			// CMO 3D display 1920x1080 100+ Hz 3D mode optimization
			if( (m_RefreshRate > 99) &&
				(m_D3DPresentParameters.BackBufferWidth  == 1920) &&
				(m_D3DPresentParameters.BackBufferHeight == 1080) )
			{
				// Tweak timing parameters for 100 Hz 3D mode with pixel ID:
				// The EDID with pixel ID mechanism is	(1920 + 160)*(1080 +  500), pixel clock: 328.64 MHz
				// without pixel ID mechanism is		(1920 +  70)*(1080 +  578), pixel clock: 329.94 MHz
				const float fVBlankTime	= 0.520f;					// milliseconds
				SCAN_LINES_PER_MS =	(unsigned)((m_D3DPresentParameters.BackBufferHeight+500) / (1000.0f/(float)m_RefreshRate - fVBlankTime));
				VSYNC_SCAN_LINES  =	SCAN_LINES_PER_MS>>1;	// 0.5 mS
			}

			do {
				NSCALL(m_pDev9->GetRasterStatus( 0, &rStatus ));
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

				NSCALL(m_pDev9->ColorFill( pBackBuffer, &markRect, markColor[ m_VSyncCount & 1 ] ));
#endif

				if( rStatus.InVBlank ) break;
				if( rStatus.ScanLine < (m_D3DPresentParameters.BackBufferHeight - SCAN_LINES_PER_MS - VSYNC_SCAN_LINES) )
					Sleep( 1 );
 				else
 					Sleep( 0 );

			} while( rStatus.ScanLine <(m_D3DPresentParameters.BackBufferHeight - VSYNC_SCAN_LINES) );
			break;
		}
	} while( 0 );
#else
    // non-active wait
	// Sleep( 8 );
#endif

	PH_BeforePresent();
	NSCALL(m_hrPresent = CallPresent(true));
	m_pPresenter->m_SuperSynchronizer.AfterPresent();
	PH_SetVSyncTime(m_pPresenter->m_SuperSynchronizer.GetVSyncTime());
	if (gInfo.PresenterSleepTime == -2)
		m_pPresenter->m_SuperSynchronizer.AfterPresentOne();
	PH_AfterPresent(/*itCount*/);
	pStereoRenderer->m_pOutputMethod->AfterPresent(bLeft);

// #ifndef USE_PRESENTER_ACTIVE_WAIT
// 	m_ddSyncronizer.WaitForNonVBlank();
// #endif

	//NSCALL(m_hrPresent = m_pSwapChain.Present(NULL, NULL, NULL, NULL, 0));
	//NSCALL(m_hrPresent = m_pDev9->PresentEx(NULL, NULL, NULL, NULL, 0));
	if (SUCCEEDED(m_hrPresent))
	{
		//_ASSERT( S_OK == hrPresent_ );
	}

	CalculateFPS();
}

PresenterMultiThread::PresenterMultiThread( CBaseStereoRenderer* pStereoRenderer, IDirect3D9* pD3D9, UINT adapter, HWND hWnd ) 
:	CPresenter( pStereoRenderer, pD3D9, adapter, hWnd )
{
	DEBUG_MESSAGE(_T("Initialized multi-threaded presenter\n"));
}

void PresenterMultiThread::SetPresentationParameters( D3DPRESENT_PARAMETERS* params )
{
	m_D3DPresentParameters = *params;

	// DON'T turn off automatic depth stencil surface creation!
	//m_D3DPresentParameters.EnableAutoDepthStencil = FALSE;
	//m_D3DPresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	//m_D3DPresentParameters.Flags &= ~D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	//m_D3DPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	//m_D3DPresentParameters.MultiSampleQuality = 0;

	m_D3DPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_D3DPresentParameters.BackBufferCount = 2; // Triple buffering
	//m_D3DPresentParameters.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
#ifdef USE_PRESENTER_ACTIVE_WAIT
	m_D3DPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE /*D3DPRESENT_INTERVAL_IMMEDIATE*/;
#else
	m_D3DPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#endif

	if (!params->Windowed)
		m_D3DPresentParameters.FullScreen_RefreshRateInHz = m_pStereoRenderer->GetShutterRefreshRate(m_D3DPresentParameters.FullScreen_RefreshRateInHz);
}

void PresenterMultiThread::ResetDevice()
{
	HRESULT hResult = S_OK;
	D3DDISPLAYMODEEX DisplayMode;
	D3DDISPLAYMODEEX* pDisplayMode = PrepareDisplayMode(DisplayMode);					// N.B. it calls CreatePresenterWindow()!
	//NSCALL_TRACE1(m_pDev9->ResetEx(&m_D3DPresentParameters, pDisplayMode),
	NSCALL_TRACE1(m_pDev9->Reset(&m_D3DPresentParameters ),
		DEBUG_MESSAGE(_T("Presenter::Reset()")));
	m_hrReset = hResult;
	Initialize();
}

HRESULT PresenterMultiThread::CreateD3D9Device		()
{
	HRESULT	hResult		= S_OK;
	m_hrReset = 0xFFFFFFFF;

	// Create a D3D9 device
	DWORD	dwFlags	= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;									// N.B. MULTITHREADED !!!
	// D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_ENABLE_PRESENTSTATS;

	if( m_pStereoRenderer->m_Direct3DDevice.getExMode() == EXMODE_EX )
	{
		dwFlags |=  D3DCREATE_ENABLE_PRESENTSTATS;
	}

	// DEBUG ONLY!
	//_ASSERT( false );
	D3DDISPLAYMODEEX	DisplayMode = { 0 };
	D3DDISPLAYMODEEX*	pDisplayMode = PrepareDisplayMode(DisplayMode);													// N.B. it calls CreatePresenterWindow()! 

	//CComPtr<IDirect3D9Ex>	pD3D;
	//if( FAILED( Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D)))
	//	return E_FAIL;

	NSCALL(m_pD3D->CreateDevice(
	//NSCALL(m_pStereoRenderer->m_pDirect3D->CreateDevice(
		m_pStereoRenderer->m_nAdapter[0] /*D3DADAPTER_DEFAULT*/, 
		D3DDEVTYPE_HAL, 
		m_hWnd,
		dwFlags,
		&m_D3DPresentParameters,
		//pDisplayMode,	
		&m_pDev9
		));

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

	Initialize();

	return hResult;
}

PresenterMultiThread*	PresenterMultiThread::CreatePresenterMultiThread(
	CBaseStereoRenderer*		pStereoRenderer,
	IDirect3D9*					pD3D9,
	UINT						adapter,
	HWND						hFocusWindow
)
{
	if( 0 == m_NumCopies )
	{
		PresenterMultiThread*	retValue = DNew PresenterMultiThread( pStereoRenderer, 
			pD3D9, adapter, hFocusWindow );
		++m_NumCopies;
		return	retValue;
	}
	return	NULL;
}

CBasePresenterSCData* PresenterMultiThread::CreateSwapChainData( CBaseSwapChain* pSwapChain )
{
	return DNew PresenterMultiThreadSCData( this, pSwapChain);
}
