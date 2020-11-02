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
#include "Direct3DVertexBuffer7.h"

CDirect3DVertexBuffer7::CDirect3DVertexBuffer7()
: m_pD3D7(NULL)
{
}

STDMETHODIMP CDirect3DVertexBuffer7::Initialize(CDirect3D7* pD3D7, LPD3DVERTEXBUFFERDESC lpVBDesc)
{
	m_pD3D7 = pD3D7;
	m_VBDesc = *lpVBDesc;
	D3DPOOL Pool = lpVBDesc->dwCaps & D3DVBCAPS_SYSTEMMEMORY  ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
	DWORD Usage = lpVBDesc->dwCaps & D3DVBCAPS_WRITEONLY ? D3DUSAGE_WRITEONLY : 0;
	
	HRESULT hResult = m_pD3D7->m_pD3D9Device->CreateVertexBuffer(lpVBDesc->dwNumVertices * GetVertexSize(lpVBDesc->dwFVF), 
		Usage, lpVBDesc->dwFVF, Pool, &m_pVB, NULL);
	return hResult;
}

STDMETHODIMP CDirect3DVertexBuffer7::Lock( DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	DWORD flags = 0;
	if (lpdwSize)
	{
		 D3DVERTEXBUFFER_DESC desc;
		 m_pVB->GetDesc(&desc);
		 *lpdwSize = desc.Size;
	}
	if (dwFlags & DDLOCK_READONLY)
		flags |= D3DLOCK_READONLY;
	if (!(dwFlags & DDLOCK_WAIT))
		flags |= D3DLOCK_DONOTWAIT;
	if (dwFlags & DDLOCK_NOOVERWRITE)
		flags |= D3DLOCK_NOOVERWRITE;
	if (dwFlags & DDLOCK_DISCARDCONTENTS)
		flags |= D3DLOCK_DISCARD;
	if (dwFlags & DDLOCK_NOSYSLOCK)
		flags |= D3DLOCK_NOSYSLOCK;
	HRESULT hResult = m_pVB->Lock(0, 0, lplpData, flags);
	return hResult;
}

STDMETHODIMP CDirect3DVertexBuffer7::Unlock()
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pVB->Unlock();
	return hResult;
}

STDMETHODIMP CDirect3DVertexBuffer7::ProcessVertices( DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DVertexBuffer7::ProcessVertices( DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPDIRECT3DVERTEXBUFFER lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE3 lpD3DDevice, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DVertexBuffer7::GetVertexBufferDesc(LPD3DVERTEXBUFFERDESC lpVBDesc)
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lpVBDesc = m_VBDesc;
	return S_OK;
}

STDMETHODIMP CDirect3DVertexBuffer7::Optimize(LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags)
{
	// TODO: ID3DXMesh::Optimize
	DEBUG_TRACE3("\t%s\n", __FUNCTION__);
	return S_OK;
}

STDMETHODIMP CDirect3DVertexBuffer7::Optimize( LPDIRECT3DDEVICE3 lpD3DDevice, DWORD dwFlags )
{
	DEBUG_TRACE3("\t%s\n", __FUNCTION__);
	return S_OK;
}

STDMETHODIMP CDirect3DVertexBuffer7::ProcessVerticesStrided(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, 
					LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, 
					DWORD dwFlags)
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}