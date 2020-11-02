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
#include "Direct3DSurface8.h"
#include "Direct3D8.h"
#include "Utils.h"

inline void ConvertSurfDesc( D3DSURFACE_DESC8 * pDesc, D3DSURFACE_DESC &Desc9 )
{
	D3DSURFACE_DESC8 *pDesc8 = pDesc;
	pDesc8->Format = Desc9.Format;
	pDesc8->MultiSampleType = Desc9.MultiSampleType;
	pDesc8->Pool = Desc9.Pool;
	pDesc8->Type = Desc9.Type;
	pDesc8->Usage = Desc9.Usage;
	pDesc8->Width = Desc9.Width;
	pDesc8->Height = Desc9.Height;
	pDesc8->Size = GetSurfaceSize(Desc9);
}

STDMETHODIMP CDirect3DSurface8::GetDesc(D3DSURFACE_DESC8 *pDesc)
{
	D3DSURFACE_DESC pDesc9;
	HRESULT hr = m_pReal->GetDesc(&pDesc9);
	ConvertSurfDesc(pDesc, pDesc9);
	DEBUG_TRACE3("CDirect3DSurface8::GetDesc(pDesc = %s)\n", GetSurfaceDescString(pDesc));
	return hr;
}

STDMETHODIMP CDirect3DSurface8::LockRect(D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	if(pLockedRect == NULL)
		DEBUG_MESSAGE("CDirect3DSurface8::LockRect pLockedRect = NULL\n");
	HRESULT hr = m_pReal->LockRect( pLockedRect, pRect, Flags);
	DEBUG_TRACE3( "CDirect3DSurface8::LockRect(pLockedRect = %s, pRect = %s, Flags = %d) = %s; This=%p Real=%p\n",  
		GetLockedRectString(pLockedRect), GetRectString(pRect), Flags, GetResultString( hr ), this, m_pReal );
	return hr;
}

STDMETHODIMP CDirect3DSurface8::UnlockRect()
{
	DEBUG_TRACE3( "CDirect3DSurface8::UnlockRect(); This=%p Real=%p\n", this, m_pReal);
	HRESULT hr = m_pReal->UnlockRect();
	return hr;
}
