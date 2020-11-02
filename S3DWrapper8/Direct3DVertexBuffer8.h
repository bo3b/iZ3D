/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DResource8.h"

class CDirect3DVertexBuffer8 : 
	public CDirect3DResource8<IDirect3DVertexBuffer8, IDirect3DVertexBuffer9>,
	public CComCoClass<CDirect3DVertexBuffer8>
{
public:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;
	BEGIN_COM_MAP(CDirect3DVertexBuffer8)
		COM_INTERFACE_ENTRY(IDirect3DVertexBuffer8)
		COM_INTERFACE_ENTRY(IDirect3DResource8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	STDMETHODIMP Lock(UINT OffsetToLock,UINT SizeToLock,BYTE** ppbData,DWORD Flags);
    STDMETHODIMP Unlock();
    STDMETHODIMP GetDesc(D3DVERTEXBUFFER_DESC *pDesc);
};

