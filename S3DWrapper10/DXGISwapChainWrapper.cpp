#include "StdAfx.h"
#include "Commands\Blt.h"
#include "DXGISwapChainWrapper.h"
#include "madchook.h"
#include "..\OutputMethods\OutputLib\OutputMethod_dx10.h"
#include "..\CommonUtils\System.h"
#include "..\S3DAPI\ScalingAgent.h"
#include "..\S3DAPI\ScalingHook.h"
#include "..\S3DAPI\ShutterAPI.h"
#include "presenter.h"

// {EEDFF4A3-D3CB-40e4-8D97-0FBB057FAF16}
static const GUID DXGISwapChainWrapperGUID = 
{ 0xeedff4a3, 0xd3cb, 0x40e4, { 0x8d, 0x97, 0xf, 0xbb, 0x5, 0x7f, 0xaf, 0x16 } };

HRESULT (STDMETHODCALLTYPE *Original_SetFullscreenState)( IDXGISwapChain* This, BOOL Fullscreen, IDXGIOutput *pTarget );
STDMETHODIMP Hooked_SetFullscreenState( IDXGISwapChain* This,BOOL Fullscreen, IDXGIOutput *pTarget );

HRESULT (STDMETHODCALLTYPE *Original_GetDesc)( ID3D10Texture2D* This, D3D10_TEXTURE2D_DESC *pDesc );
STDMETHODIMP Hooked_GetDesc( ID3D10Texture2D* This, D3D10_TEXTURE2D_DESC *pDesc );

DXGISwapChainWrapper::DXGISwapChainWrapper(void)
	: m_pDXGIDevice(NULL)
{
	memset( &m_OriginalSCDesc, 0, sizeof( m_OriginalSCDesc ) );
}

STDMETHODIMP DXGISwapChainWrapper::Init( IUnknown *pReal )
{
	HRESULT hResult = S_OK;
	NSCALL(pReal->QueryInterface(&m_pReal));
	if (!Original_SetFullscreenState)
	{
		void** p = *(void***)(pReal);
		HookCode(p[10], Hooked_SetFullscreenState, (PVOID*)&Original_SetFullscreenState, HOOKING_FLAG);
	}

	if (gInfo.DisplayScalingMode != 0)
	{
		CComPtr<ID3D10Texture2D> pBackBuffer;
		m_pReal->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );

		if (pBackBuffer)
		{
			void** p = *(void***)(pBackBuffer.p);
			HookCode(p[12], Hooked_GetDesc, (PVOID*)&Original_GetDesc, HOOKING_FLAG);

			pBackBuffer->SetPrivateData(DXGISwapChainWrapperGUID, sizeof(void*), this);
		}
	}

	void* pThis = this;
	m_pReal->SetPrivateData(DXGISwapChainWrapperGUID, sizeof(void*), (void*)(&pThis));
	DXGI_SWAP_CHAIN_DESC desc;
	m_pReal->GetDesc(&desc);
	ScalingAgent::Instance()->AddHWND(desc.OutputWindow);
	return hResult;
}

STDMETHODIMP DXGISwapChainWrapper::GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc)
{	
	SetCurrentSwapChain setSwapChain(m_pReal);
	
	//--- lie to the application ---
	if( m_OriginalSCDesc.BufferDesc.Width || m_OriginalSCDesc.BufferDesc.Height )
	{
		*pDesc = m_OriginalSCDesc;
		DEBUG_TRACE3(_T("\tDXGISwapChainWrapper::GetDesc(pDesc = %s)\n"), GetDXGISwapChainDescString(pDesc));
		return S_OK;
	}

	//--- old code here ---
	HRESULT hResult;
	NSCALL_TRACE3( m_pReal->GetDesc(pDesc), 
				   DEBUG_MESSAGE(_T("IDXGISwapChain::GetDesc(pDesc = %s)"), GetDXGISwapChainDescString(pDesc)) );

	if ( SUCCEEDED(hResult) )
	{
		// lie about actual swap chain resolution when scaling enabled
		if( m_pScalingHook ) 
		{
			pDesc->BufferDesc.Width = m_pScalingHook->originalSize.cx;
			pDesc->BufferDesc.Height = m_pScalingHook->originalSize.cy;
			DEBUG_TRACE3(_T("\tModified pDesc = %s"), GetDXGISwapChainDescString(pDesc));
		}
	}

	return hResult;
}

STDMETHODIMP Hooked_GetDesc( ID3D10Texture2D* This, D3D10_TEXTURE2D_DESC *pDesc )
{
	HRESULT hResult;
	NSCALL(Original_GetDesc(This, pDesc));

	if ( SUCCEEDED(hResult) )
	{
		DXGISwapChainWrapper* pWrapper = NULL;
		UINT size = sizeof(void*);
		This->GetPrivateData(DXGISwapChainWrapperGUID, &size, pWrapper);
		// lie about actual backbuffer resolution when scaling enabled
		if ( pWrapper && pWrapper->m_pScalingHook )
		{
			pDesc->Width = pWrapper->m_pScalingHook->originalSize.cx;
			pDesc->Height = pWrapper->m_pScalingHook->originalSize.cy;
		}
	}

	return hResult;
}

void DXGISwapChainWrapper::setD3DSwapChain( IDXGIDevice* pDevice, D3DSwapChain* p )
{ 
	assert(p);
	m_pDXGIDevice = pDevice;
	m_pD3DSwapChain = p;
}

DXGISwapChainWrapper::~DXGISwapChainWrapper(void)
{
	IDXGISwapChain* pSwapChain = m_pReal;
	D3DDeviceWrapper* pD3DDev = m_pD3DSwapChain->m_pD3DDeviceWrapper;
	CComPtr<IDXGIDevice> pDXGIDevice = pD3DDev->GetDXGIInterface();

	if (pD3DDev->m_DeviceMode == DEVICE_MODE_FORCEFULLSCREEN)
	{
		// force windowed mode
		pD3DDev->m_DeviceMode = DEVICE_MODE_FORCEWINDOWED;
		SetFullscreenStateEx(FALSE,0);
	}

	DXGI_SWAP_CHAIN_DESC desc;
	pSwapChain->GetDesc(&desc);
	ScalingAgent::Instance()->RemoveHWND(desc.OutputWindow);

	SetCurrentSwapChain setSwapChain(m_pReal);
	g_SwapChainMode = scDestroing;
	m_pD3DSwapChain->StopPresenter();
	m_pReal.Release();
	g_SwapChainMode = scNormal;

	SwapChainsMap::iterator it = pD3DDev->m_SwapChainsMap.find(pSwapChain);
	if (it != pD3DDev->m_SwapChainsMap.end())
		pD3DDev->m_SwapChainsMap.erase(it);
}


static void CPU2GPUSync2( D3DSwapChain*	pD3DSwapChain )
{
	// Sync by event
	if(!pD3DSwapChain->m_Description.Windowed)
	{
		g_KMShutter.Wait();
		//Sleep( 1000 * pD3DSwapChain->m_Description.BufferDesc.RefreshRate.Denominator / pD3DSwapChain->m_Description.BufferDesc.RefreshRate.Numerator );
		// Wait for 1 / monitor_refresh_rate
	}
}

