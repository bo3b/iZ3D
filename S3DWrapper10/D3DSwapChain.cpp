#include "StdAfx.h"
#include <iomanip>
#include <Shlobj.h>
#include "D3DSwapChain.h"
#include "ProductNames.h"
#include "Utils.h"
#include "D3DDeviceWrapper.h"
#include "../OutputMethods/OutputLib/OutputMethod_dx10.h"
#include "Presenter.h"
#include "../DX10SharedLibrary/WizardDrawer.h"
#include "..\S3DAPI\ReadData.h"
#include "..\S3DAPI\Utils.h"
#include "..\S3DAPI\ShutterAPI.h"
#include "../Shared/version.h"
#ifndef WIN64
	#ifdef _DEBUG
		#define DRIVER_VER _T("debug x32")
	#else
		#define DRIVER_VER _T("release x32")
	#endif
#else
	#ifdef _DEBUG
		#define DRIVER_VER _T("debug x64")
	#else
		#define DRIVER_VER _T("release x64")
	#endif
#endif

TLS IDXGISwapChain* g_pCurrentSwapChain = NULL;
TLS SwapChainState g_SwapChainMode = scNormal;

DWORD   WINAPI   S3DMessageQueue(LPVOID lParam);
LRESULT CALLBACK S3DWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#define  S3D_WINDOW_CLASS_NAME _T(PRODUCT_NAME L" Window Class")

D3DSwapChain::D3DSwapChain(D3DDeviceWrapper* pD3DDeviceWrapper)
:	m_pD3DDeviceWrapper(pD3DDeviceWrapper)
,	m_bS3DSecondWindowVisible(false)
,	m_bClassWasRegistered(false)
,	m_hS3DSecondWindowThread(NULL)
,	m_hS3DSecondWindow(NULL)
,	m_pDXGIFactory(NULL)
,	m_bLeftEyeActive( true )
,	m_nLineOffset( 0 )
,	m_LaserSightData	( true )
,	m_bPresenterCall	( false )
,	m_bResourceCopyRegionCalled( false )
,	m_bPresenterThreadId( 0 )
,	m_bUseSimplePresenter( true )
,	m_bUseKMShutterService( false )
,	m_bPresentationCall( false )
{
	m_pSwapChain = NULL;
	m_MultiSample.Count = 1;
	m_MultiSample.Quality = 0;
	m_nFrameTimeDelta.QuadPart		= 0;
	m_nLastFrameTime.QuadPart		= 0;
	m_nFPSDropShowTimeLeft.QuadPart	= 0;
	m_nFPSTimeDelta.QuadPart		= 0;
	m_nLastDropTime.QuadPart		= 0;
	m_nSessionFrameCount			= 0;
	m_fLastMonoFPS					= 0;
	m_LatestResourcePrimary			= m_ResourcesPrimary.end();

	m_hSecondWindowInitialized = CreateEvent(0,TRUE,FALSE,0);
	m_DstMonitorPoint.x = 0;
	m_DstMonitorPoint.y = 0;
}

void D3DSwapChain::SetDXGISwapChain( IDXGISwapChain* pSwapChain )
{
	_ASSERT(!m_pSwapChain);
	m_pSwapChain = pSwapChain;
	pSwapChain->GetDesc(&m_Description);
	m_MultiSample = m_Description.SampleDesc;
	SetBackBufferSize();	
}

void D3DSwapChain::InitAQBSData()
{
	m_nLineOffset = m_pD3DDeviceWrapper->AQBSGetLineOffset( this );
	DEBUG_TRACE1(_T("D3DSwapChain::InitAQBSData: aqbs line offset is: %d.\n"), m_nLineOffset );
}

void D3DSwapChain::InitializeSwapChains( )
{
	WE_CALL;

	PrimaryResourcesMap::iterator itf = m_LatestResourcePrimary;
	PrimaryResourcesMap::iterator itp = m_ResourcesPrimary.begin();
	for (; itp != m_ResourcesPrimary.end(); ++itp) // initialize all "backbuffers"
	{
		m_LatestResourcePrimary = itp;
		Initialize();
	}
	m_LatestResourcePrimary = itf;

	if (m_pD3DDeviceWrapper->m_bTwoWindows)
	{
		_ASSERT(m_pDXGIFactory);
		CComPtr<IDXGIOutput>	FirstOutput;
		CComPtr<IDXGIOutput>	SecondOutput;
		if (!GetOutputs( m_pDXGIFactory, &FirstOutput, &SecondOutput ))
		{
			m_pD3DDeviceWrapper->m_bTwoWindows = false;
			return;
		}

		InitWindows();

		// create a swap chain
		DXGI_SWAP_CHAIN_DESC SwapChainDesc = m_Description;
		SwapChainDesc.OutputWindow = m_hS3DSecondWindow;
		if (m_Description.Windowed)
		{
			RECT rcClient;
			GetSecondaryWindowRect(&m_S3DSecondWindowRect, &rcClient);
		}
		else
		{
			SetRect(&m_S3DSecondWindowRect, 
				0, 0, SwapChainDesc.BufferDesc.Width, SwapChainDesc.BufferDesc.Height);
		}
		SendOrPostMessage(SWM_MOVE_SECOND_WINDOW);
		HRESULT hResult;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		NSCALL(m_pDXGIFactory->CreateSwapChain( m_pD3DDeviceWrapper->m_pDXGIDevice, &SwapChainDesc, &m_pSecondSwapChain ));	

		CheckSecondWindowPosition();  // !m_Description.Windowed
	}
}

void D3DSwapChain::InitWindows()
{
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return;

	// thread and window created
	if (m_hS3DSecondWindow)
		return;

	DWORD currentThreadId = GetCurrentThreadId();
	DWORD windowThreadId = GetWindowThreadProcessId(m_Description.OutputWindow,0);
	DEBUG_MESSAGE(_T("** D3DSwapChain: D3D render thread id: %d\n"), m_pD3DDeviceWrapper->m_MainThreadId);
	DEBUG_MESSAGE(_T("** D3DSwapChain:       DXGI thread id: %d\n"), currentThreadId);
	DEBUG_MESSAGE(_T("** D3DSwapChain: output wnd thread id: %d\n"), windowThreadId);

	if (windowThreadId != currentThreadId)
	{
		DEBUG_MESSAGE(_T("D3DSwapChain: creating a separate thread for the second window\n"));

		// create thread, window class, window and run message queue
		m_hS3DSecondWindowThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned(__stdcall*)(void *))S3DMessageQueue, 
			(LPVOID)this, 0, NULL);

		if (m_hS3DSecondWindowThread == NULL)
		{
			ZLOG_MESSAGE(zlog::SV_FATAL_ERROR, _T("Can't initialize thread for second window"));
			return; // exit(-1) maybe ?)
		}

		WaitForSingleObject(m_hSecondWindowInitialized,INFINITE);
	}
	else
	{
		S3DMessageQueue(this);
	}
}

void D3DSwapChain::PrepareFPSMessage( D3DDeviceWrapper* pWrapper_ )
{
	m_nFPSTimeDelta.QuadPart += m_nFrameTimeDelta.QuadPart;
	m_nSessionFrameCount++;

	m_fFPS = m_nSessionFrameCount / ( 1.0 * m_nFPSTimeDelta.QuadPart / pWrapper_->m_nFreq.QuadPart );

	if ( !gInfo.ShowFPS )
		return;

	bool bCleared = false;
	if ( pWrapper_->m_bStereoModeChanged ||
		m_nLastFrameTime.QuadPart > m_nLastDropTime.QuadPart + pWrapper_->m_nFreq.QuadPart ) 
	{
		bCleared = true;
		m_szFPS.str(L"");
#ifndef FINAL_RELEASE
		m_szFPS << VERSION << L" (" << DRIVER_VER << L")\n" << gInfo.ProfileName << L"\n";
#endif
		switch (pWrapper_->GetD3DVersion())
		{
		case TD3DVersion_10_0:
			m_szFPS << L"D3D10 ";
			break;
		case TD3DVersion_10_1:
			m_szFPS << L"D3D10.1 ";
			break;
		case TD3DVersion_11_0:
			m_szFPS << L"D3D11 ";
			break;
		default:
			m_szFPS << L"D3D? ";
			break;
		}
		//DOUBLE fFPS = 0.0;
		if ( pWrapper_->m_bStereoModeChanged )
			m_szFPS << g_LocalData.GetText(LT_CountingFPS);
		else
		{
			//float fDiv = (float)m_nFPSTimeDelta.QuadPart / pWrapper_->m_nFreq.QuadPart;
			//fFPS = m_nSessionFrameCount / ( 1.0 * m_nFPSTimeDelta.QuadPart / pWrapper_->m_nFreq.QuadPart);
			m_szFPS << std::fixed << std::setprecision(2) << m_fFPS;
		}
		m_szFPS << L"  " << (pWrapper_->IsStereoActive() ? g_LocalData.GetText(LT_Stereo) : g_LocalData.GetText(LT_Mono));

		if ( !pWrapper_->m_bStereoModeChanged )
		{
#ifndef FINAL_RELEASE
			if (gInfo.ShowDebugInfo)
			{
				if ( !pWrapper_->IsStereoActive() )
					m_fLastMonoFPS = m_fFPS;
				else if ( m_nFPSDropShowTimeLeft.QuadPart > 0 )
				{
					m_szFPS << L" (" <<  g_LocalData.GetText(LT_drop) << L" " << std::setprecision(2) << 
						100 * (1 - m_fFPS / m_fLastMonoFPS) << L"%)";
				}
			}
#endif
			m_nSessionFrameCount		= 0;
			m_nFPSTimeDelta.QuadPart	= 0;
		}
		else if ( GINFO_DEBUG && m_fLastMonoFPS != 0.0f)
			m_nFPSDropShowTimeLeft.QuadPart = (7 + 1) * pWrapper_->m_nFreq.QuadPart;
		m_nLastDropTime.QuadPart = m_nLastFrameTime.QuadPart;
	}

	if (bCleared && (gInfo.OutputCaps & odShutterMode))
	{
		m_szFPS << L"\n";
		if (USE_UM_PRESENTER)
		{
			m_szFPS << L"UM ";
			CBasePresenterX* presenter = CPresenterX::Get();
			if (!m_pD3DDeviceWrapper->SkipPresenterThread())
			{
				int sleepTime = presenter->GetLastSleepTime();
				m_szFPS << L"Presenter: FPS " << std::fixed << std::setprecision(2) << presenter->GetFPS() << 
					L" sleepTime " << sleepTime <<
					L" SPS " << presenter->GetLagsPerSecond();

				m_szFPS << L"\nFramesFromLastVSync " << presenter->m_SuperSynchronizer.GetFramesFromLastVSync();

				//if (m_pD3DDeviceWrapper)
				//	m_szFPS << L"\nRefreshRate " << m_pD3DDeviceWrapper->m_RefreshRate <<
				//	L" SuperSynch RefreshRate " << m_pD3DDeviceWrapper->m_SuperSynchronizer.GetRefreshRate() <<				
				//	L" FPS RefreshRate " << m_pBasePresenterData->m_AverageFPS.GetRefreshRate();

				if (gInfo.PresenterSleepTime == -2 && !m_Description.Windowed)
					m_szFPS << L"\nFlush: Time " << m_pD3DDeviceWrapper->m_PresenterFlushTime;
			}
			else
				m_szFPS << L"Presenter: Simple";

#ifndef FINAL_RELEASE
			if (presenter->GetPresentCall())
			{
				m_szFPS << L"\n    Present(";
				if (presenter->GetPresentData()->Flags.Blt)
					m_szFPS << L"Blt)";
				else if (presenter->GetPresentData()->Flags.Flip)
					m_szFPS << L"Flip->" << GetFlipIntervalString(presenter->GetPresentData()->FlipInterval) << L")";
				else
					m_szFPS << L")";
			}
			else
				m_szFPS << L"\n    Blt()";
#endif
		}
		else
		{
			if (USE_ATI_PRESENTER)
				m_szFPS << L"ATI ";
			else if (USE_SIMPLE_PRESENTER)
				m_szFPS << L"Simple ";
			else if (USE_IZ3DKMSERVICE_PRESENTER)
				m_szFPS << L"iZ3D Shutter ";
			m_szFPS << L"Presenter";
		}
	}
}

void D3DSwapChain::StopPresenter()
{
	if( !CPresenterX::Get() )
		return;
	CPresenterX::Get()->SetActionAndWait(ptReleaseSC);
}

D3DSwapChain::~D3DSwapChain(void)
{
	PrimaryResourcesMap::iterator itp = m_ResourcesPrimary.begin();
	for (; itp != m_ResourcesPrimary.end(); ++itp)
	{
		Clear(&itp->second);
	}

	m_pSecondSwapChain.Release();
	m_pDXGIFactory.Release();

	if(m_hS3DSecondWindow)
	{
		SendMessage(m_hS3DSecondWindow, WM_CLOSE, 0, 0);
		if ( WaitForSingleObject(m_hS3DSecondWindowThread, 2000) != WAIT_OBJECT_0 )
		{
			ZLOG_MESSAGE(zlog::SV_ERROR, _T("Can't stop S3D window thread, terminating"));
			if (TerminateThread(m_hS3DSecondWindowThread, 0) == 0) {
				ZLOG_MESSAGE(zlog::SV_FATAL_ERROR, _T("Can't terminate S3D window thread"));
			}
		}
	}

	if(m_hS3DSecondWindowThread)
		CloseHandle(m_hS3DSecondWindowThread);

	CloseHandle(m_hSecondWindowInitialized);

	if(m_bClassWasRegistered)
		UnregisterClass(S3D_WINDOW_CLASS_NAME, (HINSTANCE)gData.hModule);
}

void D3DSwapChain::Initialize()
{
	CriticalSectionLocker locker( m_pD3DDeviceWrapper->m_CSUMCall  );
	// scaling
	if (gInfo.DisplayScalingMode != 0)
	{
		RECT src, dst;
		SetRect(&src, 0, 0, m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy);
		SetRect(&dst, 0, 0, m_BackBufferSize.cx, m_BackBufferSize.cy);

		TDisplayScalingMode mode = (TDisplayScalingMode)gInfo.DisplayScalingMode;
		ScaleRects(mode, &src, &dst, &m_SrcRect,  &m_DstRect);
	}

	SwapChainResources* pRes = GetCurrentPrimaryResource();
	ResourceWrapper* pWrp = pRes->m_pBackBufferPrimaryBeforeScalingWrap;
	pRes->Initialize(this);

	//--- initialize shift finder class ---
	UINT width  = m_BackBufferSize.cx;
	UINT height = m_BackBufferSize.cy;
	RECT r = { 0, 0, width, height };
	GetShiftFinder()->Initialize(m_pD3DDeviceWrapper, &r, &r);
	//--- initialize laser sight data ---
	GetLaserSightData()->Initialize(m_pD3DDeviceWrapper->m_LaserSight.GetTextureSize(), &r, &r);
#ifndef DISABLE_WIZARD
	//////////////////////////////////////////////////////////////////////////
	// Wizard initializing
	//if (m_BackBufferSize.cx > 400 && m_BackBufferSize.cy > 300)
	{
		m_WizardSCData.Initialize(&m_pD3DDeviceWrapper->m_Wizard, width, height );
		if (gInfo.Input.ShowWizardAtStartup && !gInfo.WizardWasShown)
			m_WizardSCData.Show();
		m_pD3DDeviceWrapper->m_HotKeyOSD.SetSize( width, height );
		m_pD3DDeviceWrapper->m_HotKeyOSD.Show();
	}
#endif

	// Add the presenter resources creation here //
	pRes->InitializePresenterResources();

	QueryPerformanceCounter(&m_nLastFrameTime);
}

void D3DSwapChain::ResolveMethodResources(SwapChainResources* pRes)
{	
	if (!pRes->m_bAAOn || pRes->m_hMethodResourceLeft.pDrvPrivate == NULL)
		return;
	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnResourceResolveSubresource( m_pD3DDeviceWrapper->hDevice, 
		pRes->m_hMethodResourceLeft, 0,
		pRes->m_hBackBufferLeft, 0, m_Description.BufferDesc.Format );
	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnShaderResourceViewReadAfterWriteHazard( m_pD3DDeviceWrapper->hDevice, 
		pRes->m_hMethodTextureLeft, pRes->m_hMethodResourceLeft  );

	if (m_pD3DDeviceWrapper->m_Input.StereoActive)
	{
		m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnResourceResolveSubresource( m_pD3DDeviceWrapper->hDevice, 
			pRes->m_hMethodResourceRight, 0,
			pRes->m_hBackBufferRight, 0, m_Description.BufferDesc.Format );
		m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnShaderResourceViewReadAfterWriteHazard( m_pD3DDeviceWrapper->hDevice, 
			pRes->m_hMethodTextureRight, pRes->m_hMethodResourceRight  );
	}
}

bool D3DSwapChain::GetOutputs(IDXGIFactory* pDXGIFactory, IDXGIOutput** ppFirstOutput, IDXGIOutput** ppSecondOutput)
{
	_ASSERT(ppFirstOutput);
	_ASSERT(ppSecondOutput);
	if ((*ppFirstOutput) == NULL)
		m_pSwapChain->GetContainingOutput( ppFirstOutput );
	if (!*ppFirstOutput || !pDXGIFactory)
		return false;
	DXGI_OUTPUT_DESC descFirst;
	(*ppFirstOutput)->GetDesc(&descFirst);

	int nAdapter = 0;
	CComPtr<IDXGIAdapter> pAdapter = NULL;
	CComPtr<IDXGIAdapter> pMainAdapter = NULL;
	while( pDXGIFactory->EnumAdapters(nAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND )
	{
		int nOutput = 0;
		CComPtr<IDXGIOutput> pOutput = NULL;
		while( pAdapter->EnumOutputs( nOutput, &pOutput) != DXGI_ERROR_NOT_FOUND )
		{
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);

			if ( descFirst.Monitor == desc.Monitor )
			{
				pOutput.Release();
				pMainAdapter = pAdapter;
				break;
			}
			++nOutput;
			pOutput.Release();
		};
		pAdapter.Release();
		++nAdapter;
	}

	bool bFound = false;
	if ( pMainAdapter != NULL )
	{
		int nOutput = 0;
		CComPtr<IDXGIOutput> pOutput = NULL;
		while( pMainAdapter->EnumOutputs( nOutput, &pOutput) != DXGI_ERROR_NOT_FOUND )
		{
			DXGI_OUTPUT_DESC desc;
			pOutput->GetDesc(&desc);

			if ( descFirst.Monitor != desc.Monitor  )
			{
				*ppSecondOutput = pOutput;
				(*ppSecondOutput)->AddRef();
				pOutput.Release();
				bFound = true;
				break;
			}
			++nOutput;
			pOutput.Release();
		};
	}

#ifndef FINAL_RELEASE
	if (!*ppSecondOutput)
	{
		nAdapter = 0;
		while( pDXGIFactory->EnumAdapters(nAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND && !bFound)
		{
			int nOutput = 0;
			CComPtr<IDXGIOutput> pOutput = NULL;
			while( pAdapter->EnumOutputs( nOutput, &pOutput) != DXGI_ERROR_NOT_FOUND )
			{
				DXGI_OUTPUT_DESC desc;
				pOutput->GetDesc(&desc);

				if ( descFirst.Monitor != desc.Monitor   )
				{
					*ppSecondOutput = pOutput;
					(*ppSecondOutput)->AddRef();
					pOutput.Release();
					bFound = true;
					break;
				}
				++nOutput;
				pOutput.Release();
			};
			pAdapter.Release();
			++nAdapter;
		}
	}
#endif	

	pMainAdapter.Release();	
	return bFound;
}

bool D3DSwapChain::GetSecondaryWindowRect(RECT* rcWindow, RECT* rcClient)
{
	CComPtr<IDXGIOutput>	FirstOutput;
	DXGI_OUTPUT_DESC firstDesc;

	RECT primaryWindowRect;
	if (gInfo.MultiWindowsMode != MULTI_WINDOWS_MODE_SIDEBYSIDEWINDOWED)
	{
		CComPtr<IDXGIOutput>	SecondOutput;
		DXGI_OUTPUT_DESC secondDesc;
		if (!GetOutputs( m_pDXGIFactory, &FirstOutput, &SecondOutput ))
			return false;

		SecondOutput->GetDesc(&secondDesc);
		FirstOutput->GetDesc(&firstDesc);
		FirstOutput.Release();
		SecondOutput.Release();

		HWND hWnd = m_Description.OutputWindow;
		GetClientRect(hWnd, &primaryWindowRect);
		POINT pt;
		pt.x = primaryWindowRect.left;
		pt.y = primaryWindowRect.top;
		ClientToScreen(hWnd, &pt);

		DWORD c = m_pD3DDeviceWrapper->m_pOutputMethod->GetSecondWindowCaps();
		if (c & swcInvertHorizontal)
			pt.x = firstDesc.DesktopCoordinates.right - (pt.x + primaryWindowRect.right - primaryWindowRect.left);
		if (c & swcInvertVertical)
			pt.y = firstDesc.DesktopCoordinates.bottom - (pt.y + primaryWindowRect.bottom -  primaryWindowRect.top);

		rcClient->left   = pt.x  - firstDesc.DesktopCoordinates.left + secondDesc.DesktopCoordinates.left;
		rcClient->top    = pt.y  - firstDesc.DesktopCoordinates.top + secondDesc.DesktopCoordinates.top;
	}
	else
	{
		if (FirstOutput == NULL)
			m_pSwapChain->GetContainingOutput( &FirstOutput );
		if (FirstOutput == NULL)
			return false;

		FirstOutput->GetDesc(&firstDesc);
		FirstOutput.Release();

		HWND hWnd = m_Description.OutputWindow;
		GetClientRect(hWnd, &primaryWindowRect);
		POINT pt;
		pt.x = primaryWindowRect.left;
		pt.y = primaryWindowRect.top;
		ClientToScreen(hWnd, &pt);
		rcClient->left   = pt.x - firstDesc.DesktopCoordinates.left + 
			primaryWindowRect.right - primaryWindowRect.left + 10;
		rcClient->top    = pt.y;
	}

	rcClient->right  = rcClient->left + primaryWindowRect.right - primaryWindowRect.left;
	rcClient->bottom = rcClient->top + primaryWindowRect.bottom -  primaryWindowRect.top;
	*rcWindow = *rcClient;
	AdjustWindowRect(rcWindow, (DWORD)GetWindowLongPtr(m_hS3DSecondWindow, GWL_STYLE), FALSE);
	return true;
}

void D3DSwapChain::CheckSecondWindowPosition(bool bShowWindow)
{	
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
	{
#ifndef FINAL_RELEASE
		if (IsDebuggerPresent())
			ClipCursor(NULL);
#endif
		return;
	}

	//--- use SW_SHOWNA flag to save focus ---
	if (!m_bS3DSecondWindowVisible && bShowWindow)	
	{
		m_bS3DSecondWindowVisible = true;
		SendOrPostMessage(SWM_SHOW_SECOND_WINDOW);
	}

	if (m_Description.Windowed)
	{
		//--- synchronize secondary window position ---
		RECT rcClient, rcFront;
		if (GetSecondaryWindowRect(&m_S3DSecondWindowRect, &rcClient))
		{
			GetWindowRect(m_hS3DSecondWindow, &rcFront);
			if(!EqualRect(&m_S3DSecondWindowRect, &rcFront))
				SendOrPostMessage(SWM_MOVE_SECOND_WINDOW);
		}
	}
	else // fullscreen
	{
		RECT rcWindow;
		RECT primaryWindowRect;
		HWND hWnd = m_Description.OutputWindow;
		GetClientRect(hWnd, &primaryWindowRect);
		POINT pt;
		pt.x = primaryWindowRect.left;
		pt.y = primaryWindowRect.top;
		ClientToScreen(hWnd, &pt);
		rcWindow.left   = pt.x;
		rcWindow.top    = pt.y;
		rcWindow.right  = rcWindow.left + primaryWindowRect.right - primaryWindowRect.left;
		rcWindow.bottom = rcWindow.top + primaryWindowRect.bottom -  primaryWindowRect.top;

#ifndef FINAL_RELEASE
		if (IsDebuggerPresent())
			ClipCursor(NULL);
		else
#endif
			ClipCursor(&rcWindow);
		//if (GetBaseDevice()->m_pVar)
		//	*GetBaseDevice()->m_pVar = GetCurrentProcessId();
	}

}

bool D3DSwapChain::SendOrPostMessage(WPARAM wParam)
{
	DEBUG_TRACE1(_T("SendOrPostMessage(%S)\n"), EnumToString((SecondWindowMessage)wParam));
	if (wParam != SWM_MOVE_APP_WINDOW && m_pD3DDeviceWrapper->IsCurrentThreadMain())
		return S3DWindowProc(m_hS3DSecondWindow, WM_USER, wParam, (LPARAM)this) != 0;
	else if (m_hS3DSecondWindow)
		return PostMessage(m_hS3DSecondWindow, WM_USER, wParam, (LPARAM)this) != 0;
	else 
		return false;
}

void D3DSwapChain::Present( UINT SyncInterval, UINT Flags )
{
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return;
	WE_CALL;
	HRESULT hResult;
	NSCALL_TRACE3(m_pSecondSwapChain->Present( SyncInterval, Flags ),
		DEBUG_MESSAGE(_T("\tm_pSecondSwapChain->Present(SyncInterval = %u, Flags = %u)"), 
		SyncInterval, Flags ));
}

void D3DSwapChain::ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags )
{
	m_Description.BufferCount = BufferCount;
	if (!m_Description.Windowed && gInfo.DisplayScalingMode != 0)
	{
		m_Description.BufferDesc.Width = gInfo.DisplayNativeWidth;
		m_Description.BufferDesc.Height = gInfo.DisplayNativeHeight;
	}
	else
	{
		m_Description.BufferDesc.Width = Width;
		m_Description.BufferDesc.Height = Height;
	}
	m_Description.BufferDesc.Format = NewFormat;
	SetBackBufferSize();
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return;
	WE_CALL;
	HRESULT hResult;
	CheckSecondWindowPosition();
//	if (!m_Description.Windowed)
//		SwapChainFlags &= ~DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // TODO: retest this
	NSCALL_TRACE1(m_pSecondSwapChain->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags),
		DEBUG_MESSAGE(_T("\tm_pSecondSwapChain::ResizeBuffers(BufferCount = %u, Width = %u, Height = %u, NewFormat = %S, SwapChainFlags = %u)"), 
		BufferCount, Width, Height, EnumToString(NewFormat), SwapChainFlags));
}

void D3DSwapChain::ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters )
{
	m_Description.BufferDesc = *pNewTargetParameters;
	SetBackBufferSize();
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return;
	WE_CALL;
	HRESULT hResult;
	CheckSecondWindowPosition();

	NSCALL_TRACE1(m_pSecondSwapChain->ResizeTarget(pNewTargetParameters), 
		DEBUG_MESSAGE(_T("\tm_pSecondSwapChain::ResizeTarget(pNewTargetParameters = (%s))"), 
		GetDXGIModeString(pNewTargetParameters)));

	/*
	SetWindowPos(m_hS3DSecondWindow, HWND_NOTOPMOST, m_S3DSecondWindowRect.left, m_S3DSecondWindowRect.top, 
		m_S3DSecondWindowRect.right - m_S3DSecondWindowRect.left, m_S3DSecondWindowRect.bottom - m_S3DSecondWindowRect.top, SWP_NOACTIVATE);
		*/
	//ShowWindow(m_hS3DSecondWindow, SW_HIDE);
}

void D3DSwapChain::WriteJPSScreenshot( bool bApplyGammaCorrection_ )
{
	DEBUG_MESSAGE(_T("Making Screenshot..."));
	TCHAR	str[ MAX_PATH ];
	TCHAR	path[ MAX_PATH ];
	TCHAR	exeName[ MAX_PATH ];	

	D3D10DDI_DEVICEFUNCS& GET_ORIG		= m_pD3DDeviceWrapper->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& GET_ORIG11	= m_pD3DDeviceWrapper->OriginalDeviceFuncs11;
	D3D10DDI_HDEVICE hDevice			= m_pD3DDeviceWrapper->hDevice;
	SwapChainResources* pRes			= GetCurrentPrimaryResource();	

	//////////////////////////////////////////////////////////////////////////

	BYTE*	pTextureData				= NULL;
	UINT	nTextureWidth				= m_Description.BufferDesc.Width;
	UINT	nTextureHeight				= m_Description.BufferDesc.Height;
	DXGI_FORMAT tFormat					= m_Description.BufferDesc.Format;	

	D3D11DDIARG_CREATERESOURCE CreateTexRes;
	memset( &CreateTexRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	D3D10DDI_MIPINFO TexMipInfo;
	memset( &TexMipInfo, 0, sizeof( D3D10DDI_MIPINFO ) );
	TexMipInfo.TexelWidth				= nTextureWidth;
	TexMipInfo.TexelHeight				= nTextureHeight;
	TexMipInfo.TexelDepth				= 1;
	TexMipInfo.PhysicalWidth			= nTextureWidth;
	TexMipInfo.PhysicalHeight			= nTextureHeight;
	TexMipInfo.PhysicalDepth			= 1;

	CreateTexRes.pMipInfoList			= &TexMipInfo;
	CreateTexRes.pInitialDataUP			= NULL;
	CreateTexRes.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
	CreateTexRes.Usage					= D3D10_DDI_USAGE_STAGING;
	CreateTexRes.MapFlags				= D3D10_DDI_MAP_READ;
	CreateTexRes.BindFlags				= 0;
	CreateTexRes.Format					= tFormat;
	CreateTexRes.SampleDesc.Count		= 1;
	CreateTexRes.SampleDesc.Quality		= 0;
	CreateTexRes.MipLevels				= 1;
	CreateTexRes.ArraySize				= 1;

	D3D10DDI_HRESOURCE	hTexture		= { NULL };
	D3D10DDI_HRTRESOURCE hRTTexture		= { NULL };
	m_pD3DDeviceWrapper->CreateResource(hTexture, &CreateTexRes, hRTTexture);

	/************************************************************************/	
	// left resource
	D3D10DDI_MAPPED_SUBRESOURCE mapInfo;
	memset( &mapInfo, 0, sizeof( D3D10DDI_MAPPED_SUBRESOURCE ) );

	D3D10DDI_HRESOURCE	hTextureSrc = pRes->GetMethodResourceLeft();
	GET_ORIG.pfnResourceCopy( hDevice, hTexture, hTextureSrc );
	GET_ORIG.pfnStagingResourceMap( hDevice, hTexture, 0, D3D10_DDI_MAP_READ, 0, &mapInfo );

	int viewCount = m_pD3DDeviceWrapper->IsStereoActive() ?  2  : 1;
	pTextureData = DNew BYTE[ viewCount * mapInfo.DepthPitch ];

	UINT	nRowPitch	 = mapInfo.RowPitch;
	UINT	nBigRowPitch = nRowPitch * viewCount;	

	UINT offset = 0;
	if(m_pD3DDeviceWrapper->IsStereoActive())
	{
		offset = (UINT)(GetBitWidthOfDXGIFormat(tFormat) * nTextureWidth / 8);
		if (offset == 0) 
			offset = nRowPitch;
	}

	for ( UINT i = 0; i < nTextureHeight; i++ )
	{		
		BYTE*	pDataDst = pTextureData + i * nBigRowPitch + offset;
		BYTE*	pDataSrc = ( BYTE* )mapInfo.pData + i * nRowPitch;
		memcpy( pDataDst, pDataSrc, nRowPitch );
	}

	GET_ORIG.pfnStagingResourceUnmap( hDevice, hTexture, 0 );

	if(m_pD3DDeviceWrapper->IsStereoActive())
	{
		/************************************************************************/	
		// right resource	
		memset( &mapInfo, 0, sizeof( D3D10DDI_MAPPED_SUBRESOURCE ) );

		hTextureSrc = ( m_pD3DDeviceWrapper->IsStereoActive() ) ? pRes->GetMethodResourceRight() : pRes->GetMethodResourceLeft();
		GET_ORIG.pfnResourceCopy( hDevice, hTexture, hTextureSrc );
		GET_ORIG.pfnStagingResourceMap( hDevice, hTexture, 0, D3D10_DDI_MAP_READ, 0, &mapInfo );	

		for ( UINT i = 0; i < nTextureHeight; i++ )
		{
			BYTE*	pDataDst = pTextureData + i * nBigRowPitch;
			BYTE*	pDataSrc = ( BYTE* )mapInfo.pData + i * nRowPitch;
			memcpy( pDataDst, pDataSrc, nRowPitch > offset ? offset : nRowPitch );
		}

		GET_ORIG.pfnStagingResourceUnmap( hDevice, hTexture, 0 );

		/************************************************************************/	

		GET_ORIG.pfnDestroyResource( hDevice, hTexture );
		DELETE_HANDLE( hTexture );	
	}

	//////////////////////////////////////////////////////////////////////////	

#define FORBIDDEN_CHARS _T("\\/:*?\"<>|")

	if (gInfo.MakeScreenshot)
	{
		SaveImageToFile(gInfo.MakeScreenshotPath, tFormat, nTextureWidth * viewCount, nTextureHeight, (char*)pTextureData, nBigRowPitch );
	}
	else if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYPICTURES|CSIDL_FLAG_CREATE, NULL, 0, path)))
	{
		PathAppend(path, _T(PRODUCT_NAME) _T(" Screenshots") );
		_tcscpy_s(exeName, _countof(exeName), gInfo.ProfileName);
		size_t p = _tcscspn(exeName, FORBIDDEN_CHARS);
		while (p != _tcslen(exeName))
		{
			exeName[p]='_';
			p = _tcscspn(exeName, FORBIDDEN_CHARS);
		}
		CreateDirectory(path, 0);
		PathAppend(path, exeName );
		CreateDirectory(path, 0);

		while(1)
		{
			const TCHAR* ext = m_pD3DDeviceWrapper->IsStereoActive() ? GetStereoImageFileExtension() : GetImageFileExtension();
			_stprintf_s(str, L"%s\\%s #%04i%s", path, exeName, m_pD3DDeviceWrapper->GetScreenShotCount(), ext);
			//--- skip existing files ---
			if(!PathFileExists(str))
				break;
			m_pD3DDeviceWrapper->IncScreenShotCount();
		}
		//if( bApplyGammaCorrection_ && !m_Description.Windowed ) // only in fullscreen mode
		//{
		//	D3DGAMMARAMP pRamp, *pRampForApply;
		//	if ( m_pD3DDeviceWrapper->m_pOutputMethod->HandleGamma())
		//	{
		//		GetGammaRamp(&pRamp);
		//		pRampForApply = &pRamp;
		//	}
		//	else
		//		pRampForApply = &GetBaseDevice()->m_GlobalGammaRamp;
		//	MakeGammaCorrectionAndSaveSurfaceToFile(str, JPSSurface, pRampForApply, GetImageFileFormat());
		//}
		//else
		{
			SaveImageToFile( str, tFormat, nTextureWidth * viewCount, nTextureHeight, (char*)pTextureData, nBigRowPitch );
		}
	}

	delete[] pTextureData;

	DEBUG_MESSAGE(_T("Making Screenshot... OK"));
}

void D3DSwapChain::CheckSwapChainMode()
{
#ifndef FINAL_RELEASE
	HRESULT hResult;
	DXGI_SWAP_CHAIN_DESC desc;
	NSCALL(m_pSwapChain->GetDesc(&desc));
	if (desc.Windowed != m_Description.Windowed)	{
		DEBUG_TRACE1(_T("\tFirst SwapChain mode changed(new mode windowed = %d (%d))\n"), desc.Windowed, m_Description.Windowed);
	}
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return;
	NSCALL(m_pSecondSwapChain->GetDesc(&desc));
	if (desc.Windowed != m_Description.Windowed)	{
		DEBUG_TRACE1(_T("\tSecond SwapChain mode changed(new mode windowed = %d (%d))\n"), desc.Windowed, m_Description.Windowed);
	}
#endif
}

bool D3DSwapChain::IsSwapChainsChangeMode()
{
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
		return false;

	HRESULT hResult;
	DXGI_SWAP_CHAIN_DESC firstDesc;
	DXGI_SWAP_CHAIN_DESC secondDesc;
	NSCALL(m_pSwapChain->GetDesc(&firstDesc));
	NSCALL(m_pSecondSwapChain->GetDesc(&secondDesc));
	if (firstDesc.Windowed != m_Description.Windowed &&
		secondDesc.Windowed != m_Description.Windowed)	{
		return true;
	}
	return false;
}

HRESULT D3DSwapChain::SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget )
{
	if (!m_pD3DDeviceWrapper->m_bTwoWindows)
	{
		if ( Fullscreen )
			InitAQBSData();
		return S_OK;
	}

	CComPtr<IDXGIOutput>	pFirstOutput = pTarget;
	CComPtr<IDXGIOutput>	pSecondOutput; 
	if (Fullscreen && !GetOutputs( m_pDXGIFactory, &pFirstOutput.p, &pSecondOutput.p )) // do not use CComPtr::operator &, it will assert if pointer is not NULL
		return S_OK;

	WE_CALL;
	HRESULT hResult;
	NSCALL_TRACE1(m_pSecondSwapChain->SetFullscreenState( Fullscreen, NULL ),
		DEBUG_MESSAGE(_T("\tm_pSecondSwapChain->SetFullscreenState(Fullscreen = %i, pTarget = %p)"), 
		Fullscreen, pSecondOutput));

	if (Fullscreen)
		ShowWindow(m_Description.OutputWindow, SW_RESTORE);
	return hResult;
}

void D3DSwapChain::SetBackBufferSize()
{
	m_BackBufferSize.cx = m_Description.BufferDesc.Width;
	m_BackBufferSize.cy = m_Description.BufferDesc.Height;
	InitAQBSData();
	m_pD3DDeviceWrapper->UpdateProjectionMatrix();
}

HRESULT D3DSwapChain::ScalingNone( SwapChainResources* pRes )
{
	HRESULT hResult = S_OK;
	DXGI_DDI_ARG_BLT bltArg;
	bltArg.hDevice			= (DXGI_DDI_HDEVICE)m_pD3DDeviceWrapper->hDevice.pDrvPrivate;
	bltArg.SrcSubresource	= 0;
	bltArg.DstSubresource	= 0;
	bltArg.DstLeft			= 0;
	bltArg.DstRight			= m_BackBufferSize.cx;
	bltArg.DstTop			= 0;
	bltArg.DstBottom		= m_BackBufferSize.cy;
	bltArg.Flags.Value		= 0;
	bltArg.Flags.Stretch	= 1;
	bltArg.Rotate			= DXGI_DDI_MODE_ROTATION_IDENTITY;

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeftBeforeScaling.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeft.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferRightBeforeScaling.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferRight.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );
	return hResult;
}

HRESULT D3DSwapChain::ScalingGeneral( SwapChainResources* pRes )
{
	HRESULT hResult = S_OK;
	DXGI_DDI_ARG_BLT bltArg;
	bltArg.hDevice			= (DXGI_DDI_HDEVICE)m_pD3DDeviceWrapper->hDevice.pDrvPrivate;
	bltArg.SrcSubresource	= 0;
	bltArg.DstSubresource	= 0;
	bltArg.DstLeft			= m_DstRect.left;
	bltArg.DstRight			= m_DstRect.right;
	bltArg.DstTop			= m_DstRect.top;
	bltArg.DstBottom		= m_DstRect.bottom;
	bltArg.Flags.Value		= 0;
	bltArg.Flags.Stretch	= 1;
	bltArg.Rotate			= DXGI_DDI_MODE_ROTATION_IDENTITY;

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeftBeforeScaling.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeft.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferRightBeforeScaling.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferRight.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );
	return hResult;
}

HRESULT D3DSwapChain::ScalingAspectX( SwapChainResources* pRes )
{
	HRESULT hResult = S_OK;
	D3D10_DDI_BOX cpyBox;
	cpyBox.front	= 0;
	cpyBox.back		= 1;
	cpyBox.top		= m_SrcRect.top;
	cpyBox.bottom	= m_SrcRect.bottom;
	cpyBox.left		= m_SrcRect.left;
	cpyBox.right	= m_SrcRect.right;

	DXGI_DDI_ARG_BLT bltArg;
	bltArg.hDevice			= (DXGI_DDI_HDEVICE)m_pD3DDeviceWrapper->hDevice.pDrvPrivate;
	bltArg.hSrcResource		= (DXGI_DDI_HRESOURCE)pRes->m_hMethodResourceLeft.pDrvPrivate;
	bltArg.SrcSubresource	= 0;
	bltArg.hDstResource		= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeft.pDrvPrivate;
	bltArg.DstSubresource	= 0;
	bltArg.DstLeft			= m_DstRect.left;
	bltArg.DstRight			= m_DstRect.right;
	bltArg.DstTop			= m_DstRect.top;
	bltArg.DstBottom		= m_DstRect.bottom;
	bltArg.Flags.Value		= 0;
	bltArg.Flags.Stretch	= 1;
	bltArg.Rotate			= DXGI_DDI_MODE_ROTATION_IDENTITY;

	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnResourceCopyRegion( m_pD3DDeviceWrapper->hDevice, 
		pRes->m_hScaledMethodResource, 
		0, 
		0, 
		0, 
		0, 
		pRes->m_hBackBufferLeftBeforeScaling, 
		0, 
		&cpyBox );

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hScaledMethodResource.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferLeft.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );

	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnResourceCopyRegion( m_pD3DDeviceWrapper->hDevice, 
		pRes->m_hScaledMethodResource, 
		0, 
		0, 
		0, 
		0, 
		pRes->m_hBackBufferRightBeforeScaling, 
		0, 
		&cpyBox );

	bltArg.hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->m_hScaledMethodResource.pDrvPrivate;
	bltArg.hDstResource	= (DXGI_DDI_HRESOURCE)pRes->m_hBackBufferRight.pDrvPrivate;

	NSCALL_TRACE3( m_pD3DDeviceWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&bltArg), 
		DEBUG_MESSAGE( _T("Blt(pBltData = %s)"), GetDXGIBltArgString(&bltArg) ) );
	return hResult;
}

HRESULT D3DSwapChain::DoScaling()
{
	SwapChainResources* pRes = GetCurrentPrimaryResource();

	ResourceWrapper* pWrapperBF = pRes->m_pBackBufferPrimaryWrap;
	ResourceWrapper* pWrapper = pRes->m_pBackBufferPrimaryBeforeScalingWrap;
	
	if (pRes->m_hBackBufferLeftBeforeScaling == pRes->m_hBackBufferLeft
		&& pRes->m_hBackBufferRightBeforeScaling == pRes->m_hBackBufferRight) 
	{
		return S_OK;
	}

	// clear RTs before copying into them, to disable overdraw of info and logo
	FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnClearRenderTargetView(m_pD3DDeviceWrapper->hDevice, pRes->m_hBackBufferRTLeft, clearColor);
	m_pD3DDeviceWrapper->OriginalDeviceFuncs.pfnClearRenderTargetView(m_pD3DDeviceWrapper->hDevice, pRes->m_hBackBufferRTRight, clearColor);

	HRESULT				hResult;
	TDisplayScalingMode	mode = TDisplayScalingMode(gInfo.DisplayScalingMode);
	switch (mode)
	{
		case DISPLAY_SCALING_NONE:
			hResult = ScalingNone(pRes);
			break;

		case DISPLAY_SCALING_TOP_LEFT:
		case DISPLAY_SCALING_CENTERED:
		case DISPLAY_SCALING_SCALEDASPECT:
		case DISPLAY_SCALING_SCALED:
			hResult = ScalingGeneral(pRes);
			break;

		case DISPLAY_SCALING_SCALEDASPECTEX:
			hResult = ScalingAspectX(pRes);
			break;

		default:
			assert(!"Can't get here");
	}

#ifndef FINAL_RELEASE
	if( GINFO_DUMP_ON && m_pD3DDeviceWrapper->m_DumpType >= dtOnlyRT )
	{
		ResourceWrapper* pWrapperBeforeScaling = pRes->m_pBackBufferPrimaryBeforeScalingWrap;
		ResourceWrapper* pWrapper = pRes->m_pBackBufferPrimaryWrap;

		TCHAR szFileName[MAX_PATH];
		_stprintf_s(szFileName, L"%s\\BackBufferLeftBeforeScaling",  m_pD3DDeviceWrapper->m_DumpDirectory); 
		ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hBackBufferLeftBeforeScaling, &pWrapperBeforeScaling->m_CreateResource11, szFileName);
		_stprintf_s(szFileName, L"%s\\BackBufferLeft",  m_pD3DDeviceWrapper->m_DumpDirectory); 
		ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hBackBufferLeft, &pWrapper->m_CreateResource11, szFileName);
		_stprintf_s(szFileName, L"%s\\BackBufferRightBeforeScaling",  m_pD3DDeviceWrapper->m_DumpDirectory); 
		ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hBackBufferRightBeforeScaling, &pWrapperBeforeScaling->m_CreateResource11, szFileName);
		_stprintf_s(szFileName, L"%s\\BackBufferRight",  m_pD3DDeviceWrapper->m_DumpDirectory); 
		ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hBackBufferRight, &pWrapper->m_CreateResource11, szFileName);

	}
#endif

	return hResult;
}

void D3DSwapChain::PresentData( )
{
	RECT rect;
	GetClientRect( m_Description.OutputWindow, &rect );
	m_DstMonitorPoint.x = rect.left;
	m_DstMonitorPoint.y = rect.top;
	ClientToScreen( m_Description.OutputWindow, &m_DstMonitorPoint );

	HMONITOR hMon = MonitorFromPoint(m_DstMonitorPoint, MONITOR_DEFAULTTONEAREST);
	MONITORINFO  info;
	info.cbSize = sizeof ( MONITORINFO );
	GetMonitorInfo(hMon, &info);

	m_DstMonitorPoint.x -= info.rcMonitor.left;
	m_DstMonitorPoint.y -= info.rcMonitor.top;

	SwapChainResources* pRes = GetCurrentPrimaryResource();
	if (IS_SHUTTER_OUTPUT)
	{
		if ( (USE_UM_PRESENTER || USE_IZ3DKMSERVICE_PRESENTER) && !m_bUseSimplePresenter)
		{
			if( pRes->IsStereoActive() )
			{
				g_KMShutter.StartShutter();
			}
			else
			{
//				g_KMShutter.StopShutter();
			}
			if( USE_IZ3DKMSERVICE_PRESENTER )
			{
				// N.B. Change the presentation order if eyes are swapped ("real" left image always goes first)!
				// ...but don't change the order in mono!
				m_pD3DDeviceWrapper->m_pOutputMethod->Output( (pRes->IsStereoActive() & pRes->IsSwapEyes()) ^ m_bLeftEyeActive , this, NULL );
				m_pD3DDeviceWrapper->m_pOutputMethod->Output( !((pRes->IsStereoActive() & pRes->IsSwapEyes()) ^ m_bLeftEyeActive) , this, NULL );
			}
			else
			{
				m_pD3DDeviceWrapper->m_pOutputMethod->Output( false, this, NULL );
				m_pD3DDeviceWrapper->m_pOutputMethod->Output( true, this, NULL );
			}
#ifndef FINAL_RELEASE
			if( GINFO_DUMP_ON && m_pD3DDeviceWrapper->m_DumpType >= dtOnlyRT )
			{
				ResourceWrapper* pBBWrapper = pRes->m_pBackBufferPrimaryWrap;
				ResourceWrapper* pWrapper;
				InitWrapper(pRes->m_hPresenterResourceWrap, pWrapper);

				TCHAR szFileName[MAX_PATH];
				_stprintf_s(szFileName, L"%s\\MethodResource.%s.Src.RT", 
					m_pD3DDeviceWrapper->m_DumpDirectory, m_bLeftEyeActive ? _T("L") : _T("R")); 
				ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, m_bLeftEyeActive ? pRes->m_hMethodResourceLeft : pRes->m_hMethodResourceRight, 
					&pBBWrapper->m_CreateResource11, szFileName);

				_stprintf_s(szFileName, L"%s\\SendImageToPresenter.Dst.RT", 
					m_pD3DDeviceWrapper->m_DumpDirectory); 
				pWrapper->DumpToFile(m_pD3DDeviceWrapper, szFileName, NULL, true, true, m_bLeftEyeActive ? dvLeft : dvRight);

				// --- new code here ---
				if( USE_IZ3DKMSERVICE_PRESENTER )
				{
					_stprintf_s(szFileName, L"%s\\IZKM.MethodResourceLeft",  m_pD3DDeviceWrapper->m_DumpDirectory);
					ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hMethodResourceLeft, &pBBWrapper->m_CreateResource11, szFileName);

					_stprintf_s(szFileName, L"%s\\IZKM.MethodResourceRight",  m_pD3DDeviceWrapper->m_DumpDirectory);
					ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hMethodResourceRight, &pBBWrapper->m_CreateResource11, szFileName);

					_stprintf_s(szFileName, L"%s\\IZKM.PresenterResourceLeft",  m_pD3DDeviceWrapper->m_DumpDirectory);
					ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hPresenterResourceLeft, &pBBWrapper->m_CreateResource11, szFileName);

					_stprintf_s(szFileName, L"%s\\IZKM.PresenterResourceRight", m_pD3DDeviceWrapper->m_DumpDirectory); 
					ResourceWrapper::DumpToFile(m_pD3DDeviceWrapper, pRes->m_hPresenterResourceRight, &pBBWrapper->m_CreateResource11, szFileName);
				}
			}
#endif
		}
		else
		{
			m_pD3DDeviceWrapper->m_pOutputMethod->Output( m_bLeftEyeActive, this, NULL );
		}
	}
	else
	{
		ResolveMethodResources(pRes);
		m_pD3DDeviceWrapper->m_pOutputMethod->Output( this );
	}
}

bool D3DSwapChain::BackBufferExist( DXGI_DDI_HRESOURCE hSurfaceToPresent ) const
{
	D3D10DDI_HRESOURCE hResource = { (void*) hSurfaceToPresent };
	PrimaryResourcesMap::const_iterator it = m_ResourcesPrimary.find(hResource);
	return m_LatestResourcePrimary != m_ResourcesPrimary.end();
}

SwapChainResources* D3DSwapChain::CreateOrFindBackBufferResources( DXGI_DDI_HRESOURCE hSurfaceToPresent, DXGI_DDI_HRESOURCE hBackBufferPrimaryApp )
{
	D3D10DDI_HRESOURCE hResource = { (void*) hSurfaceToPresent };
	D3D10DDI_HRESOURCE hAppBB = { (void*) hBackBufferPrimaryApp };
	m_LatestResourcePrimary = m_ResourcesPrimary.find(hResource);
	if (m_LatestResourcePrimary == m_ResourcesPrimary.end())
	{
		DEBUG_TRACE1(_T("Add app backbuffer - %p\n"), hResource.pDrvPrivate);

		m_ResourcesPrimary[ hResource ] = SwapChainResources();
		m_LatestResourcePrimary			= m_ResourcesPrimary.find(hResource);
		SwapChainResources* pRes		= GetCurrentPrimaryResource();
		pRes->m_pBackBufferPrimaryWrap	= (ResourceWrapper*)hResource.pDrvPrivate;
		pRes->m_pBackBufferPrimaryBeforeScalingWrap	= (ResourceWrapper*)((hAppBB.pDrvPrivate == NULL) ? hResource.pDrvPrivate : hAppBB.pDrvPrivate);
		pRes->m_pDevice					= m_pD3DDeviceWrapper;
			
		Initialize();
	}
	return  &m_LatestResourcePrimary->second;
}

void D3DSwapChain::Clear( SwapChainResources* pRes )
{
	pRes->ClearPresenterResources();
	m_bPresenterCall = false;
	m_bResourceCopyRegionCalled = false;
	m_bPresenterThreadId = 0;
	pRes->Clear();
	GetShiftFinder()->Clear();
#ifndef DISABLE_WIZARD
	GetWizardData()->Clear();
	//////////////////////////////////////////////////////////////////////////
	//FIXME: because of Wizard bug we should reinit wizard object too (for recreating font textures)
	m_pD3DDeviceWrapper->m_Wizard.Clear();
	m_pD3DDeviceWrapper->m_HotKeyOSD.Uninitialize();
	auto WizDrawer = new WizardDrawer( m_pD3DDeviceWrapper->m_pDynRender );
	m_pD3DDeviceWrapper->m_Wizard.Initialize( WizDrawer );
	m_pD3DDeviceWrapper->m_HotKeyOSD.Initialize( WizDrawer );
	//////////////////////////////////////////////////////////////////////////
#endif
}

DWORD WINAPI S3DMessageQueue(LPVOID lParam)
{	
	D3DSwapChain* pSwapChain = static_cast<D3DSwapChain*>(lParam);
		
	DEBUG_MESSAGE(_T("Initializing second window"));
	ZeroMemory(&pSwapChain->m_S3DWindowClass, sizeof(pSwapChain->m_S3DWindowClass));
	pSwapChain->m_S3DWindowClass.lpfnWndProc = S3DWindowProc; 
	pSwapChain->m_S3DWindowClass.hInstance = (HINSTANCE)gData.hModule;
	pSwapChain->m_S3DWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
#ifdef DEBUG
	//--- we not need to redraw background ---
	pSwapChain->m_S3DWindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   
#endif
	pSwapChain->m_S3DWindowClass.lpszClassName = S3D_WINDOW_CLASS_NAME;
	if(RegisterClass(&pSwapChain->m_S3DWindowClass) == 0)
	{
		DEBUG_MESSAGE(_T("%s: S3DWindow class registration failed, eror code = %u\n"), _T( __FUNCTION__ ) , GetLastError());
		return -1;
	}
	else
		pSwapChain->m_bClassWasRegistered = true;

	// Resident Evil 5 requires zero parent
	HWND  hParent = 0; // pSwapChain->m_Description.OutputWindow, if you want second window synchronously minimized and maximized with main window
	DWORD dwStyle = WS_POPUP; // second window must be visible for swapchain drawing
#ifdef FINAL_RELEASE
	pSwapChain->m_hS3DSecondWindow = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, S3D_WINDOW_CLASS_NAME, TEXT("S3D Driver Window"), 
		dwStyle, 0, 0, 1, 1, hParent, 0, (HINSTANCE)gData.hModule, 0);
#else
	pSwapChain->m_hS3DSecondWindow = CreateWindowEx(WS_EX_TOOLWINDOW, S3D_WINDOW_CLASS_NAME, TEXT("S3D Driver Window"), 
		dwStyle, 0, 0, 1, 1, hParent, 0, (HINSTANCE)gData.hModule, 0);
#endif
	if(!pSwapChain->m_hS3DSecondWindow) 
	{
		DEBUG_MESSAGE(_T("%s: CreateWindowEx() failed, eror code = %u\n"), _T( __FUNCTION__ ) , GetLastError());
		return -1;
	}

	SetWindowLongPtr(pSwapChain->m_hS3DSecondWindow, GWLP_USERDATA, (LONG_PTR)pSwapChain);
	DEBUG_MESSAGE(_T("Second window created: %p"), pSwapChain->m_hS3DSecondWindow);
	pSwapChain->m_bS3DSecondWindowVisible = false;

	SetEvent(pSwapChain->m_hSecondWindowInitialized);

	if (!pSwapChain->m_pD3DDeviceWrapper->IsCurrentThreadMain())
	{
		DEBUG_MESSAGE(_T("Launching S3D message queue") );
		{
			MSG   msg;
			DWORD status;
			while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
			{
				if (status == -1)
				{
					ZLOG_MESSAGE(zlog::SV_ERROR, _T("S3D message queue GetMessage failed"));
					return -1;
				}
				DispatchMessage (&msg);
			}
		}
		DEBUG_MESSAGE(_T("S3D message queue quit") );
	}

	return 0;
}

LRESULT CALLBACK S3DWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_USER:
		{
			D3DSwapChain* pSwapChain = (D3DSwapChain*)lParam;
			RECT rcWindow;
			BOOL res;
			switch(wParam) 
			{
			case SWM_MOVE_SECOND_WINDOW:
				rcWindow = pSwapChain->m_S3DSecondWindowRect; // <-- race condition??
				res = SetWindowPos(hWnd, HWND_NOTOPMOST, rcWindow.left, rcWindow.top, 
					rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, SWP_NOACTIVATE);
				DEBUG_TRACE1( _T("SetWindowPos(SECOND_WINDOW, HWND_TOPMOST, left = %d, top = %d, width = %d, height = %d, SWP_NOACTIVATE) = %d"),
					rcWindow.left, rcWindow.top, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, res );
				return 1;
			case SWM_SHOW_SECOND_WINDOW:
				res = ShowWindow(hWnd, SW_SHOWNA);
				DEBUG_TRACE1( _T("ShowWindow(SECOND_WINDOW, SW_SHOWNA) = %d"), res );
				return 1;
			case SWM_HIDE_SECOND_WINDOW:
				return E_FAIL;
			case SWM_MOVE_APP_WINDOW:
				return E_FAIL;
			case SWM_RESTORE_APP_WINDOW:
				return E_FAIL;
			}

			DEBUG_MESSAGE(_T("Invalid user message: wParam = %d\n"),wParam);
			return -1;
		}

		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 1;

		case WM_DESTROY:
			PostQuitMessage (0);
			return 0;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}
