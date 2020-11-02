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
// Output.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx10.h"

#include <HighLevelMonitorConfigurationAPI.h>
#include "MonitorsPNPID.h"
#include "../../CommonUtils/StringUtils.h"

using namespace DX10Output;

#define NSCALL(func)						hResult = func;

OUTPUT_API void* CALLBACK CreateOutputDX10(DWORD mode, DWORD spanMode)
{
	return new S3D120HzProjectorsOutput(mode, spanMode);
}

S3D120HzProjectorsOutput::S3D120HzProjectorsOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

S3D120HzProjectorsOutput::~S3D120HzProjectorsOutput(void)
{
}

void S3D120HzProjectorsOutput::Output(bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_)
{
	D3DDeviceWrapper* pD3DDevice		= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes			= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice			= pD3DDevice->hDevice;

	if( pRes->IsStereoActive() )
	{
		UINT nOffset					= ( bLeft_ ) ?	0 : pSwapChain_->GetLineOffset();
		D3D10DDI_HRESOURCE hOutput		= ( bLeft_ ) ?  pRes->GetMethodResourceLeft() : pRes->GetMethodResourceRight();
		D3D10DDI_HRESOURCE hPrimary		= pSwapChain_->GetPresenerBackBuffer( bLeft_ );
		GET_ORIG.pfnResourceCopyRegion( hDevice, hPrimary, 0, 0, nOffset, 0, hOutput, 0, NULL );
	}
	else
	{
		UINT nOffset					= 0;
		D3D10DDI_HRESOURCE hOutputLeft	= pRes->GetMethodResourceLeft();
		D3D10DDI_HRESOURCE hDstLeft		= pSwapChain_->GetPresenerBackBuffer( true );
		GET_ORIG.pfnResourceCopyRegion( hDevice, hDstLeft, 0, 0, nOffset, 0, hOutputLeft, 0, NULL );
		nOffset							= pSwapChain_->GetLineOffset();
		D3D10DDI_HRESOURCE hDstRight	= pSwapChain_->GetPresenerBackBuffer( false );
		GET_ORIG.pfnResourceCopyRegion( hDevice, hDstRight, 0, 0, nOffset, 0, hOutputLeft, 0, NULL );
	}
}

bool S3D120HzProjectorsOutput::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	OutputMethod::FindAdapter(pDXGIFactory_);
	return true;
}
