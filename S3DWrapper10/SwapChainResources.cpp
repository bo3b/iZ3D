#include "StdAfx.h"
#include "D3DSwapChain.h"
#include "SwapChainResources.h"
#include "D3DDeviceWrapper.h"
#include "../OutputMethods/OutputLib/OutputMethod_dx10.h"

SwapChainResources::SwapChainResources()
{
	m_pDevice = NULL;
	m_bAAOn = false;
	m_pBackBufferPrimaryWrap = NULL;
	m_hBackBufferPrimary.pDrvPrivate = NULL;
	m_hBackBufferRTPrimary.pDrvPrivate = NULL;
	m_hBackBufferSecondary.pDrvPrivate = NULL;
	m_hBackBufferRTSecondary.pDrvPrivate = NULL;
	m_hScaledMethodResource.pDrvPrivate = NULL;
	m_hMethodResourceLeft.pDrvPrivate = NULL;
	m_hMethodResourceRight.pDrvPrivate = NULL;
	m_hMethodTextureLeft.pDrvPrivate = NULL;
	m_hMethodTextureRight.pDrvPrivate = NULL;
	m_pBackBufferPrimaryBeforeScalingWrap = NULL;
	m_hBackBufferLeftBeforeScaling.pDrvPrivate = NULL;
	m_hBackBufferRightBeforeScaling.pDrvPrivate = NULL;
	m_hBackBufferLeft.pDrvPrivate = NULL;
	m_hBackBufferRight.pDrvPrivate = NULL;
	m_hBackBufferRTLeft.pDrvPrivate = NULL;
	m_hBackBufferRTRight.pDrvPrivate = NULL;
	m_hPresenterResourceWrap.pDrvPrivate = NULL;
	m_hPresenterResourceLeft.pDrvPrivate = NULL;
	m_hPresenterResourceRight.pDrvPrivate = NULL;
	m_hPresenterResourceWrap2.pDrvPrivate = NULL;
	m_hPresenterResourceLeft2.pDrvPrivate = NULL;
	m_hPresenterResourceRight2.pDrvPrivate = NULL;
	for( unsigned i=0; i<KM_SHUTTER_SERVICE_BUFFER_COUNT; ++i )
	{
		m_hKMShutterMarkerResource [ i ].pDrvPrivate = NULL;
		m_hKMShutterPrimaryResource[ i ].pDrvPrivate = NULL;
	}
	m_uKMShutterMarkerResourceIndex  = 0;
	m_uKMShutterPrimaryResourceIndex = 0;
}

