#include "stdafx.h"
#include "Output_dx10.h"
#include "..\..\S3DWrapper10\Presenter.h"

using namespace DX10Output;

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new AMDHD3DOutput( nMode_, nSpanMode_ );
}

AMDHD3DOutput::AMDHD3DOutput( DWORD nMode_, DWORD nSpanMode_ )
	:	OutputMethod( nMode_, nSpanMode_ )
{
}

AMDHD3DOutput::~AMDHD3DOutput()
{
}

int AMDHD3DOutput::GetPreferredRefreshRate() const
{
	return 0;
}

void AMDHD3DOutput::Output(bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_)
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
	GET_ORIG.pfnResourceCopyRegion( hDevice, hPrimary, 0, 0, nOffset, 0, hOutput, 0, NULL );
}
