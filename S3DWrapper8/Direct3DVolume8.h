/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDirect3DVolume8

class CDirect3DVolume8 : 
	public CWrapper<IDirect3DVolume8, IDirect3DVolume9>,
    public CComCoClass<CDirect3DVolume8>
{
public:
	BEGIN_COM_MAP(CDirect3DVolume8)
		COM_INTERFACE_ENTRY(IDirect3DVolume8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	// IDirect3DVolume8
	IMPLEMENT_GET_DEVICE(m_pReal);
	IMPLEMENT_INTERNAL_RELEASE_WEAK();

	STDMETHODIMP SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
	{
		DEBUG_TRACE3("CDirect3DVolume8::SetPrivateData\n");
		return (m_pReal->SetPrivateData(refguid, pData, SizeOfData, Flags));
	}

	STDMETHODIMP GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
	{
		DEBUG_TRACE3("CDirect3DVolume8::GetPrivateData\n");
		return (m_pReal->GetPrivateData(refguid, pData, pSizeOfData));
	}

	STDMETHODIMP FreePrivateData(REFGUID refguid)
	{
		DEBUG_TRACE3("CDirect3DVolume8::FreePrivateData\n");
		return (m_pReal->FreePrivateData(refguid));
	}

	IMPLEMENT_GET_CONTAINER(m_pReal);

    STDMETHODIMP GetDesc( D3DVOLUME_DESC8 *pDesc);
    STDMETHODIMP LockBox( D3DLOCKED_BOX * pLockedVolume,CONST D3DBOX* pBox,DWORD Flags);
    STDMETHODIMP UnlockBox();
};

void ConvertVolumeDesc( D3DVOLUME_DESC8 * pDesc, D3DVOLUME_DESC &pDesc9 );
