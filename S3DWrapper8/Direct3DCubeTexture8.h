/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"       
#include "Direct3DBaseTexture8.h"
#include "Direct3DDevice8.h"
#include "Direct3D8.h"

class CDirect3DCubeTexture8: 
	public CDirect3DBaseTexture8<IDirect3DCubeTexture8, IDirect3DCubeTexture9>,
	public CComCoClass<CDirect3DCubeTexture8>
{
public:
	BEGIN_COM_MAP(CDirect3DCubeTexture8)
		COM_INTERFACE_ENTRY(IDirect3DCubeTexture8)
		COM_INTERFACE_ENTRY(IDirect3DBaseTexture8)
		COM_INTERFACE_ENTRY(IDirect3DResource8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()
public:
	STDMETHODIMP GetLevelDesc(UINT Level,D3DSURFACE_DESC8 *pDesc);
    STDMETHODIMP GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface8** ppCubeMapSurface);
    STDMETHODIMP LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    STDMETHODIMP UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level);
    STDMETHODIMP AddDirtyRect(D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect);
private:
};