static void CPU2GPUSync( IDXGISwapChain *pSwapChain, UINT buffer, UINT shutterindex )
{
	// Sync by backbuffer copy Lock()
	HRESULT						hResult;
	CComPtr<ID3D10Device>		pDevice;
	CComPtr<ID3D11Device>		pDevice11;
	LARGE_INTEGER				starttime, endtime, freq;

	CComQIPtr<IDXGIDeviceSubObject>	pSubObj = pSwapChain;
	if( pSubObj )
	{
		NSCALL( pSubObj->GetDevice( __uuidof(pDevice11), reinterpret_cast<void**>(&pDevice11)));
		if( !pDevice11 )
		{
			NSCALL( pSubObj->GetDevice( __uuidof(pDevice), reinterpret_cast<void**>(&pDevice)));
		}
	}

	QueryPerformanceCounter( &starttime );
	QueryPerformanceFrequency( &freq );

	WE_CALL;
	if( pDevice )
	{
		// Get the surface from the swap chain
		CComPtr<ID3D10Texture2D>	pRT;			// render target
		CComPtr<ID3D10Texture2D>	pSR;			// staging resource
		NSCALL( pSwapChain->GetBuffer( buffer, __uuidof(pRT), reinterpret_cast<void**>(&pRT)));

		D3D10_TEXTURE2D_DESC	desc;
		pRT->GetDesc( &desc );

		desc.Width			= 16;
		desc.Height			= 16;
		desc.MiscFlags		= 0;
		desc.BindFlags		= 0;
		desc.CPUAccessFlags	= D3D10_CPU_ACCESS_READ;
		desc.Usage			= D3D10_USAGE_STAGING;

		NSCALL( pDevice->CreateTexture2D( &desc, NULL, &pSR ));
		if( pSR )
		{
			D3D10_BOX box = { 0, 0, 0, 16, 16, 1 };			// UINT left, top, front, right, bottom, back;

			pDevice->CopySubresourceRegion(
				pSR,			// ID3D10Resource *pDstResource,
				0,				// UINT DstSubresource,
				0,				// UINT DstX,
				0,				// UINT DstY,
				0,				// UINT DstZ,
				pRT,			// ID3D10Resource *pSrcResource,
				0,				// UINT SrcSubresource,
				&box			// const D3D10_BOX *pSrcBox
			);
			pDevice->Flush();

			D3D10_MAPPED_TEXTURE2D	mappedinfo;
// 			while( DXGI_ERROR_WAS_STILL_DRAWING == pSR->Map(0, D3D10_MAP_READ, D3D10_MAP_FLAG_DO_NOT_WAIT, &mappedinfo))
// 				Sleep(0);
			NSCALL( pSR->Map(0, D3D10_MAP_READ, 0, &mappedinfo) );
			BYTE* pData = (BYTE *)mappedinfo.pData + mappedinfo.RowPitch;
			UINT  markedindex = pData[0];		// the first component of the first pixel in the second row
#ifndef FINAL_RELEASE
			markedindex /= 64;
#endif
			pSR->Unmap(0);
			DEBUG_MESSAGE(_T("CPU2GPUSync(): shutterindex = %u, markedindex = %u\n"), shutterindex, markedindex );
			_ASSERT( shutterindex == markedindex ); 
		}
	}
	else if( pDevice11 )
	{
		CComPtr<ID3D11Texture2D>	pRT11;
		CComPtr<ID3D11Texture2D>	pSR11;			
		NSCALL( pSwapChain->GetBuffer( buffer, __uuidof(pRT11), reinterpret_cast<void**>(&pRT11)));

		D3D11_TEXTURE2D_DESC	desc;
		pRT11->GetDesc( &desc );

		desc.Width			= 16;
		desc.Height			= 16;
		desc.MiscFlags		= 0;
		desc.BindFlags		= 0;
		desc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ;
		desc.Usage			= D3D11_USAGE_STAGING;

		NSCALL( pDevice11->CreateTexture2D( &desc, NULL, &pSR11 ));
		if( pSR11 )
		{
			CComPtr<ID3D11DeviceContext>	pContext;
			pDevice11->GetImmediateContext( &pContext );

			D3D11_BOX box = { 0, 0, 0, 16, 16, 1 };			// UINT left, top, front, right, bottom, back;

			pContext->CopySubresourceRegion(
				pSR11,			// ID3D11Resource *pDstResource,
				0,				// UINT DstSubresource,
				0,				// UINT DstX,
				0,				// UINT DstY,
				0,				// UINT DstZ,
				pRT11,			// ID3D11Resource *pSrcResource,
				0,				// UINT SrcSubresource,
				&box			// const D3D11_BOX *pSrcBox
			);
			pContext->Flush();

			D3D11_MAPPED_SUBRESOURCE	mappedinfo11;
// 			while( D3DERR_WASSTILLDRAWING == pContext->Map( pSR11, 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &mappedinfo11 ))
// 				Sleep(0);
			NSCALL( pContext->Map( pSR11, 0, D3D11_MAP_READ, 0, &mappedinfo11 ) );
			BYTE* pData = (BYTE *)mappedinfo11.pData + mappedinfo11.RowPitch;
			UINT  markedindex = pData[0];		// the first component of the first pixel in the second row
#ifndef FINAL_RELEASE
			markedindex /= 64;
#endif
			pContext->Unmap( pSR11, 0 );
			DEBUG_MESSAGE(_T("CPU2GPUSync(): shutterindex = %u, markedindex = %u\n"), shutterindex, markedindex );
			_ASSERT( shutterindex == markedindex ); 
		}
	}
	QueryPerformanceCounter( &endtime );
	double timedelta = ((double)endtime.QuadPart - (double)starttime.QuadPart) / (double)freq.QuadPart;
	DEBUG_MESSAGE(_T("CPU2GPUSync(): timedelta = %2.3f ms\n"), 1000.0f * timedelta );
}

