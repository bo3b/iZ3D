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

#define  EMA_STATIC_LINK 
#include "EMADevice.h"

using namespace DX10Output;

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new Z800Output( nMode_, nSpanMode_ );
}

enum { LEFT_EYE=0, RIGHT_EYE, MONO_EYES };

Z800Output::Z800Output( DWORD nMode_, DWORD nSpanMode_ )
: OutputMethod( nMode_, nSpanMode_ )
{
	m_pZ800					= NULL;
	m_dwFrameCount			= 0;
	m_bAdapterFound			= false;
	m_bLastFrameStereoOn	= true;
}

Z800Output::~Z800Output()
{
	if ( m_pZ800 )
	{
		HRESULT hr = m_pZ800->GoSleep();
		delete m_pZ800; 
	}
}

void Z800Output::StereoModeChanged( bool bNewMode_ )
{
	HRESULT hr = m_pZ800->EnableStereoVision ( bNewMode_ );
	m_bLastFrameStereoOn = bNewMode_;
}

void Z800Output::Initialize( D3DDeviceWrapper* pD3DWrapper_ )
{
	if (m_pZ800)
	{
		HRESULT hr = m_pZ800->EnableStereoVision ( false ) ;
	}
}

void Z800Output::Clear( D3DDeviceWrapper* pD3DWrapper_ )
{
}

void Z800Output::Output( bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_ )
{
	D3DDeviceWrapper* pD3DDevice	= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes		= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice->hDevice;

	if (m_pZ800)
	{
		++m_dwFrameCount ; 
		if ( 0 == ( m_dwFrameCount % 20000 ) ) 
			m_pZ800->KeepAlive ( ) ;
	}
	
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
}

bool Z800Output::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	m_pZ800 = new EMADevice ( EMA_NO_HEADTRACKING ) ;
	if ( m_pZ800 ) 
	{ 
		// Turn on the headset if it was sleeping 
		HRESULT hr = m_pZ800->KeepAlive ( ) ; 

		m_bAdapterFound = true;
		return true;
	}

#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}