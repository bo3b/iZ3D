#include "StdAfx.h"
#include "MonoOutput_dx10.h"
#include "../../S3DWrapper10/D3DSwapChain.h"

using namespace DX10Output;

MonoOutput::MonoOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

MonoOutput::~MonoOutput(void)
{
}

void MonoOutput::Output( D3DSwapChain* pSwapChain )
{
	D3DDeviceWrapper* pD3DDevice = pSwapChain->m_pD3DDeviceWrapper;
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DDevice->hDevice;
	SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
	//if ( pSwapChain->m_MultiSample.Count == 1 )
	{
		if ( pRes->GetBackBufferLeft() != pRes->GetBackBufferPrimary() )
		{
			GET_ORIG.pfnResourceCopy( hDevice, pRes->GetBackBufferPrimary(), pRes->GetBackBufferLeft() );
		}
	}
	//else
	//{
	//	if ( pRes->GetBackBufferLeft() != pRes->GetBackBufferPrimary() )
	//	{
	//		GET_ORIG.pfnResourceResolveSubresource( hDevice, pRes->GetBackBufferPrimary(), 0,
	//			pRes->GetBackBufferLeft(), 0,  pSwapChain->m_Description.BufferDesc.Format);
	//	}
	//}
}