STDMETHODIMP DXGISwapChainWrapper::Present( UINT SyncInterval, UINT Flags )
{
	SetCurrentSwapChain setSwapChain(m_pReal); 
	m_pD3DSwapChain->m_bPresentationCall = true;
	HRESULT hResult = S_OK;

	if( CPresenterX::Get() )
	{
		bool bSkipPresenter = m_pD3DSwapChain->m_pD3DDeviceWrapper->SkipPresenterThread();
		m_pD3DSwapChain->m_bUseSimplePresenter = bSkipPresenter;
		if (gInfo.DisplayScalingMode > 0 && !m_pD3DSwapChain->m_Description.Windowed)
			Commands::Blt::IsPresentBlt = true;
		if (!bSkipPresenter)
		{
			if( m_pD3DSwapChain->m_bPresenterCall && m_pD3DSwapChain->m_bPresenterThreadId == GetCurrentThreadId() )
			{
				DEBUG_TRACE3(_T("IDXGISwapChain::Present() - Call from CPresenterX\n"));
				NSCALL( m_pReal->Present(SyncInterval, Flags) );
			}
			else 
			{
				DEBUG_TRACE3(_T("IDXGISwapChain::Present() - SendImageToPresenter\n"));
				NSCALL( m_pReal->Present(0, 0) );
				if (SUCCEEDED(hResult))
					m_pD3DSwapChain->m_pD3DDeviceWrapper->CheckEngine();
			}
		} else
		{
			NSCALL( m_pReal->Present(SyncInterval, Flags) );
		}
		if (gInfo.DisplayScalingMode > 0 && !m_pD3DSwapChain->m_Description.Windowed)
			Commands::Blt::IsPresentBlt = false;
		return hResult;
	}

	if (IS_SHUTTER_OUTPUT && !USE_ATI_PRESENTER )	
		SyncInterval = D3DDDI_FLIPINTERVAL_ONE;					// KM flipper needs VSync on!
	else if ( gInfo.ForceVSyncOff || USE_ATI_PRESENTER )
		SyncInterval = D3DDDI_FLIPINTERVAL_IMMEDIATE;	
	
	if (gInfo.DisplayScalingMode > 0 && !m_pD3DSwapChain->m_Description.Windowed)
		Commands::Blt::IsPresentBlt = true;

	NSCALL_TRACE3(m_pReal->Present(SyncInterval, Flags),
		DEBUG_MESSAGE(_T("IDXGISwapChain::Present(SyncInterval = %u, Flags = %u)"), 
		SyncInterval, Flags));

	if( USE_IZ3DKMSERVICE_PRESENTER && !m_pD3DSwapChain->m_Description.Windowed )
	{
//		CPU2GPUSync2( m_pD3DSwapChain );
	}

	//if (hResult == DXGI_STATUS_OCCLUDED)
	//{
	//	DEBUG_TRACE1(_T("IDXGISwapChain::Present(SyncInterval = %u, Flags = %u) = DXGI_STATUS_OCCLUDED\n"), 
	//		SyncInterval, Flags);
	//}
	if (SUCCEEDED(hResult))
	{
		if ( IS_SHUTTER_OUTPUT && (m_pD3DSwapChain->m_Description.Windowed || USE_IZ3DKMSERVICE_PRESENTER ) )
		{
			m_pD3DSwapChain->m_pD3DDeviceWrapper->m_pOutputMethod->AfterPresent( m_pD3DSwapChain->m_pD3DDeviceWrapper, 
				m_pD3DSwapChain->m_bLeftEyeActive );

			if( !USE_ATI_PRESENTER || D3DDeviceWrapper::IsAmdStereoInited( ) )
			{
				m_pD3DSwapChain->m_bLeftEyeActive = false;

				NSCALL_TRACE3(m_pReal->Present(SyncInterval, Flags),
					DEBUG_MESSAGE(_T("\tIDXGISwapChain::Present(SyncInterval = %u, Flags = %u)"), 
					SyncInterval, Flags));

				if( USE_IZ3DKMSERVICE_PRESENTER && !m_pD3DSwapChain->m_Description.Windowed )
				{
//					CPU2GPUSync2( m_pD3DSwapChain );
				}

				m_pD3DSwapChain->m_pD3DDeviceWrapper->m_pOutputMethod->AfterPresent( m_pD3DSwapChain->m_pD3DDeviceWrapper, 
					m_pD3DSwapChain->m_bLeftEyeActive );
				m_pD3DSwapChain->m_bLeftEyeActive = true;
			}
		}
		m_pD3DSwapChain->Present(SyncInterval, Flags);			// call Present() for the second swap chain (second window) 
		m_pD3DSwapChain->m_pD3DDeviceWrapper->CheckEngine();
	}
	m_pD3DSwapChain->m_bPresentationCall = false;
	return hResult;
}			

