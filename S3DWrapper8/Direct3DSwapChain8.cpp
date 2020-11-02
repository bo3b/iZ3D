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
#include "d3d9_8.h"
#include "Direct3DSwapChain8.h"

STDMETHODIMP CDirect3DSwapChain8::Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	DEBUG_TRACE3("CDirect3DSwapChain8::Present\n");
    return m_pReal->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion,0);
}

STDMETHODIMP CDirect3DSwapChain8::GetBackBuffer( UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer)
{
	HRESULT hr;
	DEBUG_TRACE3("CDirect3DSwapChain8::GetBackBuffer\n");
    CComPtr<IDirect3DSurface9> pBBuff;
	hr = m_pReal->GetBackBuffer( BackBuffer, Type, &pBBuff);
	Wrap(pBBuff.p, ppBackBuffer);
    return hr;
}

