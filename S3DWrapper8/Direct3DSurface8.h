/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"       // main symbols
#include "Direct3DDevice8.h"

class CDirect3DSurface8: 
	public CWrapper<IDirect3DSurface8, IDirect3DSurface9>,
    public CWrapperCoClass<CDirect3DSurface8>
{
public:
	BEGIN_COM_MAP(CDirect3DSurface8)
		COM_INTERFACE_ENTRY(IDirect3DSurface8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	IMPLEMENT_INTERNAL_RELEASE_WEAK();

public:
	// IDirect3DSurface8
	IMPLEMENT_GET_DEVICE(m_pReal);

	STDMETHODIMP SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
	{
		DEBUG_TRACE3("CDirect3DSurface8::SetPrivateData\n");
		return (m_pReal->SetPrivateData(refguid, pData, SizeOfData, Flags));
	}

	STDMETHODIMP GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
	{
		DEBUG_TRACE3("CDirect3DSurface8::GetPrivateData\n");
		return (m_pReal->GetPrivateData(refguid, pData, pSizeOfData));
	}

	STDMETHODIMP FreePrivateData(REFGUID refguid)
	{
		DEBUG_TRACE3("CDirect3DSurface8::FreePrivateData\n");
		return (m_pReal->FreePrivateData(refguid));
	}

	IMPLEMENT_GET_CONTAINER(m_pReal);

    STDMETHODIMP GetDesc(D3DSURFACE_DESC8 *pDesc);
    STDMETHODIMP LockRect(D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
	STDMETHODIMP UnlockRect();
};

void ConvertSurfDesc( D3DSURFACE_DESC8 * pDesc, D3DSURFACE_DESC &pDesc9 );