STDMETHODIMP DXGISwapChainWrapper::ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags )
{
	DEBUG_TRACE1(_T("ResizeBuffers(): ENTER\n"));

	SetCurrentSwapChain setSwapChain(m_pReal); 
	HRESULT hResult = S_OK;

	// store the original parameters for GetDesc()
	// !!! FIXME !!! m_OriginalSCDesc should not be changed if m_pReal->ResizeBuffers() fails!
	m_OriginalSCDesc.BufferCount = BufferCount;
	m_OriginalSCDesc.BufferDesc.Width = Width;
	m_OriginalSCDesc.BufferDesc.Height = Height;
	m_OriginalSCDesc.BufferDesc.Format = NewFormat;
	m_OriginalSCDesc.Flags = SwapChainFlags;

	// get actual Width && Height
	if (Width == 0 || Height == 0)
	{
		// NOTE: GetClientRect is Hooked when scaling is enabled returning original resolution
		RECT rcCurrentClient;
		GetClientRect( m_pD3DSwapChain->m_Description.OutputWindow, &rcCurrentClient );
		m_OriginalSCDesc.BufferDesc.Width	= Width		= ( UINT )rcCurrentClient.right;
		m_OriginalSCDesc.BufferDesc.Height	= Height	= ( UINT )rcCurrentClient.bottom;
	}

	if( USE_IZ3DKMSERVICE_PRESENTER )
	{
		// Force quadbuffering for KM shutter service
// 		if( !m_pD3DSwapChain->m_Description.Windowed && !m_bSetFullscreenState )
// 			BufferCount	= KM_SHUTTER_SERVICE_BUFFER_COUNT;	// Don't change BufferCount for internal calls from SetFullscreenStateEx()

		g_KMShutter.StopShutter();				// Don't call StartShutter(), D3DSwapChain::PresentData() will call it!
		Sleep( 1000 / 50 );						// FIXME!
	}
	g_SwapChainMode = scResizing;
	NSCALL_TRACE1(m_pReal->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags),
		DEBUG_MESSAGE(_T("IDXGISwapChain::ResizeBuffers(BufferCount = %u, Width = %u, Height = %u, NewFormat = %S, SwapChainFlags = %u)"), 
		BufferCount, Width, Height, EnumToString(NewFormat), SwapChainFlags));
	if (SUCCEEDED(hResult) && !IsInternalCall())
	{
		m_pD3DSwapChain->ResizeBuffers( BufferCount, Width, Height, NewFormat, SwapChainFlags );
		if ((SwapChainFlags & DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) && CPresenterX::Get())
		{
			DXGI_SWAP_CHAIN_DESC	desc;
			m_pReal->GetDesc(&desc);
			CPresenterX::Get()->ReInit( desc.BufferDesc.RefreshRate );
		}
// 		if( USE_IZ3DKMSERVICE_PRESENTER )
// 		{
// 			g_KMShutter.StopShutter();				// Don't call StartShutter(), D3DSwapChain::PresentData() will call it!
// 		}
	}
	
	g_SwapChainMode = scNormal;
	DEBUG_TRACE1(_T("ResizeBuffers(): EXIT\n"));
	return hResult;
}

STDMETHODIMP DXGISwapChainWrapper::ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters )
{
	DEBUG_TRACE1(_T("ResizeTarget(): ENTER\n"));

	SetCurrentSwapChain setSwapChain(m_pReal); 
	HRESULT hResult = S_OK;
	CComPtr<IDXGIOutput> pDXGIOutput;
	DXGI_MODE_DESC	ModeDesc = *pNewTargetParameters;
	pNewTargetParameters = &ModeDesc;
	
	// store the original for GetDesc()
	// !!! FIXME !!! m_OriginalSCDesc should not be changed if m_pReal->ResizeTarget() fails!
	m_OriginalSCDesc.BufferDesc = *pNewTargetParameters;

	if( USE_IZ3DKMSERVICE_PRESENTER )
	{
		g_KMShutter.StopShutter();				// Don't call StartShutter(), D3DSwapChain::PresentData() will call it!
		Sleep( 1000 / 50 );						// FIXME!
	}

	// handle scaling case
	m_pScalingHook.reset();
	if (!m_pD3DSwapChain->m_Description.Windowed && gInfo.DisplayScalingMode != 0)
	{
		DEBUG_MESSAGE(_T("IDXGISwapChainWrapper: complex resize for scaling\n"));
		BOOL NeedResize = pNewTargetParameters->Width != m_pD3DSwapChain->m_BackBufferSizeBeforeScaling.cx ||
						  pNewTargetParameters->Height != m_pD3DSwapChain->m_BackBufferSizeBeforeScaling.cy;
		NSCALL(m_pReal->GetContainingOutput(&pDXGIOutput));

		// Remember requested resolution
		m_pD3DSwapChain->m_BackBufferSizeBeforeScaling.cx = pNewTargetParameters->Width;
		m_pD3DSwapChain->m_BackBufferSizeBeforeScaling.cy = pNewTargetParameters->Height;

		// Because DXGISwapChainWrapper::ResizeTarget can be called inside the hooked function, releasing the hook
		// will not destroy it immediately. So we initialize it no matter need we it or no to get proper values from
		// hooked functions
		SIZE nativeSize = {gInfo.DisplayNativeWidth, gInfo.DisplayNativeHeight};
		m_pScalingHook  = ScalingAgent::Instance()->InitializeHook( TDisplayScalingMode(gInfo.DisplayScalingMode), 
																	m_pD3DSwapChain->m_BackBufferSizeBeforeScaling, 
																	nativeSize );

		// With scaling enabled WM_SIZE message is not send to the app, because we don't change resolution, so pass it explicitly
		if (NeedResize) 
		{
			DXGI_SWAP_CHAIN_DESC desc;
			m_pReal->GetDesc(&desc);
			DEBUG_MESSAGE(_T("IDXGISwapChainWrapper: send WM_SIZE(%d, %d) message\n"), pNewTargetParameters->Width, pNewTargetParameters->Height);
			SendMessage( desc.OutputWindow, WM_SIZE, SIZE_RESTORED, MAKELPARAM(pNewTargetParameters->Width, pNewTargetParameters->Height) );
		}

		// Switch to native resolution
		ModeDesc.Width = gInfo.DisplayNativeWidth;
		ModeDesc.Height = gInfo.DisplayNativeHeight;
		ModeDesc.RefreshRate = gInfo.DisplayNativeFrequency;
		ModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DEBUG_MESSAGE(_T("IDXGISwapChainWrapper: resize to native mode\n"));
	}

	// Find appropriate mode for shutter
	if (IS_SHUTTER_OUTPUT)
	{
		if (!pDXGIOutput) {
			NSCALL(m_pReal->GetContainingOutput(&pDXGIOutput));
		}

		DEBUG_TRACE1(_T("FindShutterClosestMatchingMode <source>: ( pDesc = (%s) )\n"), GetDXGIModeString(&ModeDesc) );
		hResult = D3DDeviceWrapper::FindShutterClosestMatchingMode( m_pDXGIDevice, &ModeDesc, pDXGIOutput );
		DEBUG_TRACE1(_T("FindShutterClosestMatchingMode <modified>: ( pDesc = (%s) )\n"), GetDXGIModeString(&ModeDesc) );
		if ( FAILED(hResult) ) {
			return hResult;
		}
	}

	hResult = ResizeTargetImpl(pNewTargetParameters);
	DEBUG_TRACE1(_T("ResizeTarget(): EXIT\n"));
	return hResult;
}

HRESULT DXGISwapChainWrapper::ResizeTargetImpl(const DXGI_MODE_DESC *pNewTargetParameters)
{
	if(m_pD3DSwapChain->m_pD3DDeviceWrapper->m_bTwoWindows && !m_pD3DSwapChain->m_Description.Windowed)
		return ResizeTargetDualMonitors(pNewTargetParameters);
	
	HRESULT hResult = S_OK;
	//if( CPresenterX::Get() )
	//{
	//	CPresenterX::Get()->SetResizeTargetParams( pNewTargetParameters );
	//	CPresenterX::Get()->SetAction( ptResizeTarget );
	//	CPresenterX::Get()->WaitForActionDone();
	//}
	//else
	{
		g_SwapChainMode = scResizing;
		NSCALL_TRACE1(m_pReal->ResizeTarget(pNewTargetParameters), 
			DEBUG_MESSAGE(_T("IDXGISwapChain::ResizeTarget(pNewTargetParameters = (%s))"), 
			GetDXGIModeString(pNewTargetParameters)));
		if (SUCCEEDED(hResult))
		{
			m_pReal->GetDesc(&m_pD3DSwapChain->m_Description);
			m_pD3DSwapChain->ResizeTarget( pNewTargetParameters );
			if (CPresenterX::Get())
				CPresenterX::Get()->ReInit( m_pD3DSwapChain->m_Description.BufferDesc.RefreshRate );
		}
// 		if( USE_IZ3DKMSERVICE_PRESENTER )
// 		{
// 			g_KMShutter.StopShutter();				// Don't call StartShutter(), D3DSwapChain::PresentData() will call it!
// 		}
		g_SwapChainMode = scNormal;
	}
	return hResult;
}

HRESULT DXGISwapChainWrapper::ResizeTargetDualMonitors( const DXGI_MODE_DESC * pNewTargetParameters )
{
	BOOL FullscreenFirst;
	CComPtr<IDXGIOutput> pTargetFirst;
	m_pReal->GetFullscreenState(&FullscreenFirst, &pTargetFirst);
	BOOL FullscreenSecond;
	CComPtr<IDXGIOutput> pTargetSecond;
	m_pReal->GetFullscreenState(&FullscreenSecond, &pTargetSecond);
	bool bSkip = false;
	if (FullscreenFirst && FullscreenSecond)
	{
		DXGI_OUTPUT_DESC DescFirst;
		pTargetFirst->GetDesc(&DescFirst);
		DXGI_OUTPUT_DESC DescSecond;
		pTargetFirst->GetDesc(&DescSecond);
		if ((DescFirst.DesktopCoordinates.right - DescFirst.DesktopCoordinates.left) == pNewTargetParameters->Width &&
			(DescFirst.DesktopCoordinates.bottom - DescFirst.DesktopCoordinates.top) == pNewTargetParameters->Height)
			bSkip = true;
		else if ((DescSecond.DesktopCoordinates.right - DescSecond.DesktopCoordinates.left) == pNewTargetParameters->Width &&
			(DescSecond.DesktopCoordinates.bottom - DescSecond.DesktopCoordinates.top) == pNewTargetParameters->Height)
			bSkip = true;
	}
	else bSkip = true;
	if (bSkip)
	{
		DEBUG_TRACE1(_T("IDXGISwapChain::ResizeTargetSkipped(pNewTargetParameters = (%s))\n"), 
			GetDXGIModeString(pNewTargetParameters));
		return S_OK;
	}

	DEBUG_TRACE1(_T("\tBegin Complex ResizeTarget\n"));
	HRESULT hResult = S_OK;
	Timer timer;
	timer.Start();
	g_SwapChainMode = scResizing;
	m_pD3DSwapChain->CheckSwapChainMode();
	m_pD3DSwapChain->ResizeTarget( pNewTargetParameters );
	NSCALL_TRACE1(m_pReal->ResizeTarget(pNewTargetParameters), 
		DEBUG_MESSAGE(_T("IDXGISwapChain::ResizeTarget(pNewTargetParameters = (%s))"), 
		GetDXGIModeString(pNewTargetParameters)));
	if (SUCCEEDED(hResult))
		m_pReal->GetDesc(&m_pD3DSwapChain->m_Description);
	m_pD3DSwapChain->CheckSwapChainMode();

	int c = 0;
	while(true)
	{
		c++;
		m_pD3DSwapChain->Present(0, /*0*/DXGI_PRESENT_TEST); // DXGI switch first swapchain to window mode, because of ResizeTarget
		if (m_pD3DSwapChain->IsSwapChainsChangeMode())
			break;
		c++;
		NSCALL_TRACE1(m_pReal->Present(0, /*0*/DXGI_PRESENT_TEST),
			DEBUG_MESSAGE(_T("IDXGISwapChain::Present(SyncInterval = %u, Flags = %u)"), 
			0, 0)); // DXGI switch second swapchain to window mode, because of ResizeTarget
		if (m_pD3DSwapChain->IsSwapChainsChangeMode())
			break;
		if (c >= 6)
			break;
	}
	DEBUG_MESSAGE(_T("Loop len: %d\n"), c); // always 3 for me

	// try switch back to fullscreen mode
	NSCALL_TRACE1(Original_SetFullscreenState(m_pReal, TRUE, NULL),
		DEBUG_MESSAGE(_T("IDXGISwapChain::SetFullscreenState(m_pReal = %p, Fullscreen = %i, pTarget = %p)"), 
		m_pReal, TRUE, NULL));
	m_pD3DSwapChain->CheckSwapChainMode();
	m_pD3DSwapChain->SetFullscreenState(TRUE, NULL);
	m_pD3DSwapChain->CheckSwapChainMode();
	// TODO: Add check correct mode after switch
	g_SwapChainMode = scNormal;
	timer.Stop();
	DEBUG_TRACE1(_T("\tEnd Complex ResizeTarget (%f ms)\n"), timer.GetDiffMs());
	return hResult;
}

HRESULT DXGISwapChainWrapper::SetFullscreenStateEx( BOOL Fullscreen, IDXGIOutput * pTarget )
{
	DEBUG_TRACE1(_T("SetFullscreenStateEx(): ENTER\n"));

	HRESULT hResult = S_OK;

	// !!! FIXME !!! m_OriginalSCDesc should not be changed if SetFullscreenStateExt() fails!
	m_OriginalSCDesc.Windowed = !Fullscreen;

	DeviceModes deviceMode = m_pD3DSwapChain->m_pD3DDeviceWrapper->m_DeviceMode;
	if (deviceMode == DEVICE_MODE_FORCEFULLSCREEN)
		Fullscreen = true;
	else if (deviceMode == DEVICE_MODE_FORCEWINDOWED)
		Fullscreen = false;
	m_pD3DSwapChain->m_Description.Windowed = !Fullscreen;
	m_pD3DSwapChain->CheckSecondWindowPosition();

	SetCurrentSwapChain setSwapChain(m_pReal);
	if (gInfo.AlternativeSetFullscreenStateOrder)
	{
		NSCALL_TRACE1(Original_SetFullscreenState(m_pReal, Fullscreen, pTarget),
			DEBUG_MESSAGE(_T("IDXGISwapChain::SetFullscreenStateEx(m_pReal = %p, Fullscreen = %i, pTarget = %p)"), 
			m_pReal, Fullscreen, pTarget));
		if (SUCCEEDED(hResult))
			m_pD3DSwapChain->SetFullscreenState(Fullscreen, pTarget);
	}
	else
	{
		m_pD3DSwapChain->SetFullscreenState(Fullscreen, pTarget);
		NSCALL_TRACE1(Original_SetFullscreenState(m_pReal, Fullscreen, pTarget),
			DEBUG_MESSAGE(_T("IDXGISwapChain::SetFullscreenStateEx(m_pReal = %p, Fullscreen = %i, pTarget = %p)"), 
			m_pReal, Fullscreen, pTarget));
	}
	if (SUCCEEDED(hResult))
	{
		m_pReal->GetDesc(&m_pD3DSwapChain->m_Description);
		if (CPresenterX::Get())
			CPresenterX::Get()->ReInit( m_pD3DSwapChain->m_Description.BufferDesc.RefreshRate );
	}

	DEBUG_TRACE1(_T("SetFullscreenStateEx(): EXIT\n"));
	return hResult;
}

STDMETHODIMP DXGISwapChainWrapper::SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget )
{
	HRESULT hResult = S_OK;
	g_SwapChainMode = scChangeFullscreenMode;
	if(IsInternalCall())
		hResult = Original_SetFullscreenState(m_pReal, Fullscreen, pTarget);
	else
		hResult = SetFullscreenStateEx(Fullscreen, pTarget);
	g_SwapChainMode = scNormal;	
	return hResult;
}

// for Alt+Enter
STDMETHODIMP Hooked_SetFullscreenState( IDXGISwapChain* This,BOOL Fullscreen, IDXGIOutput *pTarget )
{
	HRESULT hResult = S_OK;
	g_SwapChainMode = scChangeFullscreenMode;
	UINT size = sizeof(void*);
	DXGISwapChainWrapper* pWrapper = NULL;
	This->GetPrivateData(DXGISwapChainWrapperGUID, &size, &pWrapper);
	if(IsInternalCall() || !pWrapper)
		hResult = Original_SetFullscreenState(This, Fullscreen, pTarget);
	else
		hResult = pWrapper->SetFullscreenStateEx(Fullscreen, pTarget);
	g_SwapChainMode = scNormal;
	return hResult;
}
