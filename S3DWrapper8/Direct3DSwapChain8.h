/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"       // main symbols
#include "Direct3DDevice8.h"

/////////////////////////////////////////////////////////////////////////////
// CDirect3DSwapChain8

class CDirect3DSwapChain8 : 
	public CWrapper<IDirect3DSwapChain8, IDirect3DSwapChain9>,
    public CComCoClass<CDirect3DSwapChain8>
{
public:
	BEGIN_COM_MAP(CDirect3DSwapChain8)
		COM_INTERFACE_ENTRY(IDirect3DSwapChain8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()
	IMPLEMENT_INTERNAL_RELEASE_STRONG();

    // IDirect3DSwapChain8
    STDMETHODIMP Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
    STDMETHODIMP GetBackBuffer( UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer);
};

