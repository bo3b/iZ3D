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
#include "Direct3DTexture8.h"
#include "Direct3DSurface8.h"

STDMETHODIMP CDirect3DTexture8::GetLevelDesc(UINT Level,D3DSURFACE_DESC8 *pDesc)
{
	NSTRY;
	D3DSURFACE_DESC pDesc9;
	NSCALL(m_pReal->GetLevelDesc(Level, &pDesc9));
	ConvertSurfDesc(pDesc, pDesc9);
	DEBUG_TRACE3("CDirect3DTexture8::GetLevelDesc(Level=%d, pDesc=%s) = %s; This=%p Real=%p\n", 
		Level, GetSurfaceDescString(pDesc), GetResultString(hResult), this, m_pReal);
	NSRET;
}

STDMETHODIMP CDirect3DTexture8::GetSurfaceLevel(UINT Level,IDirect3DSurface8** ppDirect3DSurface8)
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSurface9> pDirect3DSurface9;
	hResult = m_pReal->GetSurfaceLevel(Level, &pDirect3DSurface9);
	Wrap(pDirect3DSurface9.p, ppDirect3DSurface8);
	DEBUG_TRACE3("CDirect3DTexture8::GetSurfaceLevel(Level=%d, ppDirect3DSurface8=%p) = %s; This=%p Real=%p\n", 
		Level, ppDirect3DSurface8 ? *ppDirect3DSurface8 : 0, GetResultString(hResult), this, m_pReal);
	return hResult;
}

STDMETHODIMP CDirect3DTexture8::LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	NSTRY;
	NSCALL(m_pReal->LockRect(Level, pLockedRect, pRect, Flags));
	DEBUG_TRACE3( "CDirect3DTexture8::LockRect(Level = %d, pLockedRect = %s, pRect = %s, Flags = %d) = %s; This=%p Real=%p\n",  
		Level, GetLockedRectString(pLockedRect), GetRectString(pRect), Flags, GetResultString( hResult ), this, m_pReal );
	NSRET;
}

STDMETHODIMP CDirect3DTexture8::UnlockRect(UINT Level)
{
	NSTRY;
	NSCALL(m_pReal->UnlockRect(Level));
	DEBUG_TRACE3("CDirect3DTexture8::UnlockRect(Level = %d) = %s; This=%p Real=%p\n", 
		Level, GetResultString( hResult ), this, m_pReal);
	NSRET;
}

STDMETHODIMP CDirect3DTexture8::AddDirtyRect(CONST RECT* pDirtyRect)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DTexture8::AddDirtyRect\n");
	NSCALL(m_pReal->AddDirtyRect(pDirtyRect));
	NSRET;
}

STDMETHODIMP_(DWORD) CDirect3DTexture8::SetLOD(DWORD LODNew)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DTexture8::SetLOD\n");
	NSCALL(m_pReal->SetLOD(LODNew));
	NSRET;
}
STDMETHODIMP_(DWORD) CDirect3DTexture8::GetLOD()
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DTexture8::GetLOD\n");
	NSCALL(m_pReal->GetLOD());
	NSRET;
}
STDMETHODIMP_(DWORD) CDirect3DTexture8::GetLevelCount()
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DTexture8::GetLevelCount\n");
	NSCALL(m_pReal->GetLevelCount());
	NSRET;
}

