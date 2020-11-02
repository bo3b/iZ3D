/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DBaseTexture8.h"

class CDirect3DVolumeTexture8 : 
	public CDirect3DBaseTexture8<IDirect3DVolumeTexture8, IDirect3DVolumeTexture9>,
	public CComCoClass<CDirect3DVolumeTexture8>
{
public:
	BEGIN_COM_MAP(CDirect3DVolumeTexture8)
		COM_INTERFACE_ENTRY(IDirect3DVolumeTexture8)
		COM_INTERFACE_ENTRY(IDirect3DBaseTexture8)
		COM_INTERFACE_ENTRY(IDirect3DResource8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

public:
	STDMETHODIMP GetLevelDesc(UINT Level,D3DVOLUME_DESC8 *pDesc);
    STDMETHODIMP GetVolumeLevel(UINT Level,IDirect3DVolume8** ppVolumeLevel);
    STDMETHODIMP LockBox(UINT Level,D3DLOCKED_BOX* pLockedVolume,CONST D3DBOX* pBox,DWORD Flags);
    STDMETHODIMP UnlockBox(UINT Level);
    STDMETHODIMP AddDirtyBox(CONST D3DBOX* pDirtyBox);
};

