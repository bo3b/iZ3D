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
#include "Direct3DCubeTexture8.h"
#include "Direct3DSurface8.h"

STDMETHODIMP CDirect3DCubeTexture8::GetLevelDesc(UINT Level, D3DSURFACE_DESC8 *pDesc)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DCubeTexture8::GetLevelDesc\n");
	D3DSURFACE_DESC pDesc9;
	NSCALL(m_pReal->GetLevelDesc(Level, &pDesc9));
	ConvertSurfDesc(pDesc, pDesc9);
	NSRET;
}

STDMETHODIMP CDirect3DCubeTexture8::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType,UINT Level,IDirect3DSurface8** ppDirect3DSurface8)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DCubeTexture8::GetCubeMapSurface\n");
	CComPtr<IDirect3DSurface9> pDirect3DSurface9Real;
	NSCALL(m_pReal->GetCubeMapSurface(FaceType, Level, &pDirect3DSurface9Real));
	NSPTR(pDirect3DSurface9Real);
	Wrap(pDirect3DSurface9Real.p, ppDirect3DSurface8);
	NSRET;
}

STDMETHODIMP CDirect3DCubeTexture8::LockRect(D3DCUBEMAP_FACES FaceType,UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DCubeTexture8::LockRect\n");
	NSCALL(m_pReal->LockRect(FaceType, Level, pLockedRect, pRect, Flags));
	NSRET;
}

STDMETHODIMP CDirect3DCubeTexture8::UnlockRect(D3DCUBEMAP_FACES FaceType,UINT Level)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DCubeTexture8::UnlockRect\n");
	NSCALL(m_pReal->UnlockRect(FaceType, Level));
	NSRET;
}

STDMETHODIMP CDirect3DCubeTexture8::AddDirtyRect(D3DCUBEMAP_FACES FaceType,CONST RECT* pDirtyRect)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DCubeTexture8::AddDirtyRect\n");
	NSCALL(m_pReal->AddDirtyRect(FaceType, pDirtyRect));
	NSRET;
}
