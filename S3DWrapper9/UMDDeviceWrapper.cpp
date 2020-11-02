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
#include "UMDDeviceWrapper.h"
#include "UMDShutterPresenter.h"
#include "BaseStereoRenderer.h"
//#define  DISABLE_PRESENTER
#define	 KM_UM_PRESENTER_BACKBUFFER_COUNT 6

UMDDeviceWrapper*	g_pUMDeviceWrapper = NULL;
#ifndef FINAL_RELEASE
CallProfiler		g_Profiler;
const wchar_t*		g_CurrentUMFunctionName = L"No wait";
ResourcesTypeMap	g_ResourceType;
QueriesTypeMap		g_QueryType;
#endif

//--------------------------------------------- Device wrapper ------------------------------------
UMDDeviceWrapper::UMDDeviceWrapper(UMWrapper* pAdapter, D3DDDIARG_CREATEDEVICE* pDevice, 
								   D3DKMT_HANDLE hKMAdapter, D3DKMT_HANDLE hKMDevice)
:	m_hDevice( pDevice->hDevice )
,	m_pPresenter	( NULL )
,	m_AdapterWrapper( pAdapter )
,	m_DeviceFunctions( *pDevice->pDeviceFuncs )
,	m_CommunicationSurface( 0 )
,	m_pAtiStereoCommPacket( 0 )
,	m_isStereoEnable( false )
,	m_UMCallInProgress ( false )
,	m_pStereoRenderer ( NULL )
,	m_hKMAdapter ( hKMAdapter )
,	m_hKMDevice ( hKMDevice )
,	m_BackbufferFormat	( D3DDDIFMT_UNKNOWN )
,	m_dwCommandCount ( 0 )
,	m_CommandCountStats ( 120 )
,	m_dwPresentBufferIndex ( 0 )
{
	ZeroMemory( &m_DoubledBackbuffer, sizeof( m_DoubledBackbuffer ) );
	m_BackbufferSize.cx = 0;
	m_BackbufferSize.cy = 0;
	m_Backbuffer.reserve( 1 + KM_UM_PRESENTER_BACKBUFFER_COUNT );
}

UMDDeviceWrapper::~UMDDeviceWrapper()
{
	m_pStereoRenderer->m_pPresenter = NULL;
	SAFE_DELETE( m_pPresenter );
}

//////////////////////////////////////////////////////////////////////////

// Lock function
//////////////////////////////////////////////////////////////////////////

HRESULT APIENTRY UMDDeviceWrapper::Lock( D3DDDIARG_LOCK* pLock )
{
	HRESULT hr = m_DeviceFunctions.pfnLock(m_hDevice, pLock);
	if( pLock->hResource == m_CommunicationSurface )
	{
		m_pAtiStereoCommPacket = (ATIDX9STEREOCOMMPACKET*)pLock->pSurfData;
	}
	return hr;
}

HRESULT APIENTRY LockCallback(HANDLE hDevice, D3DDDIARG_LOCK* pLock)
{
	DEBUG_TRACEUM(hDevice);
	WCHAR* pFunctionName = __FUNCTIONW__ L"Unknown";
#ifndef FINAL_RELEASE
	ResourcesTypeMap::const_iterator it = g_ResourceType.find(pLock->hResource);
	if (it != g_ResourceType.end())
	{
		D3DDDI_RESOURCEFLAGS flags = it->second;
		if (flags.RenderTarget)
			pFunctionName = __FUNCTIONW__ L"RenderTarget";
		else if (flags.ZBuffer)
			pFunctionName = __FUNCTIONW__ L"ZBuffer";
		else if (flags.Primary)
			pFunctionName = __FUNCTIONW__ L"Primary";
		else if (flags.Texture)
			pFunctionName = __FUNCTIONW__ L"Texture";
		else if (flags.CubeMap)
			pFunctionName = __FUNCTIONW__ L"CubeMap";
		else if (flags.Volume)
			pFunctionName = __FUNCTIONW__ L"Volume";
		else if (flags.VertexBuffer)
			pFunctionName = __FUNCTIONW__ L"VertexBuffer";
		else if (flags.IndexBuffer)
			pFunctionName = __FUNCTIONW__ L"IndexBuffer";
		else if (flags.SharedResource)
			pFunctionName = __FUNCTIONW__ L"SharedResource";
	}
#endif
	UMCallGuard cg(hDevice, pFunctionName);
	UMDDeviceWrapper* pDevWrapper = (UMDDeviceWrapper*)hDevice;
	HRESULT hResult = pDevWrapper->Lock( pLock );
	return hResult;
}
//////////////////////////////////////////////////////////////////////////
// Unlock function
//////////////////////////////////////////////////////////////////////////

