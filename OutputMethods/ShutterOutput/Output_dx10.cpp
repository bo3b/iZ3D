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

using namespace DX10Output;

#define NSCALL(func)						hResult = func;

OUTPUT_API void* CALLBACK CreateOutputDX10(DWORD mode, DWORD spanMode)
{
	return new ShutterOutput(mode, spanMode);
}

ShutterOutput::ShutterOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

ShutterOutput::~ShutterOutput(void)
{
}

void ShutterOutput::Output(bool bLeft_, D3DSwapChain* pSwapChain_, RECT* pDestRect_)
{
	HRESULT hResult = S_OK;
	D3DDeviceWrapper* pD3DDevice	= pSwapChain_->m_pD3DDeviceWrapper;
	SwapChainResources* pRes		= pSwapChain_->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice->hDevice;;

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
	//D3DCOLOR lineColor;
	//if (m_OutputMode == 1 || m_OutputMode == 2) {
	//	if (m_OutputMode == 1)
	//		lineColor = D3DCOLOR_XRGB(0x00, 0x00, 0xFF); // blue
	//	else
	//		lineColor = D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF); // white
	//	SIZE bbSize = pSwapChain->m_BackBufferSize;
	//	RECT rect;
	//	rect.top	= bbSize.cy - 1;
	//	rect.bottom	= bbSize.cy;
	//	rect.left	= 0;
	//	rect.right	= bbSize.cx / 4 * (bLeft ? 1 : 3);
	//	if (pDestRect)
	//	{
	//		rect.left	+= pDestRect->left;
	//		rect.top	+= pDestRect->top;
	//		rect.right	+= pDestRect->left;
	//		rect.bottom	+= pDestRect->top;
	//	}
	//	NSCALL(m_pd3dDevice->ColorFill(primary, &rect, lineColor));
	//	rect.left	= bbSize.cx / 4 * (bLeft ? 1 : 3);
	//	rect.right	= bbSize.cx;
	//	if (pDestRect)
	//	{
	//		rect.left	+= pDestRect->left;
	//		rect.right	+= pDestRect->left;
	//	}
	//	NSCALL(m_pd3dDevice->ColorFill(primary, &rect, D3DCOLOR_XRGB(0x00, 0x00, 0x00)));
	//}
}