void SwapChainResources::Initialize(D3DSwapChain* pSwapChain)
{
	ResourceWrapper* pWrpApp = m_pBackBufferPrimaryWrap;

	ResourceWrapper* pWrpBeforeScaling = m_pBackBufferPrimaryBeforeScalingWrap;

	m_bAAOn = (pWrpApp->m_CreateResource.SampleDesc.Count > 1);

	m_hBackBufferLeftBeforeScaling = pWrpBeforeScaling->GetHandle();
	m_hBackBufferRightBeforeScaling = pWrpBeforeScaling->m_hRightHandle;
	_ASSERT(m_hBackBufferRightBeforeScaling.pDrvPrivate);

	// create backbuffer replacement
	m_pDevice->CreateResource(m_hBackBufferPrimary, pWrpApp);

	// create scaled left/right view
	if (pWrpApp != pWrpBeforeScaling)
	{
		//D3D11DDIARG_CREATERESOURCE createResource = pWrpApp->m_CreateResource11;
		//createResource.BindFlags			= D3D10_DDI_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET;
		//createResource.SampleDesc.Count		= 1;
		//createResource.SampleDesc.Quality	= 0;
		//createResource.pPrimaryDesc			= NULL;

		//m_pDevice->CreateResource(m_hBackBufferLeft, &createResource, pWrpApp->m_hRTResource);
		//m_pDevice->CreateResource(m_hBackBufferRight, &createResource, pWrpApp->m_hRTResource);
		_ASSERT((pWrpApp->m_CreateResource11.BindFlags & (D3D10_DDI_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET)) == (D3D10_DDI_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET));
		_ASSERT(pWrpApp->m_CreateResource11.SampleDesc.Count == 1);
		_ASSERT(pWrpApp->m_CreateResource11.pPrimaryDesc == NULL);
		m_hBackBufferLeft = pWrpApp->GetHandle();
		m_hBackBufferRight = pWrpApp->m_hRightHandle;
		
		if (gInfo.DisplayScalingMode == DISPLAY_SCALING_SCALEDASPECTEX)
		{
			static char RTData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
			D3D10DDI_HRTRESOURCE hRTResource = { RTData };

			// create helper method resource for blitting
			D3D10DDI_MIPINFO mipInfo;
			mipInfo.PhysicalDepth  = 1;
			mipInfo.PhysicalWidth  = pSwapChain->m_SrcRect.right  - pSwapChain->m_SrcRect.left;
			mipInfo.PhysicalHeight = pSwapChain->m_SrcRect.bottom - pSwapChain->m_SrcRect.top;
			mipInfo.TexelDepth     = 1;
			mipInfo.TexelWidth     = pSwapChain->m_SrcRect.right  - pSwapChain->m_SrcRect.left;
			mipInfo.TexelHeight    = pSwapChain->m_SrcRect.bottom - pSwapChain->m_SrcRect.top;

			D3D11DDIARG_CREATERESOURCE createResource = pWrpApp->m_CreateResource11;
			createResource.BindFlags			= D3D10_DDI_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET;
			createResource.SampleDesc.Count		= 1;
			createResource.SampleDesc.Quality	= 0;
			createResource.pPrimaryDesc			= NULL;
			createResource.pMipInfoList = &mipInfo;
			createResource.MipLevels    = 1;
			m_pDevice->CreateResource(m_hScaledMethodResource, &createResource, pWrpApp->m_hRTResource);
		}
	}
	else
	{
		m_hBackBufferLeft  = m_hBackBufferLeftBeforeScaling;
		m_hBackBufferRight = m_hBackBufferRightBeforeScaling;
	}

	if ( m_pDevice->m_bTwoWindows )
	{
		static char RTData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
		D3D10DDI_HRTRESOURCE hRTResource = { RTData };

		D3D11DDIARG_CREATERESOURCE createResource = pWrpApp->m_CreateResource11;
		createResource.BindFlags = D3D10_DDI_BIND_RENDER_TARGET;
		createResource.MiscFlags = 0;
		if (m_pDevice->m_pOutputMethod && m_pDevice->m_pOutputMethod->RenderDirectlyToBB())
		{
			createResource.SampleDesc.Count = 1;
			createResource.SampleDesc.Quality = 0;
		}
		createResource.pPrimaryDesc = NULL;
		m_pDevice->CreateResource(m_hBackBufferSecondary, &createResource, pWrpApp->m_hRTResource);
	}

	/************************************************************************/
	// CreateRenderTargetView for left and right D3D10DDI_HRESOURCE

	D3D10DDIARG_CREATERENDERTARGETVIEW createRenderTargetView;
	memset( &createRenderTargetView, 0, sizeof( D3D10DDIARG_CREATERENDERTARGETVIEW ));

	createRenderTargetView.Format = pWrpApp->m_CreateResource.Format;
	createRenderTargetView.ResourceDimension = D3D10DDIRESOURCE_TEXTURE2D;
	createRenderTargetView.Tex2D.ArraySize = 1;
	createRenderTargetView.Tex2D.FirstArraySlice = 0;
	createRenderTargetView.Tex2D.MipSlice = 0;

	D3D10DDI_HRTRENDERTARGETVIEW  hRTRenderTargetView = { NULL };
	m_pDevice->CreateRenderTargetView(m_hBackBufferRTLeft, m_hBackBufferLeft, &createRenderTargetView, hRTRenderTargetView);
	m_pDevice->CreateRenderTargetView(m_hBackBufferRTRight, m_hBackBufferRight, &createRenderTargetView, hRTRenderTargetView);

	/************************************************************************/	
	// CreateResource
	// if handle gamma should be SRGB format*
	if (m_bAAOn)
	{
		if (!m_pDevice->m_pOutputMethod->RenderDirectlyToBB())
		{
			static char RTData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
			D3D10DDI_HRTRESOURCE hRTResource = { RTData };

			D3D11DDIARG_CREATERESOURCE createResource;
			createResource = pWrpApp->m_CreateResource11;
			createResource.BindFlags = D3D10_DDI_BIND_SHADER_RESOURCE | D3D10_DDI_BIND_RENDER_TARGET;
			createResource.SampleDesc.Count = 1;
			createResource.SampleDesc.Quality = 0;
			createResource.pPrimaryDesc = NULL;

			m_pDevice->CreateResource(m_hMethodResourceLeft, &createResource, hRTResource);
			m_pDevice->CreateResource(m_hMethodResourceRight, &createResource, hRTResource);
		}
	}
	else
	{
		m_hMethodResourceLeft = m_hBackBufferLeft;
		m_hMethodResourceRight = m_hBackBufferRight;
	}

	if (!m_pDevice->m_pOutputMethod->RenderDirectlyToBB())
	{
		// CreateShaderResourceView
		D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView = { NULL };
		D3D11DDIARG_CREATESHADERRESOURCEVIEW  createShaderResourceView;
		memset( &createShaderResourceView, 0, sizeof( D3D11DDIARG_CREATESHADERRESOURCEVIEW ) );
		createShaderResourceView.Format					= pWrpApp->m_CreateResource.Format;
		createShaderResourceView.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
		createShaderResourceView.Tex2D.ArraySize		= 1;
		createShaderResourceView.Tex2D.FirstArraySlice	= 0;
		createShaderResourceView.Tex2D.MipLevels		= 1;
		createShaderResourceView.Tex2D.MostDetailedMip	= 0;

		m_pDevice->CreateShaderResourceView(m_hMethodTextureLeft, m_hMethodResourceLeft, &createShaderResourceView, hRTShaderResourceView);
		m_pDevice->CreateShaderResourceView(m_hMethodTextureRight, m_hMethodResourceRight, &createShaderResourceView, hRTShaderResourceView);

		// CreateRenderTargetView for SwapChain BB's
		D3D10DDIARG_CREATERENDERTARGETVIEW  createRenderTargetView;
		createRenderTargetView.Format = pWrpApp->m_CreateResource.Format;
		createRenderTargetView.ResourceDimension = D3D10DDIRESOURCE_TEXTURE2D;
		createRenderTargetView.Tex2D.ArraySize = 1;
		createRenderTargetView.Tex2D.FirstArraySlice = 0;
		createRenderTargetView.Tex2D.MipSlice = 0;

		m_pDevice->CreateRenderTargetView(m_hBackBufferRTPrimary, m_hBackBufferPrimary, &createRenderTargetView, hRTRenderTargetView);

		if ( m_pDevice->m_bTwoWindows )
		{
			m_pDevice->CreateRenderTargetView(m_hBackBufferRTSecondary, m_hBackBufferSecondary, &createRenderTargetView, hRTRenderTargetView);

			D3D10DDI_HDEVICE hDevice		= m_pDevice->hDevice;
			D3D10DDI_DEVICEFUNCS& GET_ORIG	= m_pDevice->OriginalDeviceFuncs;
			GET_ORIG.pfnClearRenderTargetView(hDevice, m_hBackBufferRTSecondary, m_pDevice->m_pOutputMethod->InitSecondBBColor());
		}
	}
}

