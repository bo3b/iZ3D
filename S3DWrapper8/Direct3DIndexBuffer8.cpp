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
#include "Direct3DIndexBuffer8.h"

STDMETHODIMP CDirect3DIndexBuffer8::Lock(UINT OffsetToLock, UINT SizeToLock,
										 BYTE** ppbData, DWORD Flags)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DIndexBuffer8::Lock\n");
	NSCALL(m_pReal->Lock(OffsetToLock, SizeToLock, (void **)ppbData, Flags));
	NSRET;
}

STDMETHODIMP CDirect3DIndexBuffer8::Unlock()
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DIndexBuffer8::Unlock\n");
	NSCALL(m_pReal->Unlock());
	NSRET;
}

STDMETHODIMP CDirect3DIndexBuffer8::GetDesc(D3DINDEXBUFFER_DESC *pDesc)
{
	NSTRY;
	DEBUG_TRACE3("CDirect3DIndexBuffer8::GetDesc\n");
	NSCALL(m_pReal->GetDesc(pDesc));
	NSRET;
}

