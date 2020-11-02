#include "stdafx.h"
#include "Output_dx10.h"
#include "../../S3DWrapper10/Presenter.h"

using namespace DX10Output;

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new CMOShutterOutput( nMode_, nSpanMode_ );
}

CMOShutterOutput::CMOShutterOutput( DWORD nMode_, DWORD nSpanMode_ )
	:	OutputMethod( nMode_, nSpanMode_ )
{
	m_hMarkedTextureLeft.pDrvPrivate	= NULL;
	m_hMarkedTextureRight.pDrvPrivate	= NULL;
	m_hMarkedTextureMono.pDrvPrivate	= NULL;
}

CMOShutterOutput::~CMOShutterOutput()
{
}

int CMOShutterOutput::GetPreferredRefreshRate() const
{
	switch(m_OutputMode)
	{
	case 0:
		return 120;
	case 1:
		return 100;
	case 2:
		return 60;
	case 3:
		return 55;
	case 4:
		return 50;
	}
	return 0;
}

void CMOShutterOutput::Output(bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_)
{
	D3DDeviceWrapper* pD3DDevice		= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes			= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice			= pD3DDevice->hDevice;

	D3D10DDI_HRESOURCE hPrimary			= pSwapChain_->GetPresenerBackBuffer( bLeft_ );
	if (hPrimary.pDrvPrivate == NULL)
		return;

	UINT nOffset						= ( bLeft_ ) ?	0 : pSwapChain_->GetLineOffset();
	D3D10DDI_HRESOURCE hOutput			= ( bLeft_ ) ?  pRes->GetMethodResourceLeft() : pRes->GetMethodResourceRight();
	D3D10DDI_HRESOURCE hMarkedSurface	= m_hMarkedTextureMono;
 	if( pRes->IsStereoActive() )
 	{
 		hMarkedSurface	= ( bLeft_ ) ?	m_hMarkedTextureLeft : m_hMarkedTextureRight;
 	}
	GET_ORIG.pfnResourceCopyRegion( hDevice, hOutput, 0, 0, 0, 0, hMarkedSurface, 0, NULL );
	GET_ORIG.pfnResourceCopyRegion( hDevice, hPrimary, 0, 0, nOffset, 0, hOutput, 0, NULL );
}

void CMOShutterOutput::Initialize( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_HDEVICE hDevice	= pD3DWrapper_->GetHandle();

	UINT	nTexWidth	= 2;
	UINT	nTexHeight	= 1;
	UINT	nBitDepth	= 32;
	SIZE_T	nRowBytes	= nTexWidth * nBitDepth / 8;
	BYTE*	pTexData	= DNew BYTE[ nRowBytes * nTexHeight ];
	memset( pTexData, 0xFF, nRowBytes * nTexHeight );

	D3D11DDIARG_CREATERESOURCE CreateTexRes;
	memset( &CreateTexRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

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
	SIZE_T nSize = pD3DWrapper_->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize( hDevice, &CreateTexRes );
	NEW_HANDLE( m_hMarkedTextureMono, nSize );
	pD3DWrapper_->OriginalDeviceFuncs11.pfnCreateResource( hDevice, &CreateTexRes, m_hMarkedTextureMono, hRTTexture );

	pTexData[ 0 ] = 0xFF;
	pTexData[ 1 ] = 0xFF;
	pTexData[ 2 ] = 0xFF;
	pTexData[ 3 ] = 0xFF;
	pTexData[ 4 ] = 0x00;
	pTexData[ 5 ] = 0x00;
	pTexData[ 6 ] = 0x00;
	pTexData[ 7 ] = 0xFF;

	hRTTexture.handle = NULL;
	NEW_HANDLE( m_hMarkedTextureLeft, nSize );
	pD3DWrapper_->OriginalDeviceFuncs11.pfnCreateResource( hDevice, &CreateTexRes, m_hMarkedTextureLeft, hRTTexture );

	pTexData[ 0 ] = 0x00;
	pTexData[ 1 ] = 0x00;
	pTexData[ 2 ] = 0x00;
	pTexData[ 3 ] = 0xFF;
	pTexData[ 4 ] = 0xFF;
	pTexData[ 5 ] = 0xFF;
	pTexData[ 6 ] = 0xFF;
	pTexData[ 7 ] = 0xFF;

	hRTTexture.handle = NULL;
	NEW_HANDLE( m_hMarkedTextureRight, nSize );
	pD3DWrapper_->OriginalDeviceFuncs11.pfnCreateResource( hDevice, &CreateTexRes, m_hMarkedTextureRight, hRTTexture );
	
	delete[] pTexData;
}

void CMOShutterOutput::Clear( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_HDEVICE hDevice	= pD3DWrapper_->GetHandle();

	if ( m_hMarkedTextureMono.pDrvPrivate )
	{
		pD3DWrapper_->OriginalDeviceFuncs.pfnDestroyResource( hDevice, m_hMarkedTextureMono );
		DELETE_HANDLE( m_hMarkedTextureMono );
	}

	if ( m_hMarkedTextureLeft.pDrvPrivate )
	{
		pD3DWrapper_->OriginalDeviceFuncs.pfnDestroyResource( hDevice, m_hMarkedTextureLeft );
		DELETE_HANDLE( m_hMarkedTextureLeft );
	}

	if ( m_hMarkedTextureRight.pDrvPrivate )
	{
		pD3DWrapper_->OriginalDeviceFuncs.pfnDestroyResource( hDevice, m_hMarkedTextureRight );
		DELETE_HANDLE( m_hMarkedTextureRight );
	}
}

bool CMOShutterOutput::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	IDXGIAdapter*		pDXGIAdapter	= NULL;
	IDXGIOutput*		pDXGIOutput		= NULL;
	DXGI_ADAPTER_DESC	AdapterDesc;
	DXGI_OUTPUT_DESC	OutputDesc;
	DISPLAY_DEVICEW		MonitorDevice;

	MonitorDevice.cb	= sizeof( DISPLAY_DEVICEW );

	for ( UINT nAdapter = 0; pDXGIFactory_->EnumAdapters( nAdapter, &pDXGIAdapter ) != DXGI_ERROR_NOT_FOUND; ++nAdapter )
	{
		pDXGIAdapter->GetDesc( &AdapterDesc );
		for ( UINT nOutput = 0; pDXGIAdapter->EnumOutputs( nOutput, &pDXGIOutput ) != DXGI_ERROR_NOT_FOUND; ++nOutput )
		{
			pDXGIOutput->GetDesc( &OutputDesc );
			for( UINT j = 0; EnumDisplayDevices( OutputDesc.DeviceName, j, &MonitorDevice, 0x0 ); j++ )
			{
				if( ( MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) &&
					( wcsstr( MonitorDevice.DeviceID, _T("NXG0D24") )	||
					  wcsstr( MonitorDevice.DeviceID, _T("CMO1590") )	||			// CMO 50Hz Model
					  wcsstr( MonitorDevice.DeviceID, _T("VSC") )	||
					  wcsstr( MonitorDevice.DeviceID, _T("MSIAE12") ) ) )
				{
						return true;
				}
			}
			pDXGIOutput->Release();
		}
		pDXGIAdapter->Release();
	}

#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}
