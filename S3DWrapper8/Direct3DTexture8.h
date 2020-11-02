/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DBaseTexture8.h"
#include "Direct3DDevice8.h"
#include "Direct3D8.h"

class CDirect3DTexture8 : 
	public CDirect3DBaseTexture8<IDirect3DTexture8, IDirect3DTexture9>,
	public CComCoClass<CDirect3DTexture8>
{
public:
	BEGIN_COM_MAP(CDirect3DTexture8)
		COM_INTERFACE_ENTRY(IDirect3DTexture8)
		COM_INTERFACE_ENTRY(IDirect3DBaseTexture8)
		COM_INTERFACE_ENTRY(IDirect3DResource8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

	STDMETHODIMP_(DWORD) SetLOD(DWORD LODNew);
    STDMETHODIMP_(DWORD) GetLOD();
    STDMETHODIMP_(DWORD) GetLevelCount();

	STDMETHODIMP GetLevelDesc(UINT Level,D3DSURFACE_DESC8 *pDesc);
    STDMETHODIMP GetSurfaceLevel(UINT Level,IDirect3DSurface8** ppSurfaceLevel);
    STDMETHODIMP LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    STDMETHODIMP UnlockRect(UINT Level);
    STDMETHODIMP AddDirtyRect(CONST RECT* pDirtyRect);
};


