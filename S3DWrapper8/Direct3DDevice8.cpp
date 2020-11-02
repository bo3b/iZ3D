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
#include "Direct3DDevice8.h"
#include "Direct3DSwapChain8.h"
#include "Direct3DSurface8.h"
#include "Direct3DTexture8.h"
#include "Direct3DCubeTexture8.h"
#include "Direct3DVolumeTexture8.h"
#include "Direct3DVertexBuffer8.h"
#include "Direct3DIndexBuffer8.h"
#include "Direct3DShader8.h"
#include "Utils.h"
#include <algorithm>
#include <atltypes.h>

FILE *stream = NULL;

// Shader disassembler variables
extern LONG textsz;
extern BOOL put_pnt;
extern LPSTR sstream;

CDirect3DDevice8::CDirect3DDevice8():
m_pDirect3D(NULL), m_BaseVertexIndex(0)
{
}

CDirect3DDevice8::~CDirect3DDevice8()
{
	DEBUG_TRACE1("CDirect3DDevice8::~CDirect3DDevice8()  Device=%p\n", this);
	DEBUG_TRACE1("Delete %d VS\n", m_VSHandle.size());
	for(size_t i = 0; i < m_VSHandle.size(); i++)
		NSFINALRELEASE((CDirect3DVertexShader8*)m_VSHandle[i]);
	DEBUG_TRACE1("Delete %d PS\n", m_PSHandle.size());
	for(size_t i = 0; i < m_PSHandle.size(); i++)
		NSFINALRELEASE((IDirect3DPixelShader9*)m_PSHandle[i]);
	DEBUG_TRACE1("Delete %d SB\n", m_SBHandle.size());
	for(size_t i = 0; i < m_SBHandle.size(); i++)
		NSFINALRELEASE((IDirect3DStateBlock9*)m_SBHandle[i]);

	KillMcClaud();

#ifndef FINAL_RELEASE
	m_pWrapperTable->DumpState();
#endif
	put_pnt = TRUE;
	stream = NULL;
	sstream = NULL;
	textsz = 0;
}

void CDirect3DDevice8::CreateMcClaud()
{
	//--- keep backbuffer ---
	CComPtr<IDirect3DSurface8> pBackBuffer;
	GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	CComQIPtr<IWrapper> pWrapperBack(pBackBuffer);
	pWrapperBack->SetMcClaudFlag();


	//--- keep depth stencil ---
	CComPtr<IDirect3DSurface8> pDepthStencil;
	GetDepthStencilSurface(&pDepthStencil);
	if (pDepthStencil)
	{
		CComQIPtr<IWrapper> pWrapperDepth(pDepthStencil);
		pWrapperDepth->SetMcClaudFlag();
	}
}

void CDirect3DDevice8::KillMcClaud()
{
	//--- free backbuffer ---
	IDirect3DSurface8* pSurface;
	GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	CComQIPtr<IWrapper> pWrapperBack(pSurface);
	pWrapperBack->ClearMcClaudFlag();
	pSurface->Release();
	//--- destroy McClaud wrapper ---
	pSurface->Release();

	//--- free depth stencil ---
	GetDepthStencilSurface(&pSurface);
	if (pSurface)
	{
		CComQIPtr<IWrapper> pWrapperDepth(pSurface);
		pWrapperDepth->ClearMcClaudFlag();
		pSurface->Release();
		//--- destroy McClaud wrapper ---
		pSurface->Release();
	}
}

STDMETHODIMP CDirect3DDevice8::CreateAdditionalSwapChain( D3DPRESENT_PARAMETERS8* pPresentationParameters,IDirect3DSwapChain8** ppSwapChain)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DSwapChain9> pChain;
	D3DPRESENT_PARAMETERS pp9;
	ConvertPresentParameters(pPresentationParameters,&pp9);
	hr=m_pReal->CreateAdditionalSwapChain(&pp9,&pChain);
	if (SUCCEEDED(hr))
		CreateWrapper(pChain.p, ppSwapChain);
	else
		DEBUG_FAILED(hr);

	DEBUG_TRACE1("CDirect3DDevice8::CreateAdditionalSwapChain( pPresentationParameters, ppSwapChain)=%s, *ppSwapChain=%p\n", 
		GetResultString(hr), ppSwapChain ? *ppSwapChain : 0);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::Reset( D3DPRESENT_PARAMETERS8* pPresentationParameters)
{	
	D3DPRESENT_PARAMETERS pp9;
	ConvertPresentParameters(pPresentationParameters,&pp9);

	KillMcClaud();

#ifndef FINAL_RELEASE
	m_pWrapperTable->DumpState();
#endif

	DEBUG_TRACE1("Delete %d SB\n", m_SBHandle.size());
	for(size_t i = 0; i < m_SBHandle.size(); i++)
		NSFINALRELEASE((IDirect3DStateBlock9*)m_SBHandle[i]);

	HRESULT hr = m_pReal->Reset(&pp9);
	DEBUG_TRACE1("CDirect3DDevice8::Reset( D3DPRESENT_PARAMETERS8* pPresentationParameters)\n");	
	if (FAILED(hr))
	{
		DEBUG_FAILED(hr);
	}
	else
		CreateMcClaud();
	return hr;
}

STDMETHODIMP CDirect3DDevice8::Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	HRESULT hr = m_pReal->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion);
	if(pSourceRect && pDestRect && pDirtyRegion )
	{
		DEBUG_TRACE3("CDirect3DDevice8::Present( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)=%s, pSourceRect->bottom=%d, pSourceRect->left=%d,  pSourceRect->right=%d, pSourceRect->top=%d,  pDestRect->bottom=%d, pDestRect->left=%d,  pDestRect->right=%d, pDestRect->top=%d,	hDestWindowOverride=%d, pDirtyRegion->Buffer=%d, pDirtyRegion->rdh.dwSize=%d, pDirtyRegion->rdh.iType=%d, pDirtyRegion->rdh.nCount=%d, pDirtyRegion->rdh.nRgnSize=%d, pDirtyRegion->rdh.rcBound.bottom=%d, pDirtyRegion->rdh.rcBound.left=%d,  pDirtyRegion->rdh.rcBound.right=%d, pDirtyRegion->rdh.rcBound.top=%d\n", 
			GetResultString(hr), pSourceRect->bottom, pSourceRect->left,  pSourceRect->right, pSourceRect->top,  pDestRect->bottom, pDestRect->left,  pDestRect->right, pDestRect->top, hDestWindowOverride, 
			pDirtyRegion->Buffer, pDirtyRegion->rdh.dwSize, pDirtyRegion->rdh.iType, pDirtyRegion->rdh.nCount, pDirtyRegion->rdh.nRgnSize, pDirtyRegion->rdh.rcBound.bottom, pDirtyRegion->rdh.rcBound.left,  pDirtyRegion->rdh.rcBound.right, pDirtyRegion->rdh.rcBound.top);
	}
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateTexture( UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture8** ppTexture)
{
	HRESULT hr = S_OK;

	if (Pool == D3DPOOL_DEFAULT && Usage == 0 && IsCompressedFormat(Format))
		Usage = D3DUSAGE_DYNAMIC; // for CopyRects - Rome Total War

	CComPtr<IDirect3DTexture9> pTexture;
	hr=m_pReal->CreateTexture(Width,Height,Levels,Usage,Format,Pool,&pTexture,NULL);
	if(SUCCEEDED(hr))
		CreateWrapper(pTexture.p, ppTexture);
	else
	{
		*ppTexture = NULL;
		DEBUG_FAILED(hr);
	}
	DEBUG_TRACE2("CDirect3DDevice8::CreateTexture(Width=%d, Height=%d, Levels=%d, Usage=%s, Format=%s, Pool=%s, *ppTexture=%p)=%s\n", 
		Width, Height, Levels, GetUsageString( Usage ), GetFormatString(Format), GetPoolString( Pool ), 
		ppTexture? *ppTexture: 0, GetResultString( hr ));	
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateVolumeTexture( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture8** ppVolumeTexture)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DVolumeTexture9> pTexture;
	hr=m_pReal->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format,Pool,&pTexture,NULL);
	if(SUCCEEDED(hr))
		CreateWrapper(pTexture.p, ppVolumeTexture);
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2( "CDirect3DDevice8::CreateVolumeTexture(Width=%d, Height=%d, Depth=%d, Levels=%d, Usage=%s, Format=%s, Pool=%s, IDirect3DVolumeTexture8** ppVolumeTexture=%p)=%s;\n", 
		Width, Height, Depth, Levels, GetUsageString( Usage ), GetFormatString(Format), GetPoolString( Pool ), ppVolumeTexture, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateCubeTexture( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture8** ppCubeTexture)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DCubeTexture9> pTexture;
	hr=m_pReal->CreateCubeTexture(EdgeLength,Levels,Usage,Format,Pool,&pTexture,NULL);
	if(SUCCEEDED(hr)) 
		CreateWrapper(pTexture.p, ppCubeTexture);
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2( "CDirect3DDevice8::CreateCubeTexture(EdgeLength=%d, Levels=%d, Usage=%s, Format=%s, Pool=%s, IDirect3DCubeTexture8** ppCubeTexture=%p)=%s;\n", 
		EdgeLength, Levels, GetUsageString( Usage ), GetFormatString(Format), GetPoolString( Pool ), ppCubeTexture, GetResultString( hr ) );
	return hr;
}
STDMETHODIMP CDirect3DDevice8::CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer8** ppVertexBuffer)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DVertexBuffer9> pVB;
	hr=m_pReal->CreateVertexBuffer(Length,Usage,FVF,Pool,&pVB,NULL);
	if(SUCCEEDED(hr))
	{
		if (FVF)
		{
			m_pReal->SetFVF(FVF);
			DEBUG_TRACE3( "\tSetFVF\n" );
		}
		CreateWrapper(pVB.p, ppVertexBuffer);
	}
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2( "CDirect3DDevice8::CreateVertexBuffer(Length=%d, Usage=%s, FVF=%d, Pool=%s, IDirect3DVertexBuffer8** ppVertexBuffer=%p)=%s;\n", 
		Length, GetUsageString( Usage ), FVF, GetPoolString( Pool ), ppVertexBuffer, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateIndexBuffer( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer8** ppIndexBuffer)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DIndexBuffer9> pIB;
	hr=m_pReal->CreateIndexBuffer(Length,Usage,Format,Pool,&pIB,NULL);
	if(SUCCEEDED(hr))
		CreateWrapper(pIB.p, ppIndexBuffer);
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2( "CDirect3DDevice8::CreateIndexBuffer(Length=%d, Usage=%s, Format=%s, Pool=%s, IDirect3DIndexBuffer8** ppIndexBuffer=%p)=%s;\n",
		Length, GetUsageString( Usage ), GetFormatString(Format), GetPoolString( Pool ), ppIndexBuffer, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateRenderTarget( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,BOOL Lockable,IDirect3DSurface8** ppSurface)
{
	HRESULT hr = S_OK;	
	CComPtr<IDirect3DSurface9> pSurf;
	hr=m_pReal->CreateRenderTarget(Width,Height,Format,MultiSample,0,Lockable,&pSurf,NULL);
	if(SUCCEEDED(hr))
		CreateWrapper(pSurf.p, ppSurface);
	else 
		DEBUG_FAILED(hr);
	DEBUG_TRACE2("CDirect3DDevice8::CreateRenderTarget(Width=%d, Height=%d, Format=%s, MultiSample=%d,"
		"Lockable=%1d, *ppSurface=%p)", 
		Width, Height, GetFormatString(Format), MultiSample, Lockable,  ppSurface ? *ppSurface : 0);	
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateDepthStencilSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,IDirect3DSurface8** ppSurface)
{
	HRESULT hr = S_OK;	
	CComPtr<IDirect3DSurface9> pSurf;
	hr=m_pReal->CreateDepthStencilSurface(Width,Height,Format,MultiSample,0,FALSE,&pSurf,NULL);//TRUE
	if(SUCCEEDED(hr))
		CreateWrapper(pSurf.p, ppSurface);
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2("CDirect3DDevice8::CreateDepthStencilSurface(Width=%d, Height=%d, Format=%s, MultiSample=%d,*ppSurface=%p)=%s", 
		Width, Height, GetFormatString(Format), MultiSample, ppSurface ? *ppSurface : 0, GetResultString( hr ));
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateImageSurface( UINT Width,UINT Height,D3DFORMAT Format,IDirect3DSurface8** ppSurface)
{	
	HRESULT hr = S_OK;	
	CComPtr<IDirect3DSurface9> pSurf;
	hr=m_pReal->CreateOffscreenPlainSurface(Width,Height,Format,D3DPOOL_SYSTEMMEM,&pSurf,NULL);
	if(SUCCEEDED(hr))
		CreateWrapper(pSurf.p, ppSurface);
	else
	{
		CComPtr<IDirect3DTexture9> pTex; // DXT surfaces - Perfect World
		hr=m_pReal->CreateTexture(Width,Height,1,0,Format,D3DPOOL_SYSTEMMEM,&pTex,NULL);
		if(SUCCEEDED(hr))
			hr=pTex->GetSurfaceLevel(0, &pSurf);
	}
	if(SUCCEEDED(hr))
		CreateWrapper(pSurf.p, ppSurface);
	else
		DEBUG_FAILED(hr);
	DEBUG_TRACE2( "CDirect3DDevice8::CreateImageSurface(Width=%d, Height=%d, Format=%s, Pool=D3DPOOL_SYSTEMMEM, IDirect3DSurface8* *ppSurface=%p)=%s;\n", 
		Width, Height, GetFormatString(Format), ppSurface ? *ppSurface : 0, GetResultString( hr ) );
	return hr;
}

RECT GetDestRect(int i, CONST RECT* pSourceRectsArray, CONST POINT* pDestPointsArray)
{
	CRect dstRect = pSourceRectsArray[i];
	if (pDestPointsArray)
	{
		dstRect.OffsetRect(pDestPointsArray[i].x - pSourceRectsArray[i].left,
			pDestPointsArray[i].y - pSourceRectsArray[i].top);
	}
	return dstRect;
}

STDMETHODIMP CDirect3DDevice8::CopyRectsUsingLock( IDirect3DSurface9* pSrc,D3DSURFACE_DESC &sourceDesc,CONST RECT* pSourceRectsArray,UINT cRects,
												   IDirect3DSurface9* pDest,D3DSURFACE_DESC &destDesc,CONST POINT* pDestPointsArray)
{
	HRESULT hResult = S_OK;
	DEBUG_TRACE2( "CopyRects -> Lock path\n" );
	D3DLOCKED_RECT rsrc, rdst;
	hResult = pSrc->LockRect(&rsrc, NULL, D3DLOCK_READONLY);
	CComPtr<IDirect3DSurface9> pTempSrcSurf;
	if (SUCCEEDED(hResult))
		pTempSrcSurf = pSrc;
	else if (sourceDesc.Pool == D3DPOOL_DEFAULT)
	{
		//NSCALL(m_pReal->CreateOffscreenPlainSurface(sourceDesc.Width, sourceDesc.Height,
		//	sourceDesc.Format, D3DPOOL_SYSTEMMEM, &pTempSrcSurf, NULL));
		//NSCALL(m_pReal->GetRenderTargetData(pSrc, pTempSrcSurf));

		NSCALL(m_pReal->CreateRenderTarget(sourceDesc.Width, sourceDesc.Height,
			sourceDesc.Format, D3DMULTISAMPLE_NONE, 0, TRUE, &pTempSrcSurf, NULL));
		if (FAILED(hResult))
			return D3DERR_INVALIDCALL;
		NSCALL(m_pReal->StretchRect(pSrc, NULL, pTempSrcSurf, NULL, D3DTEXF_NONE));
		if (FAILED(hResult))
			return D3DERR_INVALIDCALL;
		NSCALL(pTempSrcSurf->LockRect(&rsrc, NULL, D3DLOCK_READONLY));
		if (FAILED(hResult))
			return D3DERR_INVALIDCALL;
	} else 
		return D3DERR_INVALIDCALL;
	hResult = pDest->LockRect(&rdst, NULL, 0);
	CComPtr<IDirect3DSurface9> pTempDstSurf;
	if (SUCCEEDED(hResult))
		pTempDstSurf = pDest;
	else if (destDesc.Pool == D3DPOOL_DEFAULT)
	{
		NSCALL(m_pReal->CreateOffscreenPlainSurface(destDesc.Width, destDesc.Height,
			destDesc.Format, D3DPOOL_SYSTEMMEM, &pTempDstSurf, NULL));
		if (FAILED(hResult))
		{
			NSCALL(pTempSrcSurf->UnlockRect());
			return D3DERR_INVALIDCALL;
		}
		NSCALL(pTempDstSurf->LockRect(&rdst, NULL, 0));
		if (FAILED(hResult))
		{
			NSCALL(pTempSrcSurf->UnlockRect());
			return D3DERR_INVALIDCALL;
		}
	}
	else
	{
		NSCALL(pTempSrcSurf->UnlockRect());
		return D3DERR_INVALIDCALL;
	}

	LPBYTE sp = (LPBYTE)rsrc.pBits;
	LPBYTE dp = (LPBYTE)rdst.pBits;

	bool bCompressed = IsCompressedFormat(sourceDesc.Format);
	UINT bpp = BitsPerPixel(sourceDesc.Format);
	_ASSERT(bpp > 0);
	/* CopyRects does not clip or anything */
	for (UINT i = 0; i < cRects; i++) {
		CONST RECT* crect = &pSourceRectsArray[i];
		POINT startPoint = { crect->left, crect->top };
		CONST POINT* cpoint = pDestPointsArray ? &pDestPointsArray[i] : &startPoint;
		INT sourceLinePitch = rsrc.Pitch * crect->top;
		INT destLinePitch = rdst.Pitch * cpoint->y;
		UINT lineWidth = crect->right - crect->left;
		UINT lineCount = crect->bottom - crect->top;
		if (bCompressed)
		{
			// start position should be divided by 4 (compressed 4x4 block)
			_ASSERT(crect->left % 4 == 0);
			_ASSERT(crect->top % 4 == 0);
			_ASSERT(cpoint->x % 4 == 0);
			_ASSERT(cpoint->y % 4 == 0);

			// adjust line pixel count
			lineWidth = RoundUp(lineWidth, 4) * 4;
			// adjust number of lines
			lineCount = RoundUp(lineCount, 4) / 4;
			// adjust lines offset (Pitch = 4 lines)
			sourceLinePitch /= 4;
			destLinePitch /= 4;
		}
		UINT lineLength = lineWidth * bpp / 8;
		LPBYTE sourceLine = sp + sourceLinePitch + crect->left * bpp / 8;
		LPBYTE destLine = dp + destLinePitch + cpoint->x * bpp / 8;

		if (lineLength == rsrc.Pitch && rsrc.Pitch == rdst.Pitch)
			memcpy(destLine, sourceLine, lineCount * lineLength);
		else
		{
			while (lineCount) {
				memcpy(destLine, sourceLine, lineLength);
				sourceLine += rsrc.Pitch;
				destLine += rdst.Pitch;
				lineCount--;
			}
		}
	}

	NSCALL(pTempDstSurf->UnlockRect());
	NSCALL(pTempSrcSurf->UnlockRect());	
	if (pTempDstSurf != pDest)
	{
		NSCALL(m_pReal->UpdateSurface(pTempDstSurf, NULL, pDest, NULL));
	}
	return hResult;
}

STDMETHODIMP CDirect3DDevice8::CopyRects( IDirect3DSurface8* pSourceSurface,CONST RECT* pSourceRectsArray,UINT cRects,IDirect3DSurface8* pDestinationSurface,CONST POINT* pDestPointsArray)
{
	HRESULT hResult = D3DERR_INVALIDCALL;
	IDirect3DSurface9* pSrc = Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pSourceSurface);
	IDirect3DSurface9* pDest = Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pDestinationSurface);
	D3DSURFACE_DESC sourceDesc;
	pSrc->GetDesc(&sourceDesc);
	D3DSURFACE_DESC destDesc;
	pDest->GetDesc(&destDesc);

	RECT sourceRect = { 0, 0, sourceDesc.Width, sourceDesc.Height };
	POINT destPoint = { 0, 0 };
	if (cRects == 0) 
	{
		if ((sourceRect.right != destDesc.Width) ||
			(sourceRect.bottom != destDesc.Height))
		{
			NSCALL(D3DERR_INVALIDCALL);
			return hResult;
		}
		pSourceRectsArray = &sourceRect;
		pDestPointsArray = &destPoint;
		cRects = 1;
	}

	if (sourceDesc.Format == destDesc.Format)
	{
		if (sourceDesc.Pool == D3DPOOL_SYSTEMMEM && destDesc.Pool == D3DPOOL_DEFAULT)
		{
			DEBUG_TRACE2( "CopyRects -> UpdateSurface path\n" );
			for (DWORD i = 0; i < cRects; i++)
			{
				RECT dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
				NSCALL(m_pReal->UpdateSurface(pSrc, &pSourceRectsArray[i], pDest, &pDestPointsArray[i]));
			}
		}
		else if (sourceDesc.Pool == D3DPOOL_DEFAULT && destDesc.Pool == D3DPOOL_SYSTEMMEM)
		{
			DEBUG_TRACE2( "CopyRects -> GetRenderTargetData path\n" );
			CComPtr<IDirect3DSurface9> pTempSurf;
			if (sourceDesc.Width == destDesc.Width && sourceDesc.Height == destDesc.Height &&
				cRects == 1 && EqualRect(&pSourceRectsArray[0], &sourceRect) &&
				(sourceDesc.Usage & D3DUSAGE_RENDERTARGET) != 0 && 
				(pDestPointsArray == NULL || pDestPointsArray[0].x == 0 && pDestPointsArray[0].y == 0))
			{
				hResult = S_OK;
				pTempSurf = pSrc;
			}
			else
			{
				DEBUG_TRACE2( "\tCreate temporal RT\n" );
				NSCALL(m_pReal->CreateRenderTarget(destDesc.Width, destDesc.Height,
					destDesc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pTempSurf, NULL));
				if (SUCCEEDED(hResult))
				{
					for (DWORD i = 0; i < cRects; i++)
					{
						RECT dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
						NSCALL(m_pReal->StretchRect(pSrc, &pSourceRectsArray[i], pTempSurf, &dstRect, D3DTEXF_NONE));
					}
				}
			}
			if (SUCCEEDED(hResult))
				NSCALL(m_pReal->GetRenderTargetData(pTempSurf, pDest));
		}
		else if (sourceDesc.Pool == D3DPOOL_DEFAULT && 
			destDesc.Pool == D3DPOOL_DEFAULT && (destDesc.Usage & D3DUSAGE_RENDERTARGET) != 0)
		{
			DEBUG_TRACE2( "CopyRects -> StretchRect path\n" );
			for (DWORD i = 0; i < cRects; i++)
			{
				RECT dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
				NSCALL(m_pReal->StretchRect(pSrc, &pSourceRectsArray[i], pDest, &dstRect, D3DTEXF_NONE));
			}
		}
		if (FAILED(hResult))
		{
			hResult = CopyRectsUsingLock(pSrc, sourceDesc, pSourceRectsArray, cRects, 
				pDest, destDesc, pDestPointsArray);
/*#ifndef FINAL_RELEASE
			if (FAILED(hResult))
			{
				// compress/decompress surfaces (slow path)
				DEBUG_TRACE2( "CopyRects -> D3DXLoadSurfaceFromSurface path\n" );
				for (DWORD i = 0; i < cRects; i++)
				{
					CRect dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
					NSCALL(D3DXLoadSurfaceFromSurface(pDest, NULL, &dstRect, pSrc, NULL, &pSourceRectsArray[i], D3DX_FILTER_NONE, 0));
				}
				_ASSERT(FAILED(hResult));
			}
#endif*/
		}
	}
	else
	{
		IDirect3D9* pDirect3D = NULL;
		m_pDirect3D->GetReal((IUnknown**)&pDirect3D);
		bool bConvSupported = SUCCEEDED(pDirect3D->CheckDeviceFormatConversion(m_Adapter, m_DeviceType, 
			sourceDesc.Format, destDesc.Format));
		if (bConvSupported)
		{
			DEBUG_TRACE2( "CopyRects: Format conversion supported from %s to %s\n", 
				GetFormatString(sourceDesc.Format), GetFormatString(destDesc.Format) );
			bool bDestRTSupported = SUCCEEDED(pDirect3D->CheckDeviceFormat(m_Adapter, m_DeviceType, m_BackBufferFormat, 
				D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, destDesc.Format));
			if (sourceDesc.Pool == D3DPOOL_DEFAULT && destDesc.Pool == D3DPOOL_SYSTEMMEM && bDestRTSupported)
			{
				DEBUG_TRACE2( "CopyRects -> GetRenderTargetData path\n" );
				DEBUG_TRACE2( "\tCreate temporal RT\n" );
				CComPtr<IDirect3DSurface9> pTempSurf;
				NSCALL(m_pReal->CreateRenderTarget(destDesc.Width, destDesc.Height,
					destDesc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pTempSurf, NULL));
				if (SUCCEEDED(hResult))
				{
					for (DWORD i = 0; i < cRects; i++)
					{
						RECT dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
						NSCALL(m_pReal->StretchRect(pSrc, &pSourceRectsArray[i], pTempSurf, &dstRect, D3DTEXF_NONE));
					}
					if (SUCCEEDED(hResult))
					{
						NSCALL(m_pReal->GetRenderTargetData(pTempSurf, pDest));
					}
				}
			}
			else if (sourceDesc.Pool == D3DPOOL_DEFAULT && 
				destDesc.Pool == D3DPOOL_DEFAULT && (destDesc.Usage & D3DUSAGE_RENDERTARGET) != 0)
			{
				DEBUG_TRACE2( "CopyRects -> StretchRect path\n" );
				for (DWORD i = 0; i < cRects; i++)
				{
					RECT dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
					NSCALL(m_pReal->StretchRect(pSrc, &pSourceRectsArray[i], pDest, &dstRect, D3DTEXF_NONE));
				}
			}
		}
		if (FAILED(hResult))
		{
			// compress/decompress surfaces (slow path)
			DEBUG_TRACE2( "CopyRects -> D3DXLoadSurfaceFromSurface path\n" );
			for (DWORD i = 0; i < cRects; i++)
			{
				CRect dstRect = GetDestRect(i, pSourceRectsArray, pDestPointsArray);
				NSCALL(D3DXLoadSurfaceFromSurface(pDest, NULL, &dstRect, pSrc, NULL, &pSourceRectsArray[i], D3DX_FILTER_NONE, 0));
			}
		}
	}
	if(FAILED(hResult) && GINFO_DEBUG)
	{
		DEBUG_MESSAGE( "CDirect3DDevice8::CopyRects(IDirect3DSurface8* pSourceSurface=%p, CONST RECT* pSourceRectsArray=%p,UINT cRects=%d,IDirect3DSurface8* pDestinationSurface=%p,CONST POINT* pDestPointsArray=%p)=%s;\n", pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray, GetResultString( hResult ) );
		DEBUG_MESSAGE("CDirect3DDevice8::CopyRects(): failed\n");
		DEBUG_MESSAGE("\t%s\t-> %s\n", GetPoolString(sourceDesc.Pool), GetPoolString(destDesc.Pool));
		DEBUG_MESSAGE("\t%s\t-> %s\n", GetFormatString(sourceDesc.Format), GetFormatString(destDesc.Format));
		DEBUG_MESSAGE("\t%s\t-> %s\n", GetUsageString(sourceDesc.Usage), GetUsageString(destDesc.Usage));
		DEBUG_MESSAGE("\t(%d x %d)\t-> (%d x %d)\n", sourceDesc.Width, sourceDesc.Height, destDesc.Width, destDesc.Height);		
	}
	else {
		DEBUG_TRACE2( "CDirect3DDevice8::CopyRects(IDirect3DSurface8* pSourceSurface=%p, CONST RECT* pSourceRectsArray=%p,UINT cRects=%d,IDirect3DSurface8* pDestinationSurface=%p,CONST POINT* pDestPointsArray=%p)=%s;\n", pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray, GetResultString( hResult ) );
	}
	return hResult;
}

STDMETHODIMP CDirect3DDevice8::SetRenderTarget( IDirect3DSurface8* pRenderTarget,IDirect3DSurface8* pNewZStencil)
{
	HRESULT hr = S_OK;
	DEBUG_TRACE3( "CDirect3DDevice8::SetRenderTarget( IDirect3DSurface8* pRenderTarget=%p,IDirect3DSurface8* pNewZStencil=%p)\n", pRenderTarget, pNewZStencil );
	if(pRenderTarget) 
	{
		hr=m_pReal->SetRenderTarget(0, Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pRenderTarget));
		if(FAILED(hr)){
			DEBUG_MESSAGE("CDirect3DDevice9::SetRenderTarget() failed\nhr = %d\npRenderTarget = %p\n", hr, pRenderTarget);
			return hr;
		}
	}
	if(pNewZStencil) 
	{
		CComPtr<IDirect3DSurface9> pOldDS9;
		m_pReal->GetDepthStencilSurface(&pOldDS9);
		IDirect3DSurface9* pNewDS9 = Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pNewZStencil);
		if(pOldDS9.p != pNewDS9){
			hr=m_pReal->SetDepthStencilSurface(pNewDS9);
			if(FAILED(hr)){
				DEBUG_MESSAGE("CDirect3DDevice9::SetDepthStencilSurface() failed\nhr = %d\npNewZStencil = %p\n", hr, pNewZStencil);
			}
		}
	}
	else{
		DEBUG_TRACE3("CDirect3DDevice9::SetDepthStencilSurface(NULL)\n");
		hr=m_pReal->SetDepthStencilSurface(NULL);
	}

	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetRenderTarget( IDirect3DSurface8** ppRenderTarget)
{
	HRESULT hr = S_OK;
	if(!ppRenderTarget) 
		return D3DERR_INVALIDCALL;
	CComPtr<IDirect3DSurface9> pRT;
	hr=m_pReal->GetRenderTarget(0,&pRT);
	DEBUG_TRACE3( "CDirect3DDevice8::GetRenderTarget( IDirect3DSurface8* *ppRenderTarget=%p)=%s;\n", 
		ppRenderTarget ? *ppRenderTarget : 0, GetResultString( hr ) );
	if(SUCCEEDED(hr))
		Wrap(pRT.p, ppRenderTarget);
	else
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetDepthStencilSurface(IDirect3DSurface8** ppZStencilSurface)
{
	HRESULT hr = S_OK;
	if(!ppZStencilSurface) 
		return D3DERR_INVALIDCALL;
	CComPtr<IDirect3DSurface9> pStencil;
	hr=m_pReal->GetDepthStencilSurface(&pStencil);
	DEBUG_TRACE3( "CDirect3DDevice8::GetDepthStencilSurface(IDirect3DSurface8* *ppZStencilSurface=%p)=%s;\n", ppZStencilSurface ? *ppZStencilSurface : 0, GetResultString( hr ) );
	if(SUCCEEDED(hr))
		Wrap( pStencil.p, ppZStencilSurface);
	else
	{
		*ppZStencilSurface = NULL;
		if (!pStencil)
		{
			DEBUG_TRACE3("CDirect3DDevice8::GetDepthStencilSurface() NULL\n");
		}
		else
		{
			DEBUG_MESSAGE("CDirect3DDevice8::GetDepthStencilSurface() failed\nhr = %d\n", hr);
		}
	}
	return hr;
}

typedef enum _D3DRENDERSTATETYPE8 {    
	D3DRS_LINEPATTERN               =  10,    
	D3DRS_ZVISIBLE                  =  30,    
	D3DRS_EDGEANTIALIAS             =  40,    
	D3DRS_ZBIAS                     =  47,    
	D3DRS_SOFTWAREVERTEXPROCESSING  = 153,    
	D3DRS_PATCHSEGMENTS             = 164,
	D3DRS8_FORCE_DWORD               = 0x7fffffff
} D3DRENDERSTATETYPE8;

STDMETHODIMP CDirect3DDevice8::SetRenderState( D3DRENDERSTATETYPE State,DWORD Value)
{
	DEBUG_TRACE3( "CDirect3DDevice8::SetRenderState(D3DRENDERSTATETYPE State=%d, DWORD Value=%p);\n", State,Value);
	switch(State)
	{
	case D3DRS_LINEPATTERN:
	case D3DRS_ZVISIBLE:
		return S_OK;
	case D3DRS_EDGEANTIALIAS: 
		if (Value == 0)
			return S_OK;
		else
			return D3DERR_INVALIDCALL;
	case D3DRS_ZBIAS:
	{
		float convertetBias = -0.1e-4f * Value;
		return m_pReal->SetRenderState(D3DRS_DEPTHBIAS,(DWORD&)convertetBias);
	}
	case D3DRS_SOFTWAREVERTEXPROCESSING:
		return m_pReal->SetSoftwareVertexProcessing((BOOL) Value);
	case D3DRS_PATCHSEGMENTS:
		if (*((float*)&Value) <= 1) 
			return m_pReal->SetNPatchMode( 0);
		else
			return m_pReal->SetNPatchMode( *((float*)&Value));
	default:
		return m_pReal->SetRenderState(State,Value);
	}
}

typedef struct _myD3DLINEPATTERN {
	WORD    wRepeatFactor;
	WORD    wLinePattern;
} myD3DLINEPATTERN;

STDMETHODIMP CDirect3DDevice8::GetRenderState( D3DRENDERSTATETYPE State,DWORD* pValue)
{
	HRESULT hr = S_OK;
	float fdata;
	switch(State)
	{
	case D3DRS_LINEPATTERN:
		memset(pValue, 0, sizeof(myD3DLINEPATTERN));
		break;
	case D3DRS_ZVISIBLE:
		*pValue = FALSE;
		break;
	case D3DRS_EDGEANTIALIAS:
		*pValue = FALSE;
	case D3DRS_ZBIAS:
		hr = m_pReal->GetRenderState(D3DRS_DEPTHBIAS,pValue);
		break;
	case D3DRS_SOFTWAREVERTEXPROCESSING:
		*pValue = (DWORD) m_pReal->GetSoftwareVertexProcessing();
		break;
	case D3DRS_PATCHSEGMENTS:
		fdata = m_pReal->GetNPatchMode();
		*pValue =  *(DWORD*)&fdata; 
		break;
	default:
		hr = m_pReal->GetRenderState(State,pValue);
		break;
	}
	DEBUG_TRACE3( "CDirect3DDevice8::GetRenderState(State=%d, DWORD* *pValue=%p)=%s;\n", State, *pValue, GetResultString( hr ) );
	if (FAILED(hr))
	{
		DEBUG_MESSAGE("CDirect3DDevice8::GetRenderState() failed\nhr = %d\n", hr);
	}
	return hr;
}

STDMETHODIMP CDirect3DDevice8::EndStateBlock( DWORD* pToken)
{
	HRESULT hr = S_OK;
	if(pToken)
	{
		IDirect3DStateBlock9 *pSB;
		hr=m_pReal->EndStateBlock(&pSB);
		if(SUCCEEDED(hr)) 
		{
			*pToken = (DWORD_PTR)pSB;
			m_SBHandle.push_back(*pToken);
		}
		else
			DEBUG_FAILED(hr);
	}
	else 
		return D3DERR_INVALIDCALL;
	DEBUG_TRACE2( "CDirect3DDevice8::EndStateBlock(DWORD* pToken=%p)=%s;\n", pToken, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreateStateBlock( D3DSTATEBLOCKTYPE Type, DWORD* pToken)
{
	HRESULT hr = S_OK;
	if(pToken)
	{
		IDirect3DStateBlock9 *pSB;
		hr = m_pReal->CreateStateBlock(Type, &pSB);
		DEBUG_TRACE2( "CDirect3DDevice8::CreateStateBlock(Type=%d, DWORD* pToken=%p)=%s;\n", Type, pToken, GetResultString( hr ) );
		if(SUCCEEDED(hr)) 
		{
			*pToken = (DWORD_PTR)pSB;
			m_SBHandle.push_back(*pToken);
		}
		else
			DEBUG_FAILED(hr);
	}
	else 
		return D3DERR_INVALIDCALL;
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DeleteStateBlock( DWORD Token)
{
	DEBUG_TRACE2( "CDirect3DDevice8::DeleteStateBlock( DWORD Token=%d);\n", Token );
	if(Token) 
	{
		std::vector<DWORD>::iterator pos = std::find(m_SBHandle.begin(), m_SBHandle.end(), Token);
		if(pos != m_SBHandle.end())
		{
			m_SBHandle.erase(pos);
			IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)Token;
			NSFINALRELEASE(pSB);
		}
		return S_OK;
	}
	else 
		return D3DERR_INVALIDCALL;
}

STDMETHODIMP CDirect3DDevice8::GetTexture( DWORD Stage,IDirect3DBaseTexture8** ppTexture)
{
	HRESULT hr = S_OK;
	if(ppTexture)
	{
		CComPtr<IDirect3DBaseTexture9> pTexture;
		hr = m_pReal->GetTexture(Stage, &pTexture);
		if(SUCCEEDED(hr))
		{
			if(pTexture)
				Wrap(pTexture.p, ppTexture);
			else 
				*ppTexture=NULL;
		}
		else
			DEBUG_FAILED(hr);
	}
	else 
		return D3DERR_INVALIDCALL;
	DEBUG_TRACE3( "CDirect3DDevice8::GetTexture(Stage=%d, IDirect3DBaseTexture8** ppTexture=%p)=%s;\n", Stage, ppTexture, GetResultString( hr ) );
	return hr;
}

typedef enum _D3DTEXTURESTAGESTATETYPE8
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
} D3DTEXTURESTAGESTATETYPE8;

STDMETHODIMP CDirect3DDevice8::GetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
	DEBUG_TRACE3( "CDirect3DDevice8::GetTextureStageState(Stage=%d, Type=%d, DWORD* pValue=%p);\n", Stage, Type, pValue);
	switch(Type)
	{
	case D3DTSS_ADDRESSU: 
		return m_pReal->GetSamplerState(Stage,D3DSAMP_ADDRESSU,pValue);
	case D3DTSS_ADDRESSV:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_ADDRESSV,pValue);
	case D3DTSS_ADDRESSW:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_ADDRESSW,pValue);
	case D3DTSS_BORDERCOLOR:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_BORDERCOLOR,pValue);
	case D3DTSS_MAGFILTER:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MAGFILTER,pValue);
	case D3DTSS_MINFILTER:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MINFILTER,pValue);
	case D3DTSS_MIPFILTER:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MIPFILTER,pValue);
	case D3DTSS_MIPMAPLODBIAS:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MIPMAPLODBIAS,pValue);
	case D3DTSS_MAXMIPLEVEL:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MAXMIPLEVEL,pValue);
	case D3DTSS_MAXANISOTROPY:
		return m_pReal->GetSamplerState(Stage,D3DSAMP_MAXANISOTROPY,pValue);
	default:
		return m_pReal->GetTextureStageState(Stage,Type,pValue);
	}
}

STDMETHODIMP CDirect3DDevice8::SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	DEBUG_TRACE3( "CDirect3DDevice8::SetTextureStageState(Stage=%d, Type=%d, DWORD Value=%p);\n", Stage, Type, Value);
	switch(Type)
	{
	case D3DTSS_ADDRESSU: 
		return m_pReal->SetSamplerState(Stage,D3DSAMP_ADDRESSU,Value);
		break;
	case D3DTSS_ADDRESSV:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_ADDRESSV,Value);
		break;
	case D3DTSS_ADDRESSW:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_ADDRESSW,Value);
		break;
	case D3DTSS_BORDERCOLOR:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_BORDERCOLOR,Value);
		break;
	case D3DTSS_MAGFILTER:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MAGFILTER,Value);
		break;
	case D3DTSS_MINFILTER:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MINFILTER,Value);
		break;
	case D3DTSS_MIPFILTER:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MIPFILTER,Value);
		break;
	case D3DTSS_MIPMAPLODBIAS:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MIPMAPLODBIAS,Value);
		break;
	case D3DTSS_MAXMIPLEVEL:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MAXMIPLEVEL,Value);
		break;
	case D3DTSS_MAXANISOTROPY:
		return m_pReal->SetSamplerState(Stage,D3DSAMP_MAXANISOTROPY,Value);
		break;
	default:
		return m_pReal->SetTextureStageState(Stage,Type,Value);
		break;
	}
}

STDMETHODIMP CDirect3DDevice8::GetInfo( DWORD DevInfoID,void* pDevInfoStruct,DWORD DevInfoStructSize)
{
	HRESULT hr;
	CComPtr<IDirect3DQuery9> pQuery;
	switch(DevInfoID)
	{
	case 1: // D3DDEVINFOID_TEXTUREMANAGER
	case 2: // D3DDEVINFOID_D3DTEXTUREMANAGER
	case 3: // D3DDEVINFOID_TEXTURING
		DEBUG_MESSAGE("CDirect3DDevice8::GetInfo() not implemented\n");
		return E_FAIL;
	case 4: // D3DDEVINFOID_VCACHE
		if (FAILED(m_pReal->CreateQuery(D3DQUERYTYPE_VCACHE, &pQuery)))
			return E_FAIL;
		hr = pQuery->Issue(D3DISSUE_END);
		hr = pQuery->GetData(pDevInfoStruct, DevInfoStructSize, 0);
		return hr;
	case 5: // D3DDEVINFOID_RESOURCEMANAGER
		if (FAILED(m_pReal->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &pQuery)))
			return E_FAIL;
		hr = pQuery->Issue(D3DISSUE_END);
		hr = pQuery->GetData(pDevInfoStruct, DevInfoStructSize, 0);
		return hr;
	case 6: // D3DDEVINFOID_D3DVERTEXSTATS
		if (FAILED(m_pReal->CreateQuery(D3DQUERYTYPE_VERTEXSTATS, &pQuery)))
			return E_FAIL;
		hr = pQuery->Issue(D3DISSUE_END);
		hr = pQuery->GetData(pDevInfoStruct, DevInfoStructSize, 0);
		return hr;
	default: // D3DDEVINFOID_UNKNOWN
		return D3DERR_INVALIDCALL;
	}
	DEBUG_TRACE3( "CDirect3DDevice8::GetInfo(DWORD DevInfoID=%d,void* pDevInfoStruct=%p,DWORD DevInfoStructSize=%d)=%s;\n", DevInfoID, pDevInfoStruct, DevInfoStructSize, GetResultString( hr ) );
}

extern DWORD shaderGlobalIndex;

STDMETHODIMP CDirect3DDevice8::CreateVertexShader( CONST DWORD* pDeclaration, CONST DWORD* pFunction,DWORD* pHandle,DWORD Usage)
{
	DEBUG_TRACE1( "CDirect3DDevice8::CreateVertexShader(pDeclaration=%p, pFunction=%p, pHandle=%p, Usage=%d)\n",
		pDeclaration, pFunction, pHandle, Usage);
	DEBUG_TRACE1("Vertex Shader #%d\n", shaderGlobalIndex);
	*pHandle  = 0;
	DWORD declSize;
	VSDeclData declData;
	_ASSERT(pDeclaration);
#ifndef FINAL_RELEASE
	char buffer[65536];
#else
	char* buffer = NULL;
#endif
	HRESULT hr = ConvertVSDecl( pDeclaration, declSize, declData, buffer);
	CComPtr<IDirect3DVertexDeclaration9> pDecl;
	hr = m_pReal->CreateVertexDeclaration(declData.pVertexElements, &pDecl);
	if(FAILED(hr))
	{
		DEBUG_MESSAGE( "\tCreateVertexDeclaration() failed\n");
		return hr;
	}

	CDirect3DVertexShader8* pVS;
	CreateWrapper(pDecl.p, (IWrapper**)&pVS);
	*pHandle = (DWORD_PTR)pVS;
	m_VSHandle.push_back(*pHandle);
	pVS->SetVD8(pDeclaration, declSize);

	if(pFunction)
	{
		sstream = DNew char[MAXSHADERSIZE];
		CComPtr<ID3DXBuffer> pDisassembledShader;  
		LONG vdtextsz = 0;
		CComPtr<IDirect3DVertexShader9> pVS9;
		if (SUCCEEDED(D3DXDisassembleShader(pFunction, 0, 0, &pDisassembledShader)))
		{
			char ver[] = id"vs_1_1"nl; // Maximum VS version for D3D8
			strcpy_s(sstream, MAXSHADERSIZE, ver); // pos - inputShaderText
			textsz = strlen(sstream);
			textsz += GetVSDecl(declData, sstream + textsz);
			vdtextsz = textsz;
			// TODO: Combine inputShaderText with sstream
			char* inputShaderText = (char*)pDisassembledShader->GetBufferPointer();
			char* pos = strchr(inputShaderText, '\n') + 1;
			strcpy_s(sstream + textsz, MAXSHADERSIZE - textsz, pos);
			//DissasembleVS(pFunction, d);
			hr = CompileVertexShader(&pVS9, sstream, m_pReal, buffer);
#ifndef FINAL_RELEASE
			if(GINFO_DEBUG) 
			{
				TCHAR shader_file[ MAX_PATH ];
				_tcscpy( shader_file, gInfo.LogFileName );
				_tcscpy( &shader_file[ _tcslen( shader_file ) - 4 ], _T("_shaders.d3d8.txt") );
				WriteShaderTextToFile(shader_file, inputShaderText);
			}
#endif
		}
		else
			DEBUG_MESSAGE("Disassemble shader error\n");
		if (FAILED(hr))
		{
			DEBUG_MESSAGE("Compile shader #%d error\n", shaderGlobalIndex);

			textsz = vdtextsz;
			ParseVertexFunction((DWORD*)pFunction);

			hr = CompileVertexShader(&pVS9, sstream, m_pReal);
		}
		if (FAILED(hr))
		{
			hr = S_OK; // HACK
			DEBUG_MESSAGE("Compile2 shader error\n");
		}
		else
			DEBUG_MESSAGE("Compile shader OK\n");
		pVS->SetVS(pVS9);
		delete [] sstream;
	}
	delete [] declData.pVertexElements;
	shaderGlobalIndex++;
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetVertexShader( DWORD Handle)
{
	DEBUG_TRACE3( "CDirect3DDevice8::SetVertexShader(Handle = %p);\n", Handle );
	HRESULT hr = S_OK;

	std::vector<DWORD>::iterator pos = std::find(m_VSHandle.begin(), m_VSHandle.end(), Handle);
	if(pos == m_VSHandle.end())
	{
		DEBUG_TRACE3( "\tSetFVF\n" );
		hr = m_pReal->SetVertexShader( NULL );
		hr = m_pReal->SetFVF(Handle);
	}
	else
	{
		CDirect3DVertexShader8* pVS = (CDirect3DVertexShader8*)Handle;
		hr = m_pReal->SetVertexDeclaration(pVS->GetVD());
		DEBUG_TRACE3( "\tSetVertexDeclaration\n" );
		if (FAILED(hr))
			DEBUG_MESSAGE("IDirect3DDevice9::SetVertexDeclaration(): failed\n");
		DEBUG_TRACE3( "\tSetVertexShader\n" );
		hr = m_pReal->SetVertexShader(pVS->GetVS());
		if (FAILED(hr))
			DEBUG_MESSAGE("IDirect3DDevice9::SetVertexShader(): failed\n");
	}

	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetVertexShader( DWORD* pHandle)
{
	CComPtr<IDirect3DVertexDeclaration9> pVD;
	HRESULT hr = m_pReal->GetVertexDeclaration(&pVD);
	CComPtr<CDirect3DVertexShader8> pVS;
	Wrap(pVD.p, (IWrapper**)&pVS);
	*pHandle = (DWORD_PTR)pVS.p;
	DEBUG_TRACE3( "CDirect3DDevice8::GetVertexShader( *pHandle=%p)=%d;\n", pHandle ? *pHandle : 0, GetResultString( hr) );
	if (FAILED(hr))
	{
		DEBUG_MESSAGE("CDirect3DDevice8::GetVertexShader() failed\nhr = %d\n", hr);
	}
	return S_OK;
}

STDMETHODIMP CDirect3DDevice8::DeleteVertexShader(DWORD Handle)
{
	DEBUG_TRACE2( "CDirect3DDevice8::DeleteVertexShader( Handle=%p);\n", Handle );
	if (Handle)
	{
		std::vector<DWORD>::iterator pos = std::find(m_VSHandle.begin(), m_VSHandle.end(), Handle);
		if(pos != m_VSHandle.end())
		{
			m_VSHandle.erase(pos);
			CDirect3DVertexShader8* pVS = (CDirect3DVertexShader8*)Handle;
			NSFINALRELEASE(pVS);
		}
	}
	return S_OK;
}

STDMETHODIMP CDirect3DDevice8::GetVertexShaderFunction( DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	CDirect3DVertexShader8* pVS = (CDirect3DVertexShader8*)Handle;
	IDirect3DVertexShader9* pShader = pVS->GetVS();
	DEBUG_TRACE3( "CDirect3DDevice8::GetVertexShaderFunction(Handle = % d, void * pData = % p, pSizeOfData = % d);\n", 
		Handle, pData, pSizeOfData);
	if(pShader == NULL) 
		return D3DERR_INVALIDCALL;
	UINT sizeF = *pSizeOfData;
	HRESULT hr = pShader->GetFunction(pData, &sizeF);
	*pSizeOfData = sizeF;
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DeletePixelShader( DWORD Handle)
{
	DEBUG_TRACE2( "CDirect3DDevice8::DeletePixelShader(DWORD Handle = % p);\n", Handle );
	if (Handle)
	{
		std::vector<DWORD>::iterator pos = std::find(m_PSHandle.begin(), m_PSHandle.end(), Handle);
		if(pos != m_PSHandle.end())
		{
			m_PSHandle.erase(pos);
			IDirect3DPixelShader9* pPS = (IDirect3DPixelShader9*)Handle;
			NSFINALRELEASE(pPS);
		}
	}
	return S_OK;
}

STDMETHODIMP CDirect3DDevice8::GetPixelShaderFunction( DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	IDirect3DPixelShader9* pShader = (IDirect3DPixelShader9*)Handle;
	UINT szData = *pSizeOfData;
	HRESULT hr = pShader->GetFunction(pData, &szData);
	*pSizeOfData = szData;
	DEBUG_TRACE3( "CDirect3DDevice8::GetPixelShaderFunction(Handle = % d, void * pData = % p, pSizeOfData = % d) = % s;\n",
		Handle, pData, pSizeOfData, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	*pSizeOfData = szData;
	return hr;
}

STDMETHODIMP CDirect3DDevice8::TestCooperativeLevel()
{
	HRESULT hr = S_OK;
	hr = m_pReal->TestCooperativeLevel();
	DEBUG_TRACE3("CDirect3DDevice8::TestCooperativeLevel()=%s \n", GetResultString(hr));
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP_(UINT) CDirect3DDevice8::GetAvailableTextureMem()
{
	UINT hr;
	hr = m_pReal->GetAvailableTextureMem();
	DEBUG_TRACE3("CDirect3DDevice8::GetAvailableTextureMem()=%d \n", hr);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::ResourceManagerDiscardBytes(DWORD Bytes)
{
	HRESULT hr = S_OK;
	hr = m_pReal->EvictManagedResources();
	DEBUG_TRACE3("CDirect3DDevice8::ResourceManagerDiscardBytes(Bytes)=%s, Bytes = %d\n", GetResultString(hr), Bytes);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetDirect3D( IDirect3D8** ppD3D8)
{
	HRESULT hr = S_OK;
	m_pDirect3D->QueryInterface(IID_IDirect3D8, (void**)ppD3D8);
	if(!*ppD3D8)
		return D3DERR_INVALIDCALL;
	DEBUG_TRACE3("CDirect3DDevice8::GetDirect3D(ppD3D8)=%s, ppD3D8=%p \n", GetResultString(hr), ppD3D8);
	if(FAILED(hr))
		DEBUG_FAILED(hr);;
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetDeviceCaps( D3DCAPS8* pCaps)
{
	HRESULT hr = S_OK;
	D3DCAPS9 caps;
	hr=m_pReal->GetDeviceCaps(&caps);
	DEBUG_TRACE3("CDirect3DDevice8::GetDeviceCaps( D3DCAPS8* pCaps=%p)=%s \n", pCaps, GetResultString(hr));
	if(SUCCEEDED(hr))
		ConvertCaps(caps, pCaps);
	else
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetDisplayMode( D3DDISPLAYMODE* pMode)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetDisplayMode(0,pMode);
	if(pMode)
		DEBUG_TRACE3("CDirect3DDevice8::GetDisplayMode(0,pMode)=%s, pMode->Format=%s,  pMode->Height=%d, pMode->Width=%d, pMode->RefreshRate=%d\n", 
		GetResultString(hr), GetFormatString(pMode->Format), pMode->Height, pMode->Width, pMode->RefreshRate);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetCreationParameters( D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetCreationParameters(pParameters);
	if(pParameters)
		DEBUG_TRACE3("CDirect3DDevice8::GetCreationParameters(pParameters)=%s, pParameters->AdapterOrdinal=%d, pParameters->BehaviorFlags=%d, pParameters->DeviceType=%d, pParameters->hFocusWindow=%p\n", 
		GetResultString(hr), pParameters->AdapterOrdinal, pParameters->BehaviorFlags, pParameters->DeviceType, pParameters->hFocusWindow);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetCursorProperties( UINT XHotSpot,UINT YHotSpot,IDirect3DSurface8* pCursorBitmap)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetCursorProperties(XHotSpot,YHotSpot,Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pCursorBitmap));
	DEBUG_TRACE3("CDirect3DDevice8::SetCursorProperties( UINT XHotSpot,UINT YHotSpot,IDirect3DSurface8* pCursorBitmap)=%s, XHotSpot=%d, YHotSpot=%d, pCursorBitmap=%p\n", GetResultString(hr), XHotSpot, YHotSpot, pCursorBitmap);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP_(void) CDirect3DDevice8::SetCursorPosition( int X,int Y,DWORD Flags)
{
	m_pReal->SetCursorPosition(X,Y,Flags);
	DEBUG_TRACE3("CDirect3DDevice8::SetCursorPosition( int X,int Y,DWORD Flags), X=%d, Y%d, Flags=%d\n", X, Y, Flags);
}

STDMETHODIMP_(BOOL) CDirect3DDevice8::ShowCursor( BOOL bShow)
{
	HRESULT hr = m_pReal->ShowCursor(bShow);
	if (FAILED(hr))
	{
		DEBUG_FAILED(hr);
		DEBUG_TRACE3("CDirect3DDevice8::ShowCursor( BOOL bShow)=FALSE, bShow=%d\n", bShow);
	}
	else
		DEBUG_TRACE3("CDirect3DDevice8::ShowCursor( BOOL bShow)=TRUE, bShow=%d\n", bShow);
	return FALSE;
}

STDMETHODIMP CDirect3DDevice8::GetBackBuffer( UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface8** ppBackBuffer)
{
	HRESULT hr;
	CComPtr<IDirect3DSurface9> pBBuff;
	hr = m_pReal->GetBackBuffer( 0, BackBuffer, Type, &pBBuff);
	if(SUCCEEDED(hr))
		Wrap(pBBuff.p, ppBackBuffer);
	else
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetRasterStatus( D3DRASTER_STATUS* pRasterStatus)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetRasterStatus(0,pRasterStatus);
	DEBUG_TRACE3("CDirect3DDevice8::GetRasterStatus( D3DRASTER_STATUS* pRasterStatus)=%s, pRasterStatus->InVBlank=%d, pRasterStatus->ScanLine=%d \n", 
		GetResultString(hr), pRasterStatus->InVBlank, pRasterStatus->ScanLine);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP_(void) CDirect3DDevice8::SetGammaRamp( DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
	m_pReal->SetGammaRamp(0,Flags,pRamp);
	DEBUG_TRACE3("CDirect3DDevice8::SetGammaRamp( DWORD Flags,CONST D3DGAMMARAMP* pRamp), Flags=%d, pRamp=%p\n", Flags, pRamp);
}

STDMETHODIMP_(void) CDirect3DDevice8::GetGammaRamp( D3DGAMMARAMP* pRamp)
{
	m_pReal->GetGammaRamp(0,pRamp);
	DEBUG_TRACE3("CDirect3DDevice8::SetGammaRamp( D3DGAMMARAMP* pRamp), pRamp=%p\n", pRamp);
}

STDMETHODIMP CDirect3DDevice8::UpdateTexture( IDirect3DBaseTexture8* pSourceTexture,IDirect3DBaseTexture8* pDestinationTexture)
{
	HRESULT hr = S_OK;
	hr = m_pReal->UpdateTexture(Unwrap<IDirect3DBaseTexture9, CDirect3DBaseTexture8<IDirect3DBaseTexture8, IDirect3DBaseTexture9> >(pSourceTexture), 
		Unwrap<IDirect3DBaseTexture9, CDirect3DBaseTexture8<IDirect3DBaseTexture8, IDirect3DBaseTexture9> >(pDestinationTexture));
	DEBUG_TRACE3( "CDirect3DDevice8::UpdateTexture(IDirect3DBaseTexture8* pSourceTexture=%p, IDirect3DBaseTexture8* pDestinationTexture=%p)=%s;\n", pSourceTexture, pDestinationTexture, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetFrontBuffer( IDirect3DSurface8* pDestSurface)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetFrontBufferData(0,  Unwrap<IDirect3DSurface9, CDirect3DSurface8>(pDestSurface));
	DEBUG_TRACE3( "CDirect3DDevice8::GetFrontBuffer(IDirect3DSurface8* pDestSurface=%p)=%s;\n", pDestSurface, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::BeginScene()
{
	HRESULT hr = S_OK;
	hr = m_pReal->BeginScene();
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::EndScene()
{
	HRESULT hr = S_OK;
	hr = m_pReal->EndScene();
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::Clear( DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	HRESULT hr = S_OK;
	hr = m_pReal->Clear(Count,pRects,Flags,Color,Z,Stencil);
	DEBUG_TRACE3( "CDirect3DDevice8::Clear( %ld, %p, %s%s%s, #%08X, %12.7f, %ld)\n", Count, pRects,
		( Flags & D3DCLEAR_TARGET ) ? "TARGET" : "0",
		( Flags & D3DCLEAR_ZBUFFER ) ? "|ZBUFFER" : "",
		( Flags & D3DCLEAR_STENCIL ) ? "|STENCIL" : "",
		Color, Z, Stencil);
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetTransform(State, pMatrix);
	DEBUG_TRACE3( "CDirect3DDevice8::SetTransform( D3DTRANSFORMSTATETYPE %d, D3DMATRIX* pMatrix=%p) = %s;\n", State, pMatrix, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetTransform(State,pMatrix);
	DEBUG_TRACE3( "CDirect3DDevice8::GetTransform( D3DTRANSFORMSTATETYPE %d, D3DMATRIX* pMatrix=%p) = %s;\n", State, pMatrix, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::MultiplyTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	HRESULT hr = S_OK;
	hr = m_pReal->MultiplyTransform(State, pMatrix);
	DEBUG_TRACE3( "CDirect3DDevice8::MultiplyTransform( D3DTRANSFORMSTATETYPE %d, D3DMATRIX* pMatrix=%p) = %s;\n", 
		State, pMatrix, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetViewport( CONST D3DVIEWPORT8* pViewport)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetViewport(reinterpret_cast<const D3DVIEWPORT9*>(pViewport));
	DEBUG_TRACE3( "CDirect3DDevice8::SetViewport(%p) = %s;\n", pViewport, GetResultString( hr ) );
	if ( pViewport )
		DEBUG_TRACE3( "\t// *pViewport={%d,%d,%d,%d,%8.3f,%8.3f}\n",
		pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height,
		pViewport->MinZ, pViewport->MaxZ );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetViewport( D3DVIEWPORT8* pViewport)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetViewport(reinterpret_cast<D3DVIEWPORT9*>(pViewport));
	if(pViewport)
		DEBUG_TRACE3( "CDirect3DDevice8::GetViewport(%p={%d,%d,%d,%d,%8.3f,%8.3f}) = %s;\n", pViewport,
		pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height,
		pViewport->MinZ, pViewport->MaxZ, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetMaterial( CONST D3DMATERIAL8* pMaterial)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetMaterial(reinterpret_cast<const D3DMATERIAL9*>(pMaterial));
	DEBUG_TRACE3( "CDirect3DDevice8::SetMaterial(CONST D3DMATERIAL8* pMaterial=%p)=%s;\n", pMaterial, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetMaterial( D3DMATERIAL8* pMaterial)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetMaterial(reinterpret_cast<D3DMATERIAL9*>(pMaterial));
	DEBUG_TRACE3( "CDirect3DDevice8::GetMaterial(CONST D3DMATERIAL8* pMaterial=%p)=%s;\n", pMaterial, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetLight( DWORD Index,CONST D3DLIGHT8* pLight)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetLight(Index, reinterpret_cast<const D3DLIGHT9*>(pLight));
	DEBUG_TRACE3( "CDirect3DDevice8::SetLight(Index=%08X, CONST D3DLIGHT8* pLight=%p)=%s;\n", Index, pLight, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetLight( DWORD Index,D3DLIGHT8* pLight)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetLight(Index, reinterpret_cast<D3DLIGHT9*>(pLight));
	DEBUG_TRACE3( "CDirect3DDevice8::GetLight(Index=%08X, CONST D3DLIGHT8* pLight=%p)=%s;\n", Index, pLight, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::LightEnable( DWORD Index,BOOL Enable)
{
	HRESULT hr = S_OK;
	hr = m_pReal->LightEnable(Index, Enable);
	DEBUG_TRACE3( "CDirect3DDevice8::LightEnable(DWORD Index=%d,BOOL Enable=%d)=%s;\n", Index, Enable, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetLightEnable( DWORD Index,BOOL* pEnable)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetLightEnable(Index,pEnable);
	DEBUG_TRACE3( "CDirect3DDevice8::GetLightEnable(DWORD Index=%d,BOOL* pEnable=%d)=%s;\n", Index, pEnable, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetClipPlane( DWORD Index,CONST float* pPlane)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetClipPlane(Index,pPlane);
	DEBUG_TRACE3( "CDirect3DDevice8::SetClipPlane(Index=%08X, CONST float* pPlane=%p)=%s;\n", Index, pPlane, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetClipPlane( DWORD Index,float* pPlane)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetClipPlane(Index, pPlane);
	DEBUG_TRACE3( "CDirect3DDevice8::GetClipPlane(Index=%08X, float* pPlane=%p)=%s;\n", Index, pPlane, GetResultString(hr) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::BeginStateBlock()
{
	HRESULT hr = S_OK;
	hr = m_pReal->BeginStateBlock();
	DEBUG_TRACE3( "CDirect3DDevice8::BeginStateBlock()=%s;\n", GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::ApplyStateBlock( DWORD Token)
{
	HRESULT hr = S_OK;
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)Token;
	if(pSB) 
		hr = pSB->Apply();
	else
		DEBUG_MESSAGE("CDirect3DDevice8::ApplyStateBlock pSB = NULL\n");
	DEBUG_TRACE3( "CDirect3DDevice8::ApplyStateBlock( DWORD Token=%d)=%s;\n", Token, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CaptureStateBlock( DWORD Token)
{
	HRESULT hr = S_OK;
	IDirect3DStateBlock9 *pSB = (IDirect3DStateBlock9*)Token;
	if(pSB) 
		hr = pSB->Capture();
	else
		DEBUG_MESSAGE("CDirect3DDevice8::CaptureStateBlock pSB = NULL\n");
	DEBUG_TRACE3( "CDirect3DDevice8::CaptureStateBlock( DWORD Token=%d)=%s;\n", Token, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetClipStatus( CONST D3DCLIPSTATUS8* pClipStatus)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetClipStatus(reinterpret_cast<const D3DCLIPSTATUS9*>(pClipStatus));
	DEBUG_TRACE3( "CDirect3DDevice8::SetClipStatus(CONST D3DCLIPSTATUS8* pClipStatus=%p)=%s;\n", pClipStatus, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetClipStatus( D3DCLIPSTATUS8* pClipStatus)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetClipStatus(reinterpret_cast<D3DCLIPSTATUS9*>(pClipStatus));
	DEBUG_TRACE3( "CDirect3DDevice8::GetClipStatus(CONST D3DCLIPSTATUS8* pClipStatus=%p)=%s;\n", pClipStatus, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetTexture( DWORD Stage,IDirect3DBaseTexture8* pTexture)
{
	HRESULT hr = S_OK;
	hr=m_pReal->SetTexture(Stage, Unwrap<IDirect3DBaseTexture9, CDirect3DBaseTexture8<IDirect3DBaseTexture8, IDirect3DBaseTexture9> >(pTexture));
	DEBUG_TRACE3( "CDirect3DDevice8::SetTexture(Stage=%d, IDirect3DBaseTexture8* pTexture=%p)=%s;\n", Stage, pTexture, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::ValidateDevice( DWORD* pNumPasses)
{
	HRESULT hr = S_OK;
	hr = m_pReal->ValidateDevice(pNumPasses);
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	DEBUG_TRACE3( "CDirect3DDevice8::ValidateDevice(DWORD* pNumPasses=%p)=%s;\n", pNumPasses, GetResultString( hr ) );
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetPaletteEntries( UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetPaletteEntries( PaletteNumber, pEntries);
	DEBUG_TRACE3( "CDirect3DDevice8::SetPaletteEntries(PaletteNumber=%d, CONST PALETTEENTRY* pEntries=%p)=%s;\n", PaletteNumber, pEntries, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetPaletteEntries( UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetPaletteEntries( PaletteNumber, pEntries);
	DEBUG_TRACE3( "CDirect3DDevice8::GetPaletteEntries(PaletteNumber=%d, PALETTEENTRY* pEntries=%p)=%s;\n", PaletteNumber, pEntries, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetCurrentTexturePalette( UINT PaletteNumber)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetCurrentTexturePalette( PaletteNumber);
	DEBUG_TRACE3( "CDirect3DDevice8::SetCurrentTexturePalette(PaletteNumber=%d)=%s;\n", PaletteNumber, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetCurrentTexturePalette( UINT *PaletteNumber)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetCurrentTexturePalette( PaletteNumber);
	DEBUG_TRACE3( "CDirect3DDevice8::GetCurrentTexturePalette(PaletteNumber=%d)=%s;\n", PaletteNumber, GetResultString( hr ) );
	if(FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawPrimitive( PrimitiveType, StartVertex, PrimitiveCount);
	DEBUG_TRACE3( "CDirect3DDevice8::DrawPrimitive( %s, %u, %u)=%s;\n", GetPrimitiveTypeString(PrimitiveType), StartVertex, PrimitiveCount, GetResultString( hr ));
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawIndexedPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT minIndex,UINT NumVertices,UINT startIndex,UINT primCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawIndexedPrimitive( PrimitiveType, m_BaseVertexIndex, minIndex, NumVertices, startIndex, primCount);
	DEBUG_TRACE3("CDirect3DDevice8::DrawIndexedPrimitive(%s, MinVertexIndex=%u, NumVertices=%u, startIndex=%u, primCount=%u)=%s;\n",
		GetPrimitiveTypeString( PrimitiveType ), minIndex, NumVertices, startIndex, primCount, GetResultString( hr ));
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawPrimitiveUP( PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	DEBUG_TRACE3("CDirect3DDevice8::DrawPrimitiveUP(%s, PrimitiveCount=%u, pVertexStreamZeroData=%p, VertexStreamZeroStride=%u)=%s;\n",
		GetPrimitiveTypeString( PrimitiveType ), PrimitiveCount,
		pVertexStreamZeroData, VertexStreamZeroStride, GetResultString( hr ));
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawIndexedPrimitiveUP( PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	DEBUG_TRACE3( "CDirect3DDevice8::DrawIndexedPrimitiveUP(%s, %u, %u, %u, %p, D3DFORMAT = %d, %p, %u)=%s;\n",
		GetPrimitiveTypeString( PrimitiveType ), MinVertexIndex, NumVertexIndices, PrimitiveCount,
		pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride, GetResultString( hr ));
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::ProcessVertices( UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer8* pDestBuffer,DWORD Flags)
{
	if(pDestBuffer==NULL) 
		return D3DERR_INVALIDCALL;
	HRESULT hr = m_pReal->ProcessVertices( SrcStartIndex, DestIndex, VertexCount, Unwrap<IDirect3DVertexBuffer9, CDirect3DVertexBuffer8>(pDestBuffer), NULL, Flags);
	DEBUG_TRACE3( "CDirect3DDevice8::ProcessVertices(SrcStartIndex=%d, DestIndex=%d, VertexCount=%d, IDirect3DVertexBuffer8* pDestBuffer=%p, Flags=%08X)=%s;\n", 
		SrcStartIndex, DestIndex, VertexCount, pDestBuffer, Flags, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
} 

STDMETHODIMP CDirect3DDevice8::SetVertexShaderConstant( DWORD Register,CONST void* pConstantData,DWORD ConstantCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetVertexShaderConstantF( Register, (float*)pConstantData, ConstantCount);
	DEBUG_TRACE3( "CDirect3DDevice8::SetVertexShaderConstant(Register = % d, CONST void * pConstantData = % p, ConstantCount = % d) = % s;\n", 
		Register, pConstantData, ConstantCount, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetVertexShaderConstant( DWORD Register,void* pConstantData,DWORD ConstantCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetVertexShaderConstantF( Register, (float*)pConstantData, ConstantCount);
	DEBUG_TRACE3( "CDirect3DDevice8::GetVertexShaderConstant(Register = % d, void * pConstantData = % p, ConstantCount = % d) = % s;\n", 
		Register, pConstantData, ConstantCount, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetVertexShaderDeclaration( DWORD Handle,void* pData,DWORD* pSizeOfData)
{
	DEBUG_TRACE3( "CDirect3DDevice8::GetVertexShaderDeclaration(Handle = % d, void * pData = % p, pSizeOfData = % d);\n", 
		Handle, pData, pSizeOfData);
	CDirect3DVertexShader8* pVS = (CDirect3DVertexShader8*)Handle;
	pVS->GetVD8(pData, pSizeOfData);
	return S_OK;
}

STDMETHODIMP CDirect3DDevice8::SetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer8* pStreamData,UINT Stride)
{
	HRESULT hr = m_pReal->SetStreamSource( StreamNumber, Unwrap<IDirect3DVertexBuffer9, CDirect3DVertexBuffer8>(pStreamData), 0,  Stride);
	DEBUG_TRACE3( "CDirect3DDevice8::SetStreamSource(StreamNumber=%d, IDirect3DVertexBuffer8* pStreamData=%p, Stride=%d)=%s;\n", 
		StreamNumber, pStreamData, Stride, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;	
}

STDMETHODIMP CDirect3DDevice8::GetStreamSource( UINT StreamNumber,IDirect3DVertexBuffer8** ppStreamData,UINT* pStride)
{
	HRESULT hr = S_OK;
	UINT Offset = 0;
	DEBUG_TRACE3( "CDirect3DDevice8::GetStreamSource(StreamNumber = % d, IDirect3DVertexBuffer8* *ppStreamData = % p, UINT * pStride = % p);\n", 
		StreamNumber, ppStreamData ? *ppStreamData : 0, pStride, GetResultString( hr ) );
	CComPtr<IDirect3DVertexBuffer9> pBBuff;
	hr = m_pReal->GetStreamSource(StreamNumber, &pBBuff, &Offset, pStride);
	if (SUCCEEDED(hr))
		Wrap(pBBuff.p, ppStreamData);
	else
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetIndices( IDirect3DIndexBuffer8* pIndexData,UINT BaseVertexIndex)
{
	HRESULT hr = m_pReal->SetIndices(Unwrap<IDirect3DIndexBuffer9, CDirect3DIndexBuffer8>(pIndexData));
	m_BaseVertexIndex = BaseVertexIndex;
	DEBUG_TRACE3( "CDirect3DDevice8::SetIndices(IDirect3DIndexBuffer8* pIndexData=%p, BaseVertexIndex=%d)=%s;\n", 
		pIndexData, BaseVertexIndex, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetIndices( IDirect3DIndexBuffer8** ppIndexData,UINT* pBaseVertexIndex)
{
	HRESULT hr = S_OK;
	CComPtr<IDirect3DIndexBuffer9> pBBuff;
	hr = m_pReal->GetIndices(&pBBuff);
	DEBUG_TRACE3( "CDirect3DDevice8::GetIndices(IDirect3DIndexBuffer8* *ppIndexData = % p, UINT * pBaseVertexIndex = % p) = % s;\n", 
		ppIndexData ? *ppIndexData : 0, pBaseVertexIndex, GetResultString( hr ) );
	if(SUCCEEDED(hr))
	{
		Wrap(pBBuff.p, ppIndexData);
		*pBaseVertexIndex = m_BaseVertexIndex;
	}
	else
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::CreatePixelShader( CONST DWORD* pFunction,DWORD* pHandle)
{
	HRESULT hr = m_pReal->CreatePixelShader( pFunction,(IDirect3DPixelShader9**)pHandle);
	DEBUG_TRACE2( "CDirect3DDevice8::CreatePixelShader(CONST DWORD* pFunction = % p, DWORD* *pHandle = % p) = % s;\n", 
		pFunction, pHandle ? *pHandle : 0, GetResultString( hr ) );
	if (SUCCEEDED(hr))
		m_PSHandle.push_back(*pHandle);
	else
	{
		// Pariah: error X5539: (second source param) Modifiers are not allowed on constants for ps_1_x.
		DEBUG_FAILED(hr);
		return S_OK; // HACK
	}
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetPixelShader( DWORD Handle)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetPixelShader( (IDirect3DPixelShader9*)Handle);
	DEBUG_TRACE3( "CDirect3DDevice8::SetPixelShader(Handle = %p) = % s;\n", Handle, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetPixelShader( DWORD* pHandle)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetPixelShader( (IDirect3DPixelShader9**)pHandle);
	DEBUG_TRACE3( "CDirect3DDevice8::GetPixelShader(pHandle = %p) = % s;\n", pHandle, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::SetPixelShaderConstant( DWORD Register,CONST void* pConstantData,DWORD ConstantCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->SetPixelShaderConstantF( Register, (float*)pConstantData, ConstantCount);
	DEBUG_TRACE3( "CDirect3DDevice8::SetPixelShaderConstant(Register = % d, CONST void * pConstantData = % p, ConstantCount = % d) = % s;\n", 
		Register, pConstantData, ConstantCount, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::GetPixelShaderConstant( DWORD Register,void* pConstantData,DWORD ConstantCount)
{
	HRESULT hr = S_OK;
	hr = m_pReal->GetPixelShaderConstantF( Register, (float*)pConstantData, ConstantCount);
	DEBUG_TRACE3( "CDirect3DDevice8::GetPixelShaderConstant(Register = % d, void * pConstantData = % p, ConstantCount = % d) = % s;\n", 
		Register, pConstantData, ConstantCount, GetResultString( hr ) );
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawRectPatch( UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawRectPatch( Handle, pNumSegs, pRectPatchInfo);
	DEBUG_TRACE3( "CDirect3DDevice8::DrawRectPatch( Handle=%u, pNumSegs=%p, pTriPatchInfo=%p);\n", 
		Handle, pNumSegs, pRectPatchInfo);
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DrawTriPatch( UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DrawTriPatch( Handle, pNumSegs, pTriPatchInfo);
	DEBUG_TRACE3( "CDirect3DDevice8::DrawTriPatch( Handle=%u, pNumSegs=%p, pTriPatchInfo=%p);\n", 
		Handle, pNumSegs, pTriPatchInfo);
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}

STDMETHODIMP CDirect3DDevice8::DeletePatch( UINT Handle)
{
	HRESULT hr = S_OK;
	hr = m_pReal->DeletePatch( Handle);
	DEBUG_TRACE3( "CDirect3DDevice8::DeletePatch( Handle=%u);\n", Handle);
	if (FAILED(hr))
		DEBUG_FAILED(hr);
	return hr;
}