HRESULT APIENTRY UMDDeviceWrapper::Unlock( CONST D3DDDIARG_UNLOCK* pUnLock )
{
	if( pUnLock->hResource == m_CommunicationSurface && m_pAtiStereoCommPacket )
	{
		if ( m_pAtiStereoCommPacket->dwSignature == 'STER' )
		{
			switch( m_pAtiStereoCommPacket->stereoCommand )
			{
			case ATI_STEREO_ENABLESTEREO:
				m_isStereoEnable = true;
				break;
			case ATI_STEREO_ENABLELEFTONLY:
				break;
			case ATI_STEREO_ENABLERIGHTONLY:
				break;
			case ATI_STEREO_ENABLESTEREOSWAPPED:
				break;
			case ATI_STEREO_GETLINEOFFSET:
				m_pAtiStereoCommPacket->dwOutBufferSize = sizeof(DWORD);
				memcpy(m_pAtiStereoCommPacket->pOutBuffer, &(m_BackbufferSize.cy), sizeof(DWORD));
				break;
			case ATI_STEREO_GETDISPLAYMODES:
				break;
			case ATI_STEREO_FORCEDWORD:
				break;
			default:
				break;
			}
			*m_pAtiStereoCommPacket->pResult = S_OK;
		}
		m_pAtiStereoCommPacket = 0;
	}
	HRESULT hr = m_DeviceFunctions.pfnUnlock(m_hDevice, pUnLock);
	return hr;
}

HRESULT APIENTRY UnlockCallback(HANDLE hDevice, CONST D3DDDIARG_UNLOCK* pUnLock)
{
	DEBUG_TRACEUM(hDevice);
	WCHAR* pFunctionName = __FUNCTIONW__ L"Unknown";
#ifndef FINAL_RELEASE
	ResourcesTypeMap::const_iterator it = g_ResourceType.find(pUnLock->hResource);
	if (it != g_ResourceType.end())
	{
		D3DDDI_RESOURCEFLAGS flags = it->second;
		if (flags.RenderTarget)
			pFunctionName = __FUNCTIONW__ L"RenderTarget";
		else if (flags.ZBuffer)
			pFunctionName = __FUNCTIONW__ L"ZBuffer";
		else if (flags.Primary)
			pFunctionName = __FUNCTIONW__ L"Primary";
		else if (flags.Texture)
			pFunctionName = __FUNCTIONW__ L"Texture";
		else if (flags.CubeMap)
			pFunctionName = __FUNCTIONW__ L"CubeMap";
		else if (flags.Volume)
			pFunctionName = __FUNCTIONW__ L"Volume";
		else if (flags.VertexBuffer)
			pFunctionName = __FUNCTIONW__ L"VertexBuffer";
		else if (flags.IndexBuffer)
			pFunctionName = __FUNCTIONW__ L"IndexBuffer";
		else if (flags.SharedResource)
			pFunctionName = __FUNCTIONW__ L"SharedResource";
	}
#endif
	UMCallGuard cg(hDevice, pFunctionName);
	UMDDeviceWrapper* pDevWrapper = (UMDDeviceWrapper*)hDevice;
	HRESULT hResult = pDevWrapper->Unlock( pUnLock );
	return hResult;
}
//////////////////////////////////////////////////////////////////////////

// DestroyResource function
//////////////////////////////////////////////////////////////////////////

HRESULT APIENTRY DestroyResourceCallback(HANDLE hDevice, HANDLE hResource)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	UMDDeviceWrapper* pDevWrapper = (UMDDeviceWrapper*)hDevice;
	HRESULT hResult = pDevWrapper->DestroyResource( hResource );
#ifndef FINAL_RELEASE
	g_ResourceType.erase(hResource);
#endif
	return hResult;
}