void SwapChainResources::Clear()
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = m_pDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = m_pDevice->hDevice;

	m_pBackBufferPrimaryWrap = NULL;

	if (m_hBackBufferRTLeft.pDrvPrivate)
	{
		GET_ORIG.pfnDestroyRenderTargetView(hDevice, m_hBackBufferRTLeft );
		DELETE_HANDLE(m_hBackBufferRTLeft);

		GET_ORIG.pfnDestroyRenderTargetView(hDevice, m_hBackBufferRTRight );
		DELETE_HANDLE(m_hBackBufferRTRight);
	}

	if (!m_pDevice->m_pOutputMethod->RenderDirectlyToBB())
	{
		GET_ORIG.pfnDestroyShaderResourceView(hDevice, m_hMethodTextureLeft);
		DELETE_HANDLE(m_hMethodTextureLeft);

		GET_ORIG.pfnDestroyShaderResourceView(hDevice, m_hMethodTextureRight);
		DELETE_HANDLE(m_hMethodTextureRight);

		GET_ORIG.pfnDestroyRenderTargetView(hDevice, m_hBackBufferRTPrimary);
		DELETE_HANDLE(m_hBackBufferRTPrimary);

		if ( m_pDevice->m_bTwoWindows )
		{
			GET_ORIG.pfnDestroyRenderTargetView(hDevice, m_hBackBufferRTSecondary);
			DELETE_HANDLE(m_hBackBufferRTSecondary);
		}
	}

	if (m_hBackBufferPrimary.pDrvPrivate)
	{
		GET_ORIG.pfnDestroyResource(hDevice, m_hBackBufferPrimary);
		DELETE_HANDLE(m_hBackBufferPrimary);
	}
	if (m_hBackBufferSecondary.pDrvPrivate)
	{
		GET_ORIG.pfnDestroyResource(hDevice, m_hBackBufferSecondary);
		DELETE_HANDLE(m_hBackBufferSecondary);
	}

	if (m_bAAOn && !m_pDevice->m_pOutputMethod->RenderDirectlyToBB())
	{
		GET_ORIG.pfnDestroyResource(hDevice, m_hMethodResourceLeft);
		DELETE_HANDLE(m_hMethodResourceLeft);

		GET_ORIG.pfnDestroyResource(hDevice, m_hMethodResourceRight);
		DELETE_HANDLE(m_hMethodResourceRight);
	}
	else
	{
		m_hMethodResourceLeft.pDrvPrivate = NULL;
		m_hMethodResourceRight.pDrvPrivate = NULL;
	}
		
	//if (m_hBackBufferRight != m_hBackBufferRightBeforeScaling)
	//{
	//	GET_ORIG.pfnDestroyResource(hDevice, m_hBackBufferRight);
	//	DELETE_HANDLE(m_hBackBufferRight);
	//}

	//if (m_hBackBufferLeft != m_hBackBufferLeftBeforeScaling)
	//{
	//	GET_ORIG.pfnDestroyResource(hDevice, m_hBackBufferLeft);
	//	DELETE_HANDLE(m_hBackBufferLeft);
	//}
	m_hBackBufferLeft.pDrvPrivate = NULL;
	m_hBackBufferRight.pDrvPrivate = NULL;

	if (m_hScaledMethodResource.pDrvPrivate)
	{
		GET_ORIG.pfnDestroyResource(hDevice, m_hScaledMethodResource);
		DELETE_HANDLE(m_hScaledMethodResource);
	}

	m_pBackBufferPrimaryBeforeScalingWrap = NULL;
	m_hBackBufferLeftBeforeScaling.pDrvPrivate = NULL;
	m_hBackBufferRightBeforeScaling.pDrvPrivate = NULL;
}

void SwapChainResources::InitializePresenterResources()
{
	m_hPresenterResourceLeft			= m_hBackBufferPrimary;
	m_hPresenterResourceRight			= m_hBackBufferPrimary;

	if( !(USE_UM_PRESENTER || USE_IZ3DKMSERVICE_PRESENTER) )
	{
		return;						// do nothing
	}

	if( m_hPresenterResourceWrap.pDrvPrivate )
		ClearPresenterResources();	// kill old resources if they exist

	DEBUG_TRACE1(_T("InitializePresenterResources()\n") );
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= m_pDevice->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& GET_ORIG11	= m_pDevice->OriginalDeviceFuncs11;
	D3D10DDI_HDEVICE hDevice			= m_pDevice->hDevice;

	ResourceWrapper* pWrp				= m_pBackBufferPrimaryWrap;
	D3D10DDI_HRTRESOURCE  hRTResource	= pWrp->m_hRTResource;
	SIZE_T size;

	D3D11DDIARG_CREATERESOURCE CreateResource;
	CreateResource = pWrp->m_CreateResource11;
	CreateResource.SampleDesc.Count = 1;
	CreateResource.SampleDesc.Quality = 0;

	if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
		size = GET_ORIG.pfnCalcPrivateResourceSize(hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateResource);
	else
		size = GET_ORIG11.pfnCalcPrivateResourceSize(hDevice, &CreateResource);
	size += ADDITIONAL_RESOURCE_SIZE;
	NEW_HANDLE(m_hPresenterResourceWrap, size);
	ResourceWrapper* pPresenterWrp	= (ResourceWrapper*)m_hPresenterResourceWrap.pDrvPrivate;
	if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
	{
		const D3D10DDIARG_CREATERESOURCE* pCreateResource = (D3D10DDIARG_CREATERESOURCE*)&CreateResource;
		new(pPresenterWrp) ResourceWrapper(m_pDevice, pCreateResource, hRTResource);
		GET_ORIG.pfnCreateResource(hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateResource, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap), hRTResource);
	}
	else
	{
		const D3D11DDIARG_CREATERESOURCE* pCreateResource = &CreateResource;
		new(pPresenterWrp) ResourceWrapper(m_pDevice, pCreateResource, hRTResource);
		GET_ORIG11.pfnCreateResource(hDevice, &CreateResource, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap), hRTResource);
	}
	pPresenterWrp->Init(m_pDevice);

	m_hPresenterResourceLeft	= pPresenterWrp->GetHandle();
	m_hPresenterResourceRight	= pPresenterWrp->m_hRightHandle;

	if( USE_IZ3DKMSERVICE_PRESENTER )
	{
		if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
			size = GET_ORIG.pfnCalcPrivateResourceSize(hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateResource);
		else
			size = GET_ORIG11.pfnCalcPrivateResourceSize(hDevice, &CreateResource);
		size += ADDITIONAL_RESOURCE_SIZE;
		NEW_HANDLE(m_hPresenterResourceWrap2, size);
		ResourceWrapper* pPresenterWrp	= (ResourceWrapper*)m_hPresenterResourceWrap2.pDrvPrivate;
		if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
		{
			const D3D10DDIARG_CREATERESOURCE* pCreateResource = (D3D10DDIARG_CREATERESOURCE*)&CreateResource;
			new(pPresenterWrp) ResourceWrapper(m_pDevice, pCreateResource, hRTResource);
			GET_ORIG.pfnCreateResource(hDevice, (D3D10DDIARG_CREATERESOURCE*)&CreateResource, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap2), hRTResource);
		}
		else
		{
			const D3D11DDIARG_CREATERESOURCE* pCreateResource = &CreateResource;
			new(pPresenterWrp) ResourceWrapper(m_pDevice, pCreateResource, hRTResource);
			GET_ORIG11.pfnCreateResource(hDevice, &CreateResource, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap2), hRTResource);
		}
		pPresenterWrp->Init(m_pDevice);

		m_hPresenterResourceLeft2	= pPresenterWrp->GetHandle();
		m_hPresenterResourceRight2	= pPresenterWrp->m_hRightHandle;

#ifdef FINAL_RELEASE
		const UINT	nTexWidth		= 1;
		const UINT	nTexHeight		= 1;
#else
		const UINT	nTexWidth		= 16;		// debug marker width
		const UINT	nTexHeight		= 16;		// debug marker height
#endif
		const UINT	nBitDepth		= 32;		// DXGI_FORMAT_R8G8B8A8_UNORM
		SIZE_T	nRowBytes			= nTexWidth * nBitDepth / 8;
		BYTE*	pTexData			= DNew BYTE[ nRowBytes * nTexHeight ];

		for( unsigned i=0; i<KM_SHUTTER_SERVICE_BUFFER_COUNT; ++i )
		{
			D3D11DDIARG_CREATERESOURCE CreateTexRes = { 0 };

			D3D10DDI_MIPINFO TexMipInfo;
			TexMipInfo.TexelWidth				= nTexWidth;
			TexMipInfo.TexelHeight				= nTexHeight;
			TexMipInfo.TexelDepth				= 1;
			TexMipInfo.PhysicalWidth			= nTexWidth;
			TexMipInfo.PhysicalHeight			= nTexHeight;
			TexMipInfo.PhysicalDepth			= 1;

			D3D10_DDIARG_SUBRESOURCE_UP InitialTexTes;
			InitialTexTes.pSysMem				= pTexData;
			InitialTexTes.SysMemPitch			= (UINT)nRowBytes;
			InitialTexTes.SysMemSlicePitch		= (UINT)nRowBytes * nTexHeight;

			CreateTexRes.pMipInfoList			= &TexMipInfo;
			CreateTexRes.pInitialDataUP			= &InitialTexTes;
			CreateTexRes.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
			CreateTexRes.Usage					= D3D10_DDI_USAGE_DEFAULT;
			CreateTexRes.BindFlags				= D3D10_DDI_BIND_SHADER_RESOURCE;
			CreateTexRes.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
			CreateTexRes.SampleDesc.Count		= 1;
			CreateTexRes.SampleDesc.Quality		= 0;
			CreateTexRes.MipLevels				= 1;
			CreateTexRes.ArraySize				= 1;

			D3D10DDI_HRTRESOURCE hRTTexture		= { NULL };
#ifdef FINAL_RELEASE
			BYTE	markercolor = i;
#else
			BYTE	markercolor = 64 * i + 63;
#endif
			memset( pTexData, markercolor, nRowBytes * nTexHeight );

			m_pDevice->CreateResource(m_hKMShutterMarkerResource[i], &CreateTexRes, hRTTexture);
		}
		delete[] pTexData;
		m_uKMShutterMarkerResourceIndex = 0;

		//--- create additional primaries for fullscreen mode presentation
		if( true /*!m_pDevice->GetD3DSwapChain()->m_Description.Windowed*/ )
		{
			for( unsigned i=0; i<KM_SHUTTER_SERVICE_BUFFER_COUNT; ++i )
				m_pDevice->CreateResource(m_hKMShutterPrimaryResource[i], pWrp);
		}
	}

	//CPresenterX::Get()->SetAction(ptInitializeSCData);
	//CPresenterX::Get()->WaitForActionDone();
}

