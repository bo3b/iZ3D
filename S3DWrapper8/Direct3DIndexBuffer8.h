/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DResource8.h"
#include "Direct3DDevice8.h"
#include "Direct3D8.h"

class CDirect3DIndexBuffer8 : 
	public CDirect3DResource8<IDirect3DIndexBuffer8, IDirect3DIndexBuffer9>,
	public CComCoClass<CDirect3DIndexBuffer8>
{

public:
	using CDirect3DResource8<IDirect3DIndexBuffer8, IDirect3DIndexBuffer9>::Lock;
	using CDirect3DResource8<IDirect3DIndexBuffer8, IDirect3DIndexBuffer9>::Unlock;
	BEGIN_COM_MAP(CDirect3DIndexBuffer8)
		COM_INTERFACE_ENTRY(IDirect3DIndexBuffer8)
		COM_INTERFACE_ENTRY(IDirect3DResource8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

// IDirect3DIndexBuffer8
public:
	STDMETHODIMP Lock(UINT OffsetToLock,UINT SizeToLock,BYTE** ppbData,DWORD Flags);
    STDMETHODIMP Unlock();
    STDMETHODIMP GetDesc(D3DINDEXBUFFER_DESC *pDesc);
};