HRESULT APIENTRY UMDDeviceWrapper::DestroyResource( HANDLE hResource )
{
	HRESULT hr = m_DeviceFunctions.pfnDestroyResource( m_hDevice, hResource );
	if ( !m_Backbuffer.empty() && hResource == m_Backbuffer[0].hResource )
	{
		m_pStereoRenderer->m_pPresenter = NULL;
		SAFE_DELETE( m_pPresenter );
		m_Backbuffer.clear();
		m_BackbufferFormat = D3DDDIFMT_UNKNOWN;
		m_BackbufferSize.cx = 0;
		m_BackbufferSize.cy = 0;
	}
	else if ( hResource == m_CommunicationSurface )
		m_CommunicationSurface = NULL;
	return hr;
}

void UMDDeviceWrapper::AddBackBuffer(HANDLE hResource, DWORD	SubResourceCount)
{
	CResourceHandle	rh;
	rh.hResource	= hResource;
	for( DWORD d = 0; d < SubResourceCount; ++d )
	{
		rh.SubResourceIndex = d;
		m_Backbuffer.push_back( rh );
	}
}

//////////////////////////////////////////////////////////////////////////
// OpenResource function
//////////////////////////////////////////////////////////////////////////
HRESULT APIENTRY UMDDeviceWrapper::OpenResource( D3DDDIARG_OPENRESOURCE* pResource )
{
	HRESULT hr = m_DeviceFunctions.pfnOpenResource( m_hDevice, pResource );
	if( m_Backbuffer.empty() )
	{
		m_dwPresentBufferIndex = 0;
		AddBackBuffer(pResource->hResource, 1);
	}
	DEBUG_MESSAGE(_T("UM Opened Surface  = %p\n"), pResource->hResource);
	return hr;
}
//////////////////////////////////////////////////////////////////////////
// CreateResource function
//////////////////////////////////////////////////////////////////////////
HRESULT APIENTRY UMDDeviceWrapper::CreateResource( D3DDDIARG_CREATERESOURCE* pResource )
{
	HRESULT						hr;
	bool                        bBackBufferFound  = false;
	D3DDDIARG_CREATERESOURCE*   pRecourceToCreate = pResource;
	D3DDDI_SURFACEINFO			pSurfListNew[KM_UM_PRESENTER_BACKBUFFER_COUNT];
	D3DDDIARG_CREATERESOURCE	modifiedResource;

	if( m_pStereoRenderer->m_RendererEvents == reCreatingDevice && 
	    pResource->Flags.RenderTarget && pResource->Pool == D3DDDIPOOL_LOCALVIDMEM )
	{
		if(USE_UM_PRESENTER)
		{
			if( m_Backbuffer.size() < m_pStereoRenderer->GetBaseSC()->m_PresentationParameters[0].BackBufferCount )
			{
				m_BackbufferFormat = pResource->Format;
				m_BackbufferSize.cx = pResource->pSurfList->Width;
				m_BackbufferSize.cy = pResource->pSurfList->Height;
				bBackBufferFound = true;
			}
		}
		else if(USE_IZ3DKMSERVICE_PRESENTER)
		{
			//--- first opened resource (back buffer) index is 0 ---
			if(m_Backbuffer.size() == 1)
			{	
				m_BackbufferFormat = pRecourceToCreate->Format;
				m_BackbufferSize.cx = m_pStereoRenderer->GetBaseSC()->m_PresentationParameters[0].BackBufferWidth;
				m_BackbufferSize.cy = m_pStereoRenderer->GetBaseSC()->m_PresentationParameters[0].BackBufferHeight;

				//--- modify only first back buffer ---
				if(m_pStereoRenderer->IsFullscreen())	
				{
					pRecourceToCreate = &modifiedResource;
					modifiedResource  = *pResource;
					for(UINT i=0; i< KM_UM_PRESENTER_BACKBUFFER_COUNT; i++)
						pSurfListNew[i] = pResource->pSurfList[0];
					modifiedResource.pSurfList = pSurfListNew;
					modifiedResource.SurfCount = KM_UM_PRESENTER_BACKBUFFER_COUNT;
				}
			}

			//--- save all other possible backbuffers ---
			if( pResource->Format == m_BackbufferFormat && 
				pResource->pSurfList[0].Width  == m_BackbufferSize.cx && 
				pResource->pSurfList[0].Height == m_BackbufferSize.cy )
				bBackBufferFound = true;
		}
	}

	hr = m_DeviceFunctions.pfnCreateResource( m_hDevice, pRecourceToCreate );
	pResource->hResource = pRecourceToCreate->hResource;

	if( m_pStereoRenderer->m_RendererEvents == reCreatingAQBS )
		m_CommunicationSurface = pRecourceToCreate->hResource;
	else if( m_pStereoRenderer->m_RendererEvents == reCreatingDoubleBuffer )
		m_DoubledBackbuffer = *pRecourceToCreate;

	if ( bBackBufferFound )
	{
		AddBackBuffer(pRecourceToCreate->hResource, pRecourceToCreate->SurfCount);
	}
	DEBUG_MESSAGE(_T("UM Created resource = %p. Width = %i, Height = %i, Format = %x, count = %i. hRresult = %x\n"), pRecourceToCreate->hResource, 
		pRecourceToCreate->pSurfList->Width, pRecourceToCreate->pSurfList->Height, pRecourceToCreate->Format, pRecourceToCreate->SurfCount, hr );

	return hr;
}

