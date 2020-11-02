/* IZ3D_FILE: $Id: DualProjectionOutput.cpp 1540 2008-12-25 14:41:36Z kostik $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: kostik $
* $Revision: 1540 $
* $Date: 2008-12-25 17:41:36 +0300 (Чт, 25 дек 2008) $
* $LastChangedBy: kostik $
* $URL: svn://svn.neurok.ru/dev/driver/trunk/S3DWrapper9/OutputMethods/DualProjectionOutput/DualProjectionOutput.cpp $
*/
// DualProjectionOutput.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx10.h"

using namespace DX10Output;

#define NSCALL(func)						hResult = func;

OUTPUT_API void* CALLBACK CreateOutputDX10(DWORD mode, DWORD spanMode)
{
	return new DualProjectionOutput(mode, spanMode);
}

DualProjectionOutput::DualProjectionOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

DualProjectionOutput::~DualProjectionOutput(void)
{
}

void DualProjectionOutput::Output( D3DSwapChain* pSwapChain )
{
	D3DDeviceWrapper* pD3DDevice	= pSwapChain->m_pD3DDeviceWrapper;
	SwapChainResources* pRes		= pSwapChain->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice->hDevice;

	if ( !pRes->IsAAOn() )
	{
		if ( pRes->GetBackBufferLeft() != pRes->GetBackBufferPrimary() )
		{
			GET_ORIG.pfnResourceCopy( hDevice, pRes->GetBackBufferPrimary(), pRes->GetBackBufferLeft() );
		}

		if ( (pRes->GetBackBufferRight() != pRes->GetBackBufferSecondary()) && (pRes->GetBackBufferSecondary().pDrvPrivate != NULL) )
		{	
			GET_ORIG.pfnResourceCopy( hDevice, pRes->GetBackBufferSecondary(), pRes->GetBackBufferRight() );
		}
	}
	else
	{
		if ( pRes->GetBackBufferLeft() != pRes->GetBackBufferPrimary() )
		{
			GET_ORIG.pfnResourceResolveSubresource( hDevice, pRes->GetBackBufferPrimary(), 0,
				pRes->GetBackBufferLeft(), 0,  pSwapChain->m_Description.BufferDesc.Format);
		}

		if ( (pRes->GetBackBufferRight() != pRes->GetBackBufferSecondary()) && (pRes->GetBackBufferSecondary().pDrvPrivate != NULL) )
		{	
			GET_ORIG.pfnResourceResolveSubresource( hDevice, pRes->GetBackBufferSecondary(), 0,
				pRes->GetBackBufferRight(), 0,  pSwapChain->m_Description.BufferDesc.Format);
		}
	}
}
