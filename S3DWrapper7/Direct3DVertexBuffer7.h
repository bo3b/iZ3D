/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Direct3D7.h"
#include "Wrapper.h"

class DECLSPEC_UUID("9A3AF767-AC3D-49F3-B0B2-5FEDDFD557D8")
	CDirect3DVertexBuffer7 :
	public CWrapper,
	public IDirect3DVertexBuffer7,
	public IDirect3DVertexBuffer,
	public CComCoClass<CDirect3DVertexBuffer7>
{
private:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;

	BEGIN_COM_MAP(CDirect3DVertexBuffer7)
		COM_INTERFACE_ENTRY(IWrapper)
		COM_INTERFACE_ENTRY(IDirect3DVertexBuffer7)
		COM_INTERFACE_ENTRY(IDirect3DVertexBuffer)
	END_COM_MAP()

public:
	CDirect3DVertexBuffer7();
	STDMETHODIMP Initialize(CDirect3D7* pD3D7, LPD3DVERTEXBUFFERDESC lpVBDesc);

	/*** IDirect3DVertexBuffer7 methods ***/
	STDMETHODIMP Lock(DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize);
	STDMETHODIMP Unlock();
	STDMETHODIMP ProcessVertices(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, 
		LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags);
	STDMETHODIMP ProcessVertices(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, 
		LPDIRECT3DVERTEXBUFFER lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE3 lpD3DDevice, DWORD dwFlags);
	STDMETHODIMP GetVertexBufferDesc(LPD3DVERTEXBUFFERDESC lpVBDesc);
	STDMETHODIMP Optimize(LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags);
	STDMETHODIMP Optimize(LPDIRECT3DDEVICE3 lpD3DDevice, DWORD dwFlags);
	STDMETHODIMP ProcessVerticesStrided(DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, 
		LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, 
		DWORD dwFlags);

public:
	CDirect3D7* m_pD3D7;
	D3DVERTEXBUFFERDESC m_VBDesc;
	CComPtr<IDirect3DVertexBuffer9>	m_pVB;
};

