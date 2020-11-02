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
#include "Direct3DDevice7.h"
#include "DirectDrawSurface7.h"
#include "Direct3DVertexBuffer7.h"
#include <atltypes.h>
#include <algorithm>

CDirect3DDevice7::CDirect3DDevice7():
m_pDirect3D7(NULL), m_pReal(NULL), m_IndexCount(0),
m_Fvf(0), m_VB(NULL), m_VertexSize(0), m_pRenderTarget(NULL)
{
	for (int i = 0; i < 8; i++)
	{
		m_pTextures[i] = NULL;
	}
}

CDirect3DDevice7::~CDirect3DDevice7()
{
	DEBUG_TRACE1("CDirect3DDevice7::~CDirect3DDevice7()  Device=%p\n", this);
	DEBUG_TRACE1("Delete %d SB\n", m_SBHandle.size());
	for(size_t i = 0; i < m_SBHandle.size(); i++)
		NSFINALRELEASE((IDirect3DStateBlock9*)m_SBHandle[i]);
}

STDMETHODIMP CDirect3DDevice7::Initialize( CDirect3D7* pDirect3D, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS )
{
	HRESULT hResult = S_OK;
	m_pDirect3D7 = pDirect3D;
	Init(pDirect3D);
	CDirectDrawSurface7* pSurf;
	hResult = lpDDS->QueryInterface(IID_IWrapper, (void**)&pSurf);
	m_pRenderTarget = pSurf;
	if (!pSurf->m_AttachedSurfaces.empty())
	{
		m_pDirect3D7->m_PresentationParameters.EnableAutoDepthStencil = TRUE;
		m_pDirect3D7->m_PresentationParameters.AutoDepthStencilFormat = D3DFMT_D16;
	}	
	hResult = m_pDirect3D7->m_pD3D7->CreateDevice(rclsid, pSurf->m_pSurface7, &m_pD3DDevice7);
	m_pDirect3D7->InitializeD3D9Device();
	m_pDirect3D7->AddWrapper(this);
	m_pReal = m_pDirect3D7->m_pD3D9Device;
	hResult = m_pReal->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf->m_pSurface);
	if (!pSurf->m_AttachedSurfaces.empty())
	{
		IWrapper* pWrapper = (IWrapper*)pSurf->m_AttachedSurfaces[0];
		CDirectDrawSurface7* pDepthSurf = (CDirectDrawSurface7*)pWrapper;
		hResult = m_pReal->GetDepthStencilSurface(&pDepthSurf->m_pSurface);
	}
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::Initialize( LPDIRECT3D lpd3d, LPGUID lpGUID, LPD3DDEVICEDESC lpd3ddvdesc )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetCaps( LPD3DDEVICEDESC7 lpD3DDevDesc )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	D3DCAPS9 caps;
	HRESULT hResult = m_pDirect3D7->m_pD3D9Device->GetDeviceCaps(&caps);
	ZeroMemory(lpD3DDevDesc, sizeof(D3DDEVICEDESC7));
	lpD3DDevDesc->dwDevCaps = caps.Caps;
	lpD3DDevDesc->dpcTriCaps.dwSize = sizeof(D3DPRIMCAPS);
	lpD3DDevDesc->dpcTriCaps.dwMiscCaps = caps.PrimitiveMiscCaps;
	lpD3DDevDesc->dpcTriCaps.dwRasterCaps = caps.RasterCaps;
	lpD3DDevDesc->dpcTriCaps.dwZCmpCaps = caps.ZCmpCaps;
	lpD3DDevDesc->dpcTriCaps.dwSrcBlendCaps = caps.SrcBlendCaps;
	lpD3DDevDesc->dpcTriCaps.dwDestBlendCaps = caps.DestBlendCaps;
	lpD3DDevDesc->dpcTriCaps.dwAlphaCmpCaps = caps.AlphaCmpCaps;
	lpD3DDevDesc->dpcTriCaps.dwShadeCaps = caps.ShadeCaps;
	lpD3DDevDesc->dpcTriCaps.dwTextureCaps = caps.TextureCaps;
	lpD3DDevDesc->dpcTriCaps.dwTextureFilterCaps = caps.TextureFilterCaps;
	lpD3DDevDesc->dpcTriCaps.dwTextureBlendCaps = 0;
	lpD3DDevDesc->dpcTriCaps.dwTextureAddressCaps = caps.TextureAddressCaps;
	lpD3DDevDesc->dpcTriCaps.dwStippleWidth = 0;
	lpD3DDevDesc->dpcTriCaps.dwStippleHeight = 0;
	lpD3DDevDesc->dpcLineCaps = lpD3DDevDesc->dpcTriCaps;
	lpD3DDevDesc->dwMinTextureWidth = 1;
	lpD3DDevDesc->dwMinTextureHeight = 1;
	lpD3DDevDesc->dwMaxTextureWidth = caps.MaxTextureWidth;
	lpD3DDevDesc->dwMaxTextureHeight = caps.MaxTextureHeight;


	lpD3DDevDesc->dwMaxTextureRepeat = caps.MaxTextureRepeat;
	lpD3DDevDesc->dwMaxTextureAspectRatio = caps.MaxTextureAspectRatio;
	lpD3DDevDesc->dwMaxAnisotropy = caps.MaxAnisotropy;

	lpD3DDevDesc->dvGuardBandLeft = caps.GuardBandLeft;
	lpD3DDevDesc->dvGuardBandTop = caps.GuardBandTop;
	lpD3DDevDesc->dvGuardBandRight = caps.GuardBandRight;
	lpD3DDevDesc->dvGuardBandBottom = caps.GuardBandBottom;

	lpD3DDevDesc->dvExtentsAdjust = caps.ExtentsAdjust;
	lpD3DDevDesc->dwStencilCaps = caps.StencilCaps;

	lpD3DDevDesc->dwFVFCaps = caps.FVFCaps;
	lpD3DDevDesc->dwTextureOpCaps = caps.TextureOpCaps;
	lpD3DDevDesc->wMaxTextureBlendStages = (WORD)caps.MaxTextureBlendStages;
	lpD3DDevDesc->wMaxSimultaneousTextures = (WORD)caps.MaxSimultaneousTextures;

	lpD3DDevDesc->dwMaxActiveLights = caps.MaxActiveLights;
	lpD3DDevDesc->dvMaxVertexW = caps.MaxVertexW;

	lpD3DDevDesc->wMaxUserClipPlanes = (WORD)caps.MaxUserClipPlanes;
	lpD3DDevDesc->wMaxVertexBlendMatrices = (WORD)caps.MaxVertexBlendMatrices;

	lpD3DDevDesc->dwVertexProcessingCaps = caps.VertexProcessingCaps;
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetCaps( LPD3DDEVICEDESC lpD3DHWDevDesc, LPD3DDEVICEDESC lpD3DHELDevDesc )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::EnumTextureFormats( LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, LPVOID lpArg )
{
	HRESULT hResult = S_OK;
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (m_pD3DDevice7)
		hResult = m_pD3DDevice7->EnumTextureFormats(lpd3dEnumPixelProc, lpArg);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::EnumTextureFormats( LPD3DENUMTEXTUREFORMATSCALLBACK lpd3dEnumTextureProc, LPVOID lpArg )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::BeginScene()
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->BeginScene();
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::EndScene()
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->EndScene();
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetDirect3D( LPDIRECT3D7 *lplpD3D )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	*lplpD3D = m_pDirect3D7;
	return S_OK;
}

STDMETHODIMP CDirect3DDevice7::GetDirect3D( LPDIRECT3D3 *lplpD3D )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetDirect3D( LPDIRECT3D2 *lplpD3D )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetDirect3D( LPDIRECT3D *lplpD3D )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetRenderTarget( LPDIRECTDRAWSURFACE7 lpNewRenderTarget, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	SAFE_RELEASE(m_pRenderTarget);
	IDirect3DSurface9* pSurf9 = NULL;
	if (lpNewRenderTarget)
	{
		CDirectDrawSurface7* pSurf = NULL;
		lpNewRenderTarget->QueryInterface(IID_IWrapper, (void**)&pSurf);
		m_pRenderTarget = pSurf;
		if (pSurf)
			pSurf9 = pSurf->m_pSurface;
	}
	else
		m_pRenderTarget = NULL;
	HRESULT hResult = m_pReal->SetRenderTarget(0, pSurf9);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetRenderTarget( LPDIRECTDRAWSURFACE4 lpNewRenderTarget, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetRenderTarget( LPDIRECTDRAWSURFACE lpNewRenderTarget,DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetRenderTarget( LPDIRECTDRAWSURFACE7 *lplpRenderTarget )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (m_pRenderTarget)
		m_pRenderTarget->QueryInterface(IID_IDirectDrawSurface7, (void**)lplpRenderTarget);
	else
		*lplpRenderTarget = NULL;
	return S_OK;
}

STDMETHODIMP CDirect3DDevice7::GetRenderTarget( LPDIRECTDRAWSURFACE4 *lplpRenderTarget )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetRenderTarget( LPDIRECTDRAWSURFACE *lplpRenderTarget )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Clear( DWORD dwCount, LPD3DRECT lpRects, DWORD dwFlags, DWORD dwColor, D3DVALUE dvZ, DWORD dwStencil )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->Clear(dwCount, lpRects, dwFlags, dwColor, dvZ, dwStencil);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetTransform( D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->SetTransform(dtstTransformStateType, lpD3DMatrix);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetTransform( D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetTransform(dtstTransformStateType, lpD3DMatrix);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetViewport( LPD3DVIEWPORT7 lpViewport )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->SetViewport((D3DVIEWPORT9*)lpViewport);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::MultiplyTransform( D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->MultiplyTransform(dtstTransformStateType, lpD3DMatrix);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetViewport( LPD3DVIEWPORT7 lpViewport )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetViewport((D3DVIEWPORT9*)lpViewport);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetMaterial( LPD3DMATERIAL7 lpMaterial )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->SetMaterial((D3DMATERIAL9*)lpMaterial);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetMaterial( LPD3DMATERIAL7 lpMaterial )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetMaterial((D3DMATERIAL9*)lpMaterial);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetLight( DWORD dwLightIndex, LPD3DLIGHT7 lpLight )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->SetLight(dwLightIndex, (D3DLIGHT9*)lpLight);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetLight( DWORD dwLightIndex, LPD3DLIGHT7 lpLight )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetLight(dwLightIndex, (D3DLIGHT9*)lpLight);
	return hResult;
}

typedef enum _D3DRENDERSTATETYPE7 {    
	D3DRS_LINEPATTERN               =  10,    
	D3DRS_ZVISIBLE                  =  30,    
	D3DRS_EDGEANTIALIAS             =  40,    
	D3DRS_ZBIAS                     =  47,    
	D3DRS_SOFTWAREVERTEXPROCESSING  = 153,    
	D3DRS_PATCHSEGMENTS             = 164,
	D3DRS8_FORCE_DWORD               = 0x7fffffff
} D3DRENDERSTATETYPE7;

STDMETHODIMP CDirect3DDevice7::SetRenderState( D3DRENDERSTATETYPE dwRenderStateType, DWORD dwRenderState )
{
	DEBUG_TRACE3( "CDirect3DDevice7::SetRenderState(D3DRENDERSTATETYPE State=%d, DWORD Value=%p);\n", dwRenderStateType,dwRenderState);
	switch(dwRenderStateType)
	{
	case D3DRS_LINEPATTERN:
	case D3DRS_ZVISIBLE:
		return S_OK;
	case D3DRS_EDGEANTIALIAS: 
		if (dwRenderState == 0)
			return S_OK;
		else
			return D3DERR_INVALIDCALL;
	case D3DRS_ZBIAS:
		{
			float convertetBias = -0.1e-4f * dwRenderState;
			return m_pReal->SetRenderState(D3DRS_DEPTHBIAS,(DWORD&)convertetBias);
		}
	case D3DRS_SOFTWAREVERTEXPROCESSING:
		return m_pReal->SetSoftwareVertexProcessing((BOOL) dwRenderState);
	case D3DRS_PATCHSEGMENTS:
		if (*((float*)&dwRenderState) <= 1) 
			return m_pReal->SetNPatchMode( 0);
		else
			return m_pReal->SetNPatchMode( *((float*)&dwRenderState));
	default:
		return m_pReal->SetRenderState(dwRenderStateType,dwRenderState);
	}
}

typedef struct _myD3DLINEPATTERN {
	WORD    wRepeatFactor;
	WORD    wLinePattern;
} myD3DLINEPATTERN;

STDMETHODIMP CDirect3DDevice7::GetRenderState( D3DRENDERSTATETYPE dwRenderStateType, LPDWORD lpdwRenderState )
{
	HRESULT hr = S_OK;
	float fdata;
	switch(dwRenderStateType)
	{
	case D3DRS_LINEPATTERN:
		memset(lpdwRenderState, 0, sizeof(myD3DLINEPATTERN));
		break;
	case D3DRS_ZVISIBLE:
		*lpdwRenderState = FALSE;
		break;
	case D3DRS_EDGEANTIALIAS:
		*lpdwRenderState = FALSE;
	case D3DRS_ZBIAS:
		hr = m_pReal->GetRenderState(D3DRS_DEPTHBIAS,lpdwRenderState);
		break;
	case D3DRS_SOFTWAREVERTEXPROCESSING:
		*lpdwRenderState = (DWORD) m_pReal->GetSoftwareVertexProcessing();
		break;
	case D3DRS_PATCHSEGMENTS:
		fdata = m_pReal->GetNPatchMode();
		*lpdwRenderState =  *(DWORD*)&fdata; 
		break;
	default:
		hr = m_pReal->GetRenderState(dwRenderStateType,lpdwRenderState);
		break;
	}
	DEBUG_TRACE3( "CDirect3DDevice7::GetRenderState(State=%d, DWORD* *pValue=%p)=%s;\n", dwRenderStateType, *lpdwRenderState, GetResultString( hr ) );
	if (FAILED(hr))
	{
		DEBUG_MESSAGE("CDirect3DDevice7::GetRenderState() failed\nhr = %d\n", hr);
	}
	return hr;
}

STDMETHODIMP CDirect3DDevice7::BeginStateBlock()
{
	HRESULT hr = S_OK;
	hr = m_pReal->BeginStateBlock();
	DEBUG_TRACE3( "CDirect3DDevice7::BeginStateBlock()=%s;\n", GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice7::EndStateBlock( LPDWORD lpdwBlockHandle )
{
	HRESULT hr = S_OK;
	if(lpdwBlockHandle)
	{
		IDirect3DStateBlock9 *pSB;
		hr=m_pReal->EndStateBlock(&pSB);
		if(SUCCEEDED(hr)) 
		{
			*lpdwBlockHandle = (DWORD_PTR)pSB;
			m_SBHandle.push_back(*lpdwBlockHandle);
		}
		else
			DEBUG_FAILED(hr);
	}
	else 
		return D3DERR_INVALIDCALL;
	DEBUG_TRACE2( "CDirect3DDevice7::EndStateBlock(DWORD* pToken=%p)=%s;\n", lpdwBlockHandle, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice7::PreLoad( LPDIRECTDRAWSURFACE7 lpddsTexture )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetClipStatus( LPD3DCLIPSTATUS lpD3DClipStatus )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetClipStatus( LPD3DCLIPSTATUS lpD3DClipStatus )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

void CDirect3DDevice7::SetFVF(DWORD FVF)
{
	if (m_Fvf != FVF)
	{
		m_pReal->SetFVF(FVF);
		m_Fvf = FVF;
		m_VertexSize = GetVertexSize(FVF);
	}
}

STDMETHODIMP CDirect3DDevice7::DrawPrimitive( D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPVOID lpvVertices, DWORD dwVertexCount, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	SetFVF(dwVertexTypeDesc);
	UINT PrimitiveCount = GetPrimitiveCount(dptPrimitiveType, dwVertexCount);
	HRESULT hResult = m_pReal->DrawPrimitiveUP(dptPrimitiveType, PrimitiveCount, lpvVertices, m_VertexSize);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::DrawPrimitive( D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType, PVOID lpvVertices, DWORD dwVertexCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DrawPrimitiveStrided( D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwVertexCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DrawIndexedPrimitiveStrided( D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DrawPrimitiveVB( D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, DWORD dwStartVertex, DWORD dwNumVertices, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DrawPrimitiveVB( D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer, DWORD dwStartVertex, DWORD dwNumVertices, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DrawIndexedPrimitive( D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc, LPVOID lpvVertices, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	SetFVF(dwVertexTypeDesc);
	UINT PrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwVertexCount);
	HRESULT hResult = m_pReal->DrawIndexedPrimitiveUP(d3dptPrimitiveType, 0, dwVertexCount, PrimitiveCount, 
		lpwIndices, D3DFMT_INDEX16, lpvVertices, m_VertexSize);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::DrawIndexedPrimitive( D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType, LPVOID lpvVertices, DWORD dwVertexCount, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

void CDirect3DDevice7::UpdatedIndexBuffer( LPWORD lpwIndices, DWORD dwIndexCount )
{
	HRESULT hResult = S_OK;
	if (!m_pIB9 || m_IndexCount < dwIndexCount)
	{
		m_pIB9 = NULL;
		hResult = m_pReal->CreateIndexBuffer(2 * dwIndexCount, 0, D3DFMT_INDEX16, 
			D3DPOOL_DEFAULT, &m_pIB9, NULL);
		m_IndexCount = dwIndexCount;
	}
	if (m_pIB9)
	{
		LPWORD pData;
		hResult = m_pIB9->Lock(0, 2 * dwIndexCount, (void**)&pData, D3DLOCK_DISCARD);
		memcpy(pData, lpwIndices, 2 * dwIndexCount);
		hResult = m_pIB9->Unlock();
	}
	hResult = m_pReal->SetIndices(m_pIB9);
}

STDMETHODIMP CDirect3DDevice7::DrawIndexedPrimitiveVB( D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags )
{
	HRESULT hResult = S_OK;
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	CDirect3DVertexBuffer7* pVB7 = NULL;
	if (lpd3dVertexBuffer)
		lpd3dVertexBuffer->QueryInterface(IID_IWrapper, (void**)&pVB7);
	if (pVB7 != m_VB)
	{
		SAFE_RELEASE(m_VB);
		m_VB = pVB7;
		IDirect3DVertexBuffer9* pVB9 =m_VB->m_pVB;
		DWORD FVF = m_VB->m_VBDesc.dwFVF;
		SetFVF(FVF);
		NSCALL(m_pReal->SetStreamSource(0, pVB9, 0, m_VertexSize));
	}
	else
		pVB7->Release();
	UpdatedIndexBuffer(lpwIndices, dwIndexCount);

	UINT PrimitiveCount = GetPrimitiveCount(d3dptPrimitiveType, dwIndexCount);
	NSCALL(m_pReal->DrawIndexedPrimitive(d3dptPrimitiveType, 0, dwStartVertex, dwNumVertices, 0, PrimitiveCount));
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::DrawIndexedPrimitiveVB( D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER lpd3dVertexBuffer, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::ComputeSphereVisibility( LPD3DVECTOR lpCenters, LPD3DVALUE lpRadii, DWORD dwNumSpheres, DWORD dwFlags, LPDWORD lpdwReturnValues )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetTexture( DWORD dwStage, LPDIRECTDRAWSURFACE7 * lplpTexture )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	if (m_pTextures[dwStage])
		m_pTextures[dwStage]->QueryInterface(IID_IDirectDrawSurface7, (void**)lplpTexture);
	else
		*lplpTexture = NULL;
	return S_OK;
}

STDMETHODIMP CDirect3DDevice7::GetTexture( DWORD dwStage,LPDIRECT3DTEXTURE2 * lplpTexture )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetTexture( DWORD dwStage, LPDIRECTDRAWSURFACE7 lpTexture )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	SAFE_RELEASE(m_pTextures[dwStage]);
	IDirect3DTexture9* pTex9 = NULL;
	if (lpTexture)
	{
		CDirectDrawSurface7* pTex;
		lpTexture->QueryInterface(IID_IWrapper, (void**)&pTex);
		m_pTextures[dwStage] = pTex;
		if (pTex)
			pTex9 = pTex->m_pTexture;
	}
	else
		m_pTextures[dwStage] = NULL;
	HRESULT hResult = m_pReal->SetTexture(dwStage, pTex9);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetTexture( DWORD dwStage,LPDIRECT3DTEXTURE2 lpTexture )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

typedef enum _D3DTEXTURESTAGESTATETYPE7
{
	D3DTSS_ADDRESSU       = 13, /* D3DTEXTUREADDRESS for U coordinate */
	D3DTSS_ADDRESSV       = 14, /* D3DTEXTUREADDRESS for V coordinate */
	D3DTSS_BORDERCOLOR    = 15, /* D3DCOLOR */
	D3DTSS_MAGFILTER      = 16, /* D3DTEXTUREFILTER filter to use for magnification */
	D3DTSS_MINFILTER      = 17, /* D3DTEXTUREFILTER filter to use for minification */
	D3DTSS_MIPFILTER      = 18, /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
	D3DTSS_MIPMAPLODBIAS  = 19, /* float Mipmap LOD bias */
	D3DTSS_MAXMIPLEVEL    = 20, /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
	D3DTSS_MAXANISOTROPY  = 21, /* DWORD maximum anisotropy */
	D3DTSS_ADDRESSW       = 25, /* D3DTEXTUREADDRESS for W coordinate */
} D3DTEXTURESTAGESTATETYPE7;

STDMETHODIMP CDirect3DDevice7::GetTextureStageState( DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, LPDWORD lpdwValue )
{
	DEBUG_TRACE3( "CDirect3DDevice7::GetTextureStageState(Stage=%d, Type=%d, DWORD* pValue=%p);\n", dwStage, dwState, lpdwValue);
	switch(dwState)
	{
	case D3DTSS_ADDRESSU: 
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_ADDRESSU,lpdwValue);
	case D3DTSS_ADDRESSV:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_ADDRESSV,lpdwValue);
	case D3DTSS_ADDRESSW:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_ADDRESSW,lpdwValue);
	case D3DTSS_BORDERCOLOR:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_BORDERCOLOR,lpdwValue);
	case D3DTSS_MAGFILTER:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MAGFILTER,lpdwValue);
	case D3DTSS_MINFILTER:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MINFILTER,lpdwValue);
	case D3DTSS_MIPFILTER:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MIPFILTER,lpdwValue);
	case D3DTSS_MIPMAPLODBIAS:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MIPMAPLODBIAS,lpdwValue);
	case D3DTSS_MAXMIPLEVEL:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MAXMIPLEVEL,lpdwValue);
	case D3DTSS_MAXANISOTROPY:
		return m_pReal->GetSamplerState(dwStage,D3DSAMP_MAXANISOTROPY,lpdwValue);
	default:
		return m_pReal->GetTextureStageState(dwStage, dwState, lpdwValue);
	}
}

STDMETHODIMP CDirect3DDevice7::SetTextureStageState( DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue )
{
	DEBUG_TRACE3( "CDirect3DDevice7::SetTextureStageState(Stage=%d, Type=%d, DWORD Value=%p);\n", dwStage, dwState, dwValue);
	switch(dwState)
	{
	case D3DTSS_ADDRESSU: 
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_ADDRESSU,dwValue);
		break;
	case D3DTSS_ADDRESSV:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_ADDRESSV,dwValue);
		break;
	case D3DTSS_ADDRESSW:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_ADDRESSW,dwValue);
		break;
	case D3DTSS_BORDERCOLOR:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_BORDERCOLOR,dwValue);
		break;
	case D3DTSS_MAGFILTER:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MAGFILTER,dwValue);
		break;
	case D3DTSS_MINFILTER:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MINFILTER,dwValue);
		break;
	case D3DTSS_MIPFILTER:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MIPFILTER,dwValue);
		break;
	case D3DTSS_MIPMAPLODBIAS:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MIPMAPLODBIAS,dwValue);
		break;
	case D3DTSS_MAXMIPLEVEL:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MAXMIPLEVEL,dwValue);
		break;
	case D3DTSS_MAXANISOTROPY:
		return m_pReal->SetSamplerState(dwStage,D3DSAMP_MAXANISOTROPY,dwValue);
		break;
	default:
		return m_pReal->SetTextureStageState(dwStage,dwState,dwValue);
		break;
	}
}

STDMETHODIMP CDirect3DDevice7::ValidateDevice( LPDWORD lpdwPasses )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->ValidateDevice(lpdwPasses);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::ApplyStateBlock( DWORD dwBlockHandle )
{
	HRESULT hr = S_OK;
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)dwBlockHandle;
	if(pSB) 
		hr = pSB->Apply();
	else
		DEBUG_MESSAGE("CDirect3DDevice7::ApplyStateBlock pSB = NULL\n");
	DEBUG_TRACE3( "CDirect3DDevice7::ApplyStateBlock( DWORD Token=%d)=%s;\n", dwBlockHandle, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice7::CaptureStateBlock( DWORD dwBlockHandle )
{
	HRESULT hr = S_OK;
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)dwBlockHandle;
	if(pSB) 
		hr = pSB->Capture();
	else
		DEBUG_MESSAGE("CDirect3DDevice7::CaptureStateBlock pSB = NULL\n");
	DEBUG_TRACE3( "CDirect3DDevice7::CaptureStateBlock( DWORD Token=%d)=%s;\n", dwBlockHandle, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice7::DeleteStateBlock( DWORD dwBlockHandle )
{
	DEBUG_TRACE2( "CDirect3DDevice7::DeleteStateBlock( DWORD Token=%d);\n", dwBlockHandle );
	if(dwBlockHandle) 
	{
		std::vector<DWORD>::iterator pos;
		pos = std::find(m_SBHandle.begin(), m_SBHandle.end(), dwBlockHandle);
		if(pos != m_SBHandle.end())
		{
			m_SBHandle.erase(pos);
			IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)dwBlockHandle;
			NSFINALRELEASE(pSB);
		}
		return S_OK;
	}
	else 
		return D3DERR_INVALIDCALL;
}

STDMETHODIMP CDirect3DDevice7::CreateStateBlock( D3DSTATEBLOCKTYPE d3dsbType, LPDWORD lpdwBlockHandle )
{
	HRESULT hr = S_OK;
	if(lpdwBlockHandle)
	{
		IDirect3DStateBlock9 *pSB;
		hr = m_pReal->CreateStateBlock(d3dsbType, &pSB);
		DEBUG_TRACE2( "CDirect3DDevice7::CreateStateBlock(Type=%d, DWORD* pToken=%p)=%s;\n", d3dsbType, lpdwBlockHandle, GetResultString( hr ) );
		if(SUCCEEDED(hr)) 
		{
			*lpdwBlockHandle = (DWORD_PTR)pSB;
			m_SBHandle.push_back(*lpdwBlockHandle);
		}
		else
			DEBUG_FAILED(hr);
	}
	else 
		return D3DERR_INVALIDCALL;
	return hr;
}

STDMETHODIMP CDirect3DDevice7::Load( LPDIRECTDRAWSURFACE7 lpDestTex, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::LightEnable( DWORD dwLightIndex, BOOL bEnable )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->LightEnable(dwLightIndex, bEnable);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetLightEnable( DWORD dwLightIndex, BOOL* pbEnable )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetLightEnable(dwLightIndex, pbEnable);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::SetClipPlane( DWORD dwIndex, D3DVALUE* pPlaneEquation )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->SetClipPlane(dwIndex, pPlaneEquation);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetClipPlane( DWORD dwIndex, D3DVALUE* pPlaneEquation )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	HRESULT hResult = m_pReal->GetClipPlane(dwIndex, pPlaneEquation);
	return hResult;
}

STDMETHODIMP CDirect3DDevice7::GetInfo( DWORD dwDevInfoID, LPVOID pDevInfoStruct, DWORD dwSize )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SwapTextureHandles( LPDIRECT3DTEXTURE2 lpD3DTex1, LPDIRECT3DTEXTURE2 lpD3DTex2 )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SwapTextureHandles( LPDIRECT3DTEXTURE lpD3DTex1, LPDIRECT3DTEXTURE lpD3DTex2 )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::CreateExecuteBuffer( LPD3DEXECUTEBUFFERDESC lpDesc, LPDIRECT3DEXECUTEBUFFER *lplpDirect3DExecuteBuffer, IUnknown *pUnkOuter )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetStats( LPD3DSTATS lpD3DStats )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Execute( LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer, LPDIRECT3DVIEWPORT lpDirect3DViewport, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::AddViewport( LPDIRECT3DVIEWPORT3 lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s 3\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::AddViewport( LPDIRECT3DVIEWPORT2 lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::AddViewport( LPDIRECT3DVIEWPORT lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DeleteViewport( LPDIRECT3DVIEWPORT3 lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s 3\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DeleteViewport( LPDIRECT3DVIEWPORT2 lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DeleteViewport( LPDIRECT3DVIEWPORT lpDirect3DViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::NextViewport( LPDIRECT3DVIEWPORT3 lpDirect3DViewport, LPDIRECT3DVIEWPORT3 *lplpAnotherViewport, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s 3\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::NextViewport( LPDIRECT3DVIEWPORT2 lpDirect3DViewport, LPDIRECT3DVIEWPORT2 *lplpAnotherViewport, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s 2\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::NextViewport( LPDIRECT3DVIEWPORT lpDirect3DViewport, LPDIRECT3DVIEWPORT *lplpAnotherViewport, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Pick( LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer, LPDIRECT3DVIEWPORT lpDirect3DViewport, DWORD dwFlags, LPD3DRECT lpRect )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetPickRecords( LPDWORD lpCount, LPD3DPICKRECORD lpD3DPickRec )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::CreateMatrix( LPD3DMATRIXHANDLE lpD3DMatHandle )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetMatrix( D3DMATRIXHANDLE d3dMatHandle, LPD3DMATRIX lpD3DMatrix )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetMatrix( D3DMATRIXHANDLE D3DMatHandle, LPD3DMATRIX lpD3DMatrix )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::DeleteMatrix( D3DMATRIXHANDLE d3dMatHandle )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetCurrentViewport( LPDIRECT3DVIEWPORT3 lpd3dViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetCurrentViewport( LPDIRECT3DVIEWPORT2 lpd3dViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetCurrentViewport( LPDIRECT3DVIEWPORT3 *lplpd3dViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetCurrentViewport( LPDIRECT3DVIEWPORT2 *lplpd3dViewport )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Begin( D3DPRIMITIVETYPE d3dpt, DWORD dwVertexTypeDesc, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Begin( D3DPRIMITIVETYPE d3dpt, D3DVERTEXTYPE d3dvt, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::BeginIndexed( D3DPRIMITIVETYPE dptPrimitiveType, DWORD dwVertexTypeDesc, LPVOID lpvVertices, DWORD dwNumVertices, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::BeginIndexed( D3DPRIMITIVETYPE dptPrimitiveType, D3DVERTEXTYPE dvtVertexType, LPVOID lpvVertices, DWORD dwNumVertices, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Vertex( LPVOID lpVertex )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::Index( WORD wVertexIndex )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::End( DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::GetLightState( D3DLIGHTSTATETYPE dwLightStateType, LPDWORD lpdwLightState )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirect3DDevice7::SetLightState( D3DLIGHTSTATETYPE dwLightStateType, DWORD dwLightState )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}