void SwapChainResources::ClearPresenterResources()
{
	DEBUG_TRACE1(_T("ClearPresenterResources()\n") );
	
	// delete "shared" resources
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= m_pDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice			= m_pDevice->hDevice;

	if( m_hPresenterResourceWrap.pDrvPrivate )
	{
		ResourceWrapper* pPresenterWrapper = (ResourceWrapper*)m_hPresenterResourceWrap.pDrvPrivate;
		pPresenterWrapper->Destroy(m_pDevice); 
		pPresenterWrapper->~ResourceWrapper();
		GET_ORIG.pfnDestroyResource( hDevice, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap) );
		DELETE_HANDLE( m_hPresenterResourceWrap );
		m_hPresenterResourceLeft.pDrvPrivate = NULL;
		m_hPresenterResourceRight.pDrvPrivate = NULL;
	}

	if( m_hPresenterResourceWrap2.pDrvPrivate )
	{
		ResourceWrapper* pPresenterWrapper = (ResourceWrapper*)m_hPresenterResourceWrap2.pDrvPrivate;
		pPresenterWrapper->Destroy(m_pDevice); 
		pPresenterWrapper->~ResourceWrapper();
		GET_ORIG.pfnDestroyResource( hDevice, GET_RESOURCE_HANDLE(m_hPresenterResourceWrap2) );
		DELETE_HANDLE( m_hPresenterResourceWrap2 );
		m_hPresenterResourceLeft2.pDrvPrivate = NULL;
		m_hPresenterResourceRight2.pDrvPrivate = NULL;
	}

	for( unsigned i=0; i<KM_SHUTTER_SERVICE_BUFFER_COUNT; ++i )
	{
		if( m_hKMShutterMarkerResource[i].pDrvPrivate )
		{
			GET_ORIG.pfnDestroyResource( hDevice, m_hKMShutterMarkerResource[i] );
			DELETE_HANDLE( m_hKMShutterMarkerResource[i] );
		}
		if( m_hKMShutterPrimaryResource[i].pDrvPrivate )
		{
			GET_ORIG.pfnDestroyResource( hDevice, m_hKMShutterPrimaryResource[i] );
			DELETE_HANDLE( m_hKMShutterPrimaryResource[i] );
		}
	}
	m_uKMShutterMarkerResourceIndex = 0;

	//CPresenterX::Get()->SetAction(ptClearSCData);
	//CPresenterX::Get()->WaitForActionDone();
}

void	SwapChainResources::SetStereoRenderTarget( int view )
{
	_ASSERT( view == VIEWINDEX_LEFT || (view == VIEWINDEX_RIGHT && IsStereoActive()) );
	D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView = { NULL };	
	UINT nClearRTs = Commands::GetClearCount( 1, m_pDevice->m_DeviceState.m_NumRenderTargets );
	D3D10DDI_HRENDERTARGETVIEW hRT;
	hRT = (view != VIEWINDEX_RIGHT) ? m_hBackBufferRTLeft : m_hBackBufferRTRight;
	if ( m_pDevice->GetD3DVersion() != TD3DVersion_11_0 )
		m_pDevice->OriginalDeviceFuncs.pfnSetRenderTargets( m_pDevice->hDevice, &hRT, 1, nClearRTs, hDepthStencilView );
	else
		m_pDevice->OriginalDeviceFuncs11.pfnSetRenderTargets( m_pDevice->hDevice, &hRT, 1, nClearRTs, hDepthStencilView,
		NULL, NULL, 1, 0, 1, 0 );
}