HRESULT APIENTRY OpenResourceCallback(HANDLE hDevice, D3DDDIARG_OPENRESOURCE* pResource)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	UMDDeviceWrapper* pDevWrapper = ( UMDDeviceWrapper* )hDevice;
	HRESULT hResult = pDevWrapper->OpenResource( pResource );
#ifndef FINAL_RELEASE
	if(SUCCEEDED(hResult))
		g_ResourceType[pResource->hResource].SharedResource = TRUE;
#endif
	//DEBUG_TRACE3( __FUNCTIONW__ _T(". hDevice = 0x%X hResource=%p\n"), hDevice, 
	//	pResource->hResource);
	return hResult;
}

HRESULT APIENTRY CreateResourceCallback(HANDLE hDevice, D3DDDIARG_CREATERESOURCE* pResource)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	UMDDeviceWrapper* pDevWrapper = ( UMDDeviceWrapper* )hDevice;
	HRESULT hResult = pDevWrapper->CreateResource( pResource );
#ifndef FINAL_RELEASE
	if(SUCCEEDED(hResult))
		g_ResourceType[pResource->hResource] = pResource->Flags;
#endif
	//DEBUG_TRACE3( __FUNCTIONW__ _T(". hDevice = 0x%X hResource=%p SurfCount=%d\n"), hDevice, 
	//	pResource->hResource, pResource->SurfCount);
	return hResult;
}

//////////////////////////////////////////////////////////////////////////

// deleting of UMDDeviceWrapper
HRESULT APIENTRY DestroyDeviceCallback(HANDLE hDevice)                                
{
	DEBUG_TRACEUM(hDevice);
	KillPresenterThread( hDevice );
	HRESULT hResult;
	{
		UMCallGuard cg(hDevice, __FUNCTIONW__);
		hResult = GetDeviceFunctions(hDevice)->pfnDestroyDevice(GetDevice(hDevice));
	}
	if(SUCCEEDED(hResult))
		delete (UMDDeviceWrapper*)hDevice;
	return hResult;
}


//////////////////////////////////////////////////////////////////////////

// Hook
HRESULT APIENTRY PresentCallback(HANDLE hDevice, CONST D3DDDIARG_PRESENT* pPresent)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	UMDDeviceWrapper* pDevWrapper = ( UMDDeviceWrapper* )hDevice;
	HRESULT hResult	= pDevWrapper->Present( pPresent );
	return hResult;
}

// Worker
HRESULT APIENTRY UMDDeviceWrapper::Present( CONST D3DDDIARG_PRESENT* pPresent )
{
	HRESULT hResult;
#ifndef  DISABLE_PRESENTER
	if (!IsBackBuffer(pPresent->hSrcResource))
	{
		DEBUG_MESSAGE(_T("WARNING: Unknown BackBuffer = %p\n"), pPresent->hSrcResource);
	}
	if (USE_UM_PRESENTER)
	{
		hResult = DoPresent( pPresent, NULL );
	}
	else

	if (USE_IZ3DKMSERVICE_PRESENTER)
	{
		hResult = DoKMPresent( pPresent );
	}
	else
#endif
		hResult = m_DeviceFunctions.pfnPresent( m_hDevice, pPresent );

	return hResult;
}

// Hook
HRESULT APIENTRY BltCallback(HANDLE hDevice, CONST D3DDDIARG_BLT* pBlt)
{
	DEBUG_TRACEUM(hDevice);
	UMCallGuard cg(hDevice, __FUNCTIONW__);
	UMDDeviceWrapper* pDevWrapper = ( UMDDeviceWrapper* )hDevice;
	HRESULT hResult	= pDevWrapper->Blt( pBlt );
	return hResult;
}

