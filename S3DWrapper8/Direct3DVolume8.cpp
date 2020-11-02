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
// Direct3DVolume8.cpp : Implementation of CDx8wrpApp and DLL registration.

#include "stdafx.h"
#include "d3d9_8.h"
#include "Direct3DVolume8.h"
#include "Direct3DDevice8.h"
#include "Utils.h"

inline void ConvertVolumeDesc( D3DVOLUME_DESC8 * pDesc, D3DVOLUME_DESC &Desc9 )
{
	D3DVOLUME_DESC8 *pDesc8 = pDesc;
	pDesc8->Format = Desc9.Format;
	pDesc8->Height = Desc9.Height;
	pDesc8->Width = Desc9.Width;
	pDesc8->Depth = Desc9.Depth;
	pDesc8->Pool = Desc9.Pool;
	pDesc8->Type = Desc9.Type;
	pDesc8->Usage = Desc9.Usage;
	pDesc8->Width = Desc9.Width;
	pDesc8->Height = Desc9.Height;
	pDesc8->Size = GetVolumeSize(Desc9);
}

STDMETHODIMP CDirect3DVolume8::GetDesc( D3DVOLUME_DESC8 *pDesc)
{
	D3DVOLUME_DESC pDesc9;
	DEBUG_TRACE3("CDirect3DVolume8::GetDesc\n");
	HRESULT hr = m_pReal->GetDesc(&pDesc9);
	ConvertVolumeDesc(pDesc, pDesc9);
	return hr;
}

STDMETHODIMP CDirect3DVolume8::LockBox( D3DLOCKED_BOX * pLockedVolume,CONST D3DBOX* pBox,DWORD Flags)
{
	DEBUG_TRACE3("CDirect3DVolume8::LockBox\n");
    return m_pReal->LockBox( pLockedVolume, pBox, Flags);
}

STDMETHODIMP CDirect3DVolume8::UnlockBox()
{
	DEBUG_TRACE3("CDirect3DVolume8::UnlockBox\n");
    return m_pReal->UnlockBox();
}


