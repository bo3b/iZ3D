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
#include "Direct3DVolumeTexture8.h"
#include "Direct3DVolume8.h"

STDMETHODIMP CDirect3DVolumeTexture8::GetLevelDesc(UINT Level, D3DVOLUME_DESC8 *pDesc)
{
	NSTRY;
	D3DVOLUME_DESC pDesc9;
	DEBUG_TRACE3("CDirect3DVolumeTexture8::GetLevelDesc\n");
	NSCALL(m_pReal->GetLevelDesc(Level, &pDesc9));
	ConvertVolumeDesc(pDesc, pDesc9);
	NSRET;
}

STDMETHODIMP CDirect3DVolumeTexture8::GetVolumeLevel(UINT Level, IDirect3DVolume8** ppDirect3DVolume8)
{
	NSTRY;
	CComPtr<IDirect3DVolume9> pDirect3DVolume9;
	DEBUG_TRACE3("CDirect3DVolumeTexture8::GetVolumeLevel\n");
	NSCALL(m_pReal->GetVolumeLevel(Level, &pDirect3DVolume9));
	Wrap(pDirect3DVolume9.p, ppDirect3DVolume8);
	NSRET;
}

STDMETHODIMP CDirect3DVolumeTexture8::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume,
											  CONST D3DBOX* pBox, DWORD Flags)
{
	DEBUG_TRACE3("CDirect3DVolumeTexture8::LockBox\n");
	return m_pReal->LockBox(Level, pLockedVolume, pBox, Flags);
}

STDMETHODIMP CDirect3DVolumeTexture8::UnlockBox(UINT Level)
{
	DEBUG_TRACE3("CDirect3DVolumeTexture8::UnlockBox\n");
	return m_pReal->UnlockBox(Level);
}

STDMETHODIMP CDirect3DVolumeTexture8::AddDirtyBox(CONST D3DBOX* pDirtyBox)
{
	DEBUG_TRACE3("CDirect3DVolumeTexture8::AddDirtyBox\n");
	return m_pReal->AddDirtyBox(pDirtyBox);
}