bool UMDDeviceWrapper::IsBackBuffer( HANDLE  hResource )
{
	for (size_t i = 0; i < m_Backbuffer.size(); i++)
	{
		if ( hResource == m_Backbuffer[i].hResource )
			return true;
	}
	return false;
}

// Worker
HRESULT APIENTRY UMDDeviceWrapper::Blt( CONST D3DDDIARG_BLT* pBlt )
{
	HRESULT hResult;
#ifndef DISABLE_PRESENTER
	if (USE_UM_PRESENTER && IsBackBuffer(pBlt->hSrcResource))
		hResult = DoPresent( NULL, pBlt );	// we can't manual Blt to DWM
	else
	if (USE_IZ3DKMSERVICE_PRESENTER && IsBackBuffer(pBlt->hDstResource))
		hResult = DoKMBlt( pBlt );	
	else
#endif
		hResult = m_DeviceFunctions.pfnBlt( m_hDevice, pBlt );
	return hResult;
}

void UMDDeviceWrapper::CreateUMPresenter( UMDShutterPresenterSCData* pSCData, CBaseSwapChain* pBaseSwapChain )
{
	RECT	rect[2];
	// left rect
	rect[0].left	= 0;
	rect[0].top		= 0;
	rect[0].right	= m_BackbufferSize.cx;
	rect[0].bottom	= m_BackbufferSize.cy;

	// right rect
	rect[1].left	= 0;
	rect[1].top		= m_BackbufferSize.cy;
	rect[1].right	= m_BackbufferSize.cx;
	rect[1].bottom	= m_BackbufferSize.cy * 2;

	m_pPresenter = DNew UMDShutterPresenter( this );
	pSCData->SetData(m_pPresenter,
		rect,
		rect,								// (RECT *)&pBlt->DstRect
		m_DoubledBackbuffer.hResource,		// doubled backbuffer
		m_Backbuffer						// backbuffer
		);
	m_pPresenter->WaitForActionDone();

	m_pPresenter->SetAction(ptInitializeSCData, pBaseSwapChain);
	m_pPresenter->WaitForActionDone();

	m_pStereoRenderer->m_pPresenter = m_pPresenter;
	m_pPresenter->SetCurrentRefreshRate();
	int bbCount = m_pStereoRenderer->GetBaseSC()->m_PresentationParameters[0].BackBufferCount;
	m_pPresenter->m_SuperSynchronizer.Initialize(m_pPresenter->m_RefreshRate, bbCount);
}

HRESULT UMDDeviceWrapper::DoPresent( CONST D3DDDIARG_PRESENT* pPresent, CONST D3DDDIARG_BLT* pBlt )
{
	HRESULT			hResult		= S_OK;

	CBaseSwapChain* pBaseSwapChain = NULL;
	if(m_pStereoRenderer->m_SwapChains.size() > 0)
		pBaseSwapChain = m_pStereoRenderer->GetBaseSC();
	if(!pBaseSwapChain)
	{
		if(pBlt != NULL)
			hResult = m_DeviceFunctions.pfnBlt( m_hDevice, pBlt );
		else
			hResult = m_DeviceFunctions.pfnPresent( m_hDevice, pPresent );
		return hResult;
	}

	UMDShutterPresenterSCData* pSCData = (UMDShutterPresenterSCData*)pBaseSwapChain->m_pBasePresenterData;
	_ASSERT(pSCData);
	_ASSERT(pPresent || pBlt);
	if( !m_pPresenter )
		CreateUMPresenter(pSCData, pBaseSwapChain);

	DWORD BufferIndx	= (pSCData->GetFrameNumber() + 1) % pSCData->GetBackBufferCount(); // N.B. We're starting from second buffer!
	DWORD SubResourceIndex = pBlt ? pBlt->SrcSubResourceIndex : pPresent->SrcSubResourceIndex;
	if (pPresent)
	{
		pSCData->SetPresentData(pPresent);
	}
	pSCData->SetPresentCall(pPresent != NULL);

	if( pBlt != NULL || !pPresent->Flags.Flip
		|| m_pStereoRenderer->SkipPresenterThread()
		)
	{
		pSCData->PH_AfterSleep();
		DWORD			ViewIndx	= 0;  //pSCData->GetFrameNumber() & 0x0001;
		CONST RECT *	pRect		= pSCData->GetStereoResourceParameters();	
		D3DDDIARG_BLT	blt;
		blt.hSrcResource			= m_DoubledBackbuffer.hResource;		
		blt.SrcSubResourceIndex		= 0;
		blt.SrcRect					= pRect[ ViewIndx ];
		blt.ColorKey				= 0;
		if( pBlt != NULL )
		{
			blt.hDstResource			= pBlt->hDstResource;
			blt.DstSubResourceIndex		= pBlt->DstSubResourceIndex;
			blt.DstRect					= pBlt->DstRect;
			blt.Flags.Value				= pBlt->Flags.Value;
		}
		else
		{
			blt.hDstResource			= pPresent->hSrcResource;
			blt.DstSubResourceIndex		= pPresent->SrcSubResourceIndex;
			blt.DstRect					= pRect[0];
			blt.Flags.Value				= 0;
		}

		pSCData->PH_BeforePresent();
		if (gInfo.PresenterSleepTime == -3)
			m_pPresenter->m_SuperSynchronizer.BeforePresent();

		m_CS.Enter();
		NSCALL(	m_DeviceFunctions.pfnBlt( m_hDevice, &blt )	);

		if ( pPresent != NULL ) {
			NSCALL(	m_DeviceFunctions.pfnPresent	( m_hDevice, pPresent )	);
		}
		m_CS.Leave();

		m_pPresenter->m_SuperSynchronizer.AfterPresent();
		pSCData->PH_SetVSyncTime(m_pPresenter->m_SuperSynchronizer.GetVSyncTime());
		if (gInfo.PresenterSleepTime == -2)
			m_pPresenter->m_SuperSynchronizer.AfterPresentOne();
		else if (gInfo.PresenterSleepTime == -3)
			m_pPresenter->m_SuperSynchronizer.AfterPresentTwo();
		pSCData->PH_AfterPresent();
		if (gInfo.PresenterSleepTime != -1 && gInfo.PresenterSleepTime != -2)
			pSCData->CalculateFPS();
	}

	return S_OK;
}

HRESULT	UMDDeviceWrapper::DoKMPresent( CONST D3DDDIARG_PRESENT* pPresent )
{
	HRESULT hResult;
	
	if(m_pStereoRenderer->IsFullscreen())
	{
		//Sleep(10);
		D3DDDIARG_PRESENT KMPresent = *pPresent;
		KMPresent.hSrcResource = m_Backbuffer[m_dwPresentBufferIndex].hResource;
		KMPresent.SrcSubResourceIndex = m_Backbuffer[m_dwPresentBufferIndex].SubResourceIndex;
		hResult = m_DeviceFunctions.pfnPresent( m_hDevice, &KMPresent );

		if(FAILED(hResult))
		{
			DEBUG_MESSAGE(_T(__FUNCTION__) _T(" failed.\n"));
		}
		m_dwPresentBufferIndex++;
		m_dwPresentBufferIndex %= m_Backbuffer.size();
	}
	else
		hResult = m_DeviceFunctions.pfnPresent( m_hDevice, pPresent );
		
	return hResult;
}

HRESULT	UMDDeviceWrapper::DoKMBlt( CONST D3DDDIARG_BLT* pBlt )
{
	HRESULT hResult;
	
	if(m_pStereoRenderer->IsFullscreen())
	{
		D3DDDIARG_BLT KMBlt = *pBlt;
		KMBlt.hDstResource			= m_Backbuffer[m_dwPresentBufferIndex].hResource;
		KMBlt.DstSubResourceIndex	= m_Backbuffer[m_dwPresentBufferIndex].SubResourceIndex;
		hResult = m_DeviceFunctions.pfnBlt( m_hDevice, &KMBlt );

		if(FAILED(hResult))
		{
			DEBUG_MESSAGE(_T(__FUNCTION__) _T(" failed.\n"));
		}
	}
	else
		hResult = m_DeviceFunctions.pfnBlt( m_hDevice, pBlt );

	return hResult;
}

void UMDDeviceWrapper::KillPresenterThread()
{
	if( m_pPresenter )
	{
		m_pStereoRenderer->m_pPresenter = NULL;
		m_pPresenter->SetAction( ptNone );
		SAFE_DELETE(m_pPresenter);
	}
}
