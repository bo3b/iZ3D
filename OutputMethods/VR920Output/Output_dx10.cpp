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
#include "Output_dx10.h"
#include "IWear\IWRsdk.h"

using namespace DX10Output;

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new VR920Output( nMode_, nSpanMode_ );
}

enum { LEFT_EYE=0, RIGHT_EYE, MONO_EYES };

VR920Output::VR920Output( DWORD nMode_, DWORD nSpanMode_ )
: OutputMethod( nMode_, nSpanMode_ )
{
	m_bAdapterFound					= false;
	m_StereoHandle					= NULL;
	m_bLastFrameStereoOn			= false;

	m_hLeftEyeQuery.pDrvPrivate		= NULL;
	m_hRightEyeQuery.pDrvPrivate	= NULL;

	IWRLoadDll();	
}

VR920Output::~VR920Output()
{
	if ( m_StereoHandle && IWRSTEREO_Close )
	{
		IWRSTEREO_SetStereo( m_StereoHandle, false );
		IWRSTEREO_Close(m_StereoHandle);
		m_StereoHandle = NULL;
	}
	IWRFreeDll();
}

void VR920Output::StereoModeChanged( bool bNewMode_ )
{
	if ( m_StereoHandle )
	{
		IWRSTEREO_SetStereo( m_StereoHandle, bNewMode_ );
		m_bLastFrameStereoOn = bNewMode_;
	}
}

void VR920Output::Initialize( D3DDeviceWrapper* pD3DWrapper_ )
{
	if ( m_bAdapterFound && IWRSTEREO_Open )
	{
		m_StereoHandle = IWRSTEREO_Open();
	}
	if ( m_StereoHandle )
	{
		IWRSTEREO_SetStereo( m_StereoHandle, false );
	}
	if ( m_OutputMode == 0 )
	{
		D3D10DDI_HDEVICE hDevice		= pD3DWrapper_->hDevice;
		D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DWrapper_->OriginalDeviceFuncs;

		D3D10DDIARG_CREATEQUERY QueryData;
		memset( &QueryData, 0, sizeof( D3D10DDIARG_CREATEQUERY ) );
		QueryData.Query = D3D10DDI_QUERY_EVENT;
		
		D3D10DDI_HRTQUERY hRTQuery			= { (void*)&__uuidof(ID3D10Query) };
		SIZE_T nSize = GET_ORIG.pfnCalcPrivateQuerySize( hDevice, &QueryData );
		NEW_HANDLE( m_hLeftEyeQuery, nSize );
		GET_ORIG.pfnCreateQuery( hDevice, &QueryData, m_hLeftEyeQuery, hRTQuery );
		
		nSize = GET_ORIG.pfnCalcPrivateQuerySize( hDevice, &QueryData );
		NEW_HANDLE( m_hRightEyeQuery, nSize );
		GET_ORIG.pfnCreateQuery( hDevice, &QueryData, m_hRightEyeQuery, hRTQuery );
	}
}

void VR920Output::Clear( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_HDEVICE hDevice		= pD3DWrapper_->hDevice;
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DWrapper_->OriginalDeviceFuncs;

	if ( m_hLeftEyeQuery.pDrvPrivate )
	{
		GET_ORIG.pfnDestroyQuery( hDevice, m_hLeftEyeQuery );
		DELETE_HANDLE( m_hLeftEyeQuery );
	}
	if ( m_hRightEyeQuery.pDrvPrivate )
	{
		GET_ORIG.pfnDestroyQuery( hDevice, m_hRightEyeQuery );
		DELETE_HANDLE( m_hRightEyeQuery );
	}	
}

void VR920Output::Output( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ )
{
	D3DDeviceWrapper* pD3DDevice	= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes		= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice->hDevice;
	
	if ( bLeft_ )
	{
		D3D10DDI_HRESOURCE hOutputLeft = pRes->GetMethodResourceLeft();
		GET_ORIG.pfnResourceCopy(hDevice, pRes->GetBackBufferPrimary(), hOutputLeft );
	}
	else
	{
		D3D10DDI_HRESOURCE hOutputRight = pRes->GetMethodResourceRight();
		GET_ORIG.pfnResourceCopy( hDevice, pRes->GetBackBufferPrimary(), hOutputRight );
	}
	if ( m_bLastFrameStereoOn && m_StereoHandle )
		IWRSTEREO_WaitForAck( m_StereoHandle, bLeft_ ? LEFT_EYE : RIGHT_EYE );
}

void VR920Output::AfterPresent( D3DDeviceWrapper* pD3DDevice_, bool bLeft_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice_->hDevice;

	if ( !m_bLastFrameStereoOn )
		return;	
	
	D3D10DDI_HQUERY hEyeQuery = bLeft_ ? m_hLeftEyeQuery : m_hRightEyeQuery;

	if ( hEyeQuery.pDrvPrivate )
	{
		GET_ORIG.pfnQueryEnd( hDevice, hEyeQuery );
		GET_ORIG.pfnQueryGetData( hDevice, hEyeQuery, NULL, 0, D3D10_DDI_GET_DATA_DO_NOT_FLUSH );
	}
	if ( m_StereoHandle )
		IWRSTEREO_SetLR( m_StereoHandle, bLeft_ ? LEFT_EYE : RIGHT_EYE );
}

bool VR920Output::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	// Find the DirectX method of adapter ordinal, that is the VR920's
	IDXGIAdapter*		pDXGIAdapter	= NULL;
	IDXGIOutput*		pDXGIOutput		= NULL;
	DXGI_OUTPUT_DESC	OutputDesc;

	DISPLAY_DEVICEW		MonitorDevice;
	MonitorDevice.cb	= sizeof( DISPLAY_DEVICEW );

	for ( UINT nAdapter = 0; pDXGIFactory_->EnumAdapters( nAdapter, &pDXGIAdapter ) != DXGI_ERROR_NOT_FOUND; ++nAdapter )
	{
		if ( pDXGIAdapter->EnumOutputs( 0, &pDXGIOutput ) != DXGI_ERROR_NOT_FOUND )
		{
			pDXGIOutput->GetDesc( &OutputDesc );
			pDXGIOutput->Release();

			for( UINT j = 0; EnumDisplayDevices( OutputDesc.DeviceName, j, &MonitorDevice, 0x0 ); j++ )
			{
				if( ( MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) &&
					wcsstr( MonitorDevice.DeviceID, _T("IWR0002") ) )
				{
						// Found the VR920 PnP id.
						m_bAdapterFound = true;
						pDXGIAdapter->Release();
						return true;
				}
			}
		}
		pDXGIAdapter->Release();
	}
#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}