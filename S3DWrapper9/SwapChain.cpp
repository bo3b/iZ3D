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
#include "StdAfx.h"
#include "../S3DAPI/Utils.h"
#include "SwapChain.h"
#include "StereoRenderer.h"
#include "StereoRenderer_Hook.h"
#include "CommandDumper.h"

using namespace NonWide;

CSwapChain::CSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index)
: CBaseSwapChain(pDirect3DDevice9, index)
{
	m_BBOffsetBeforeScaling.cx = 0;
	m_BBOffsetBeforeScaling.cy = 0;
	m_BBOffset.cx = 0;
	m_BBOffset.cy = 0;
}

CSwapChain::~CSwapChain(void)
{

}

HRESULT CSwapChain::InitializeMode( D3DSURFACE_DESC &RenderTargetDesc )
{
	HRESULT hResult = S_OK;

	bool bAAMode = (m_MultiSampleType != D3DMULTISAMPLE_NONE);
	SIZE BBRightSize = m_BackBufferSizeBeforeScaling;
	if (bAAMode)
		GetRendD3D9Device()->ChangeSizeOfRightRTWithAA(BBRightSize.cx, BBRightSize.cy);
	if (bAAMode || m_bLockableBackBuffer || m_bScalingEnabled)
	{
		NSCALL_TRACE_RES(GetD3D9Device().CreateRenderTarget(m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy,
			RenderTargetDesc.Format, m_MultiSampleType, m_MultiSampleQuality, 
			m_bLockableBackBuffer, &m_pStereoBBLeft, 0), m_pStereoBBLeft, 
			DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Lockable = %1d, *ppSurface = %p, pSharedHandle = %p)"), 
			m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, GetFormatString(RenderTargetDesc.Format), 
			GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality, 
			m_bLockableBackBuffer, m_pStereoBBLeft, 0));
		NSCALL_TRACE_RES(GetD3D9Device().CreateRenderTarget(BBRightSize.cx, BBRightSize.cy,
			RenderTargetDesc.Format, m_MultiSampleType, m_MultiSampleQuality, 
			m_bLockableBackBuffer, &m_pStereoBBRight, 0), m_pStereoBBRight,
			DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Lockable = %1d, *ppSurface = %p, pSharedHandle = %p)"), 
			BBRightSize.cx, BBRightSize.cy, GetFormatString(RenderTargetDesc.Format), 
			GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality, 
			m_bLockableBackBuffer, m_pStereoBBRight, 0));
		m_pStereoBBRight->SetPrivateData(OriginalSurfaceSize_GUID, &m_BackBufferSizeBeforeScaling, sizeof(SIZE), 0);
		INC_DEBUG_COUNTER(GetBaseDevice()->m_nRenderTargetsStereoSize, GetSurfaceSize(m_pStereoBBLeft));
		INC_DEBUG_COUNTER(GetBaseDevice()->m_nRenderTargetsStereoSize, GetSurfaceSize(m_pStereoBBRight));
		if (DEBUGVER)
		{
			TCHAR buffer[128];
			_sntprintf_s(buffer, _TRUNCATE, _T("StereoBBLeft-SC%d"), m_Index);
			SetObjectName(m_pStereoBBLeft, buffer);
			_sntprintf_s(buffer, _TRUNCATE, _T("StereoBBRight-SC%d"), m_Index);
			SetObjectName(m_pStereoBBRight, buffer);
		}
	}
	else
	{
		NSCALL(m_pLeftMethodTexture->GetSurfaceLevel(0, &m_pStereoBBLeft));
		NSCALL(m_pRightMethodTexture->GetSurfaceLevel(0, &m_pStereoBBRight));
	}
	SetMcClaudFlag(m_pStereoBBLeft);
	m_pLeftBackBufferBeforeScaling = m_pStereoBBLeft;
	m_pRightBackBufferBeforeScaling = m_pStereoBBRight;
	SetStereoObject(m_pLeftBackBufferBeforeScaling, m_pRightBackBufferBeforeScaling);

	if(m_pPrimaryBackBuffer)
	{
		void** p = *(void***)m_pPrimaryBackBuffer.p;
		if( !IsCodeHookedByProxy(p[13], Proxy_DepthSurface_LockRect) )
			UniqueHookCode(GetBaseDevice(), p[13], Proxy_Surface_LockRect, (PVOID*)&Original_Surface_LockRect);
		if( !IsCodeHookedByProxy(p[14], Proxy_DepthSurface_UnlockRect) )
			UniqueHookCode(GetBaseDevice(), p[14], Proxy_Surface_UnlockRect, (PVOID*)&Original_Surface_UnlockRect);
	}

	SetRect(&m_LeftViewRectBeforeScaling, 0, 0, m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy);
	SetRect(&m_RightViewRectBeforeScaling, 0, 0, m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy);

	if(m_bScalingEnabled)
	{
		SetRect(&m_LeftViewRect, 0, 0, m_BackBufferSize.cx, m_BackBufferSize.cy);
		SetRect(&m_RightViewRect, 0, 0, m_BackBufferSize.cx, m_BackBufferSize.cy);
		TDisplayScalingMode mode = (TDisplayScalingMode)gInfo.DisplayScalingMode;
		ScaleRects(mode, &m_LeftViewRectBeforeScaling,  &m_LeftViewRect,  &m_SrcLeft,  &m_DstLeft);
		ScaleRects(mode, &m_RightViewRectBeforeScaling, &m_RightViewRect, &m_SrcRight, &m_DstRight);
		m_pLeftMethodTexture->GetSurfaceLevel(0, &m_pLeftBackBuffer);
		m_pRightMethodTexture->GetSurfaceLevel(0, &m_pRightBackBuffer);
		//--- keep a weak reference ---
		m_pLeftBackBuffer->Release();
		m_pRightBackBuffer->Release();
		m_BBOffset.cx = 0;
		m_BBOffset.cy = 0;
	}

	if (m_pPrimaryDepthStencil)
	{
		D3DSURFACE_DESC DepthStencilDesc;
		ZeroMemory( &DepthStencilDesc, sizeof(DepthStencilDesc) );
		NSCALL(m_pPrimaryDepthStencil->GetDesc( &DepthStencilDesc ));;
		if (!gInfo.MonoDepthStencilTextures && !bAAMode && 
			(GINFO_REPLACEDSSURFACES || (gInfo.ExtractDepthBuffer && (gInfo.ModeObtainingDepthBuffer == 0))))
		{
			CComPtr<IDirect3DTexture9> pTex;
			NSCALL(GetBaseDevice()->CreateTexture(m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, 1, D3DUSAGE_DEPTHSTENCIL, 
				GetBaseDevice()->GetDepthTextureFormat(DepthStencilDesc.Format), D3DPOOL_DEFAULT, &pTex, NULL )); // use internal method
			if (SUCCEEDED(hResult))
			{
				m_pPrimaryDepthStencil.Release();
				m_pPrimaryDepthStencil = m_pMainDepthStencilSurface;
				DEBUG_TRACE1(_T("\tReplaced DS surface by texture\n"));

				INC_DEBUG_COUNTER(GetBaseDevice()->m_nDepthStencilSurfacesStereoSize, GetSurfaceSize(m_pPrimaryDepthStencil));
			}
		} 
		else 
		{
			if (bAAMode)
			{
				m_pPrimaryDepthStencil.Release();
				NSCALL_TRACE_RES(GetD3D9Device().CreateDepthStencilSurface(
					m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, DepthStencilDesc.Format, 
					m_MultiSampleType, m_MultiSampleQuality, 
					FALSE, &m_pPrimaryDepthStencil, NULL), m_pPrimaryDepthStencil, 
					DEBUG_MESSAGE(_T("CreateDepthStencilSurface(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
					_T("MultisampleQuality = %08X, Discard = %1d, *ppSurface = %p, ")
					_T("pSharedHandle = %p)"), m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, 
					GetFormatString(DepthStencilDesc.Format), 
					GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality, FALSE, 
					m_pPrimaryDepthStencil, NULL));

				INC_DEBUG_COUNTER(GetBaseDevice()->m_nDepthStencilSurfacesStereoSize, GetSurfaceSize(m_pPrimaryDepthStencil));
			}

			void** p = *(void***)m_pPrimaryDepthStencil.p;
			if( !IsCodeHookedByProxy(p[13], Proxy_Surface_LockRect) )
				UniqueHookCode(GetBaseDevice(), p[13], Proxy_DepthSurface_LockRect, (PVOID*)&Original_DepthSurface_LockRect);
			if( !IsCodeHookedByProxy(p[14], Proxy_Surface_UnlockRect) )
				UniqueHookCode(GetBaseDevice(), p[14], Proxy_DepthSurface_UnlockRect, (PVOID*)&Original_DepthSurface_UnlockRect);
		}

		BBRightSize = m_BackBufferSizeBeforeScaling;
		if (gInfo.EnableAA)
			GetRendD3D9Device()->ChangeSizeOfRightRTWithAA(BBRightSize.cx, BBRightSize.cy);
		if (!(GINFO_REPLACEDSSURFACES || gInfo.ExtractDepthBuffer))
		{
			NSCALL_TRACE_RES(GetD3D9Device().CreateDepthStencilSurface(
				BBRightSize.cx, BBRightSize.cy, DepthStencilDesc.Format, 
				m_MultiSampleType, m_MultiSampleQuality, 
				FALSE, &m_pSecondaryDepthStencil, NULL), m_pSecondaryDepthStencil, 
				DEBUG_MESSAGE(_T("CreateDepthStencilSurface(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
				_T("MultisampleQuality = %08X, Discard = %1d, *ppSurface = %p, ")
				_T("pSharedHandle = %p)"), BBRightSize.cx, BBRightSize.cy, 
				GetFormatString(DepthStencilDesc.Format), 
				GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality, FALSE, 
				m_pSecondaryDepthStencil, NULL));
			m_pSecondaryDepthStencil->SetPrivateData(OriginalSurfaceSize_GUID, &m_BackBufferSizeBeforeScaling, sizeof(SIZE), 0);
			SetStereoObject(m_pPrimaryDepthStencil, m_pSecondaryDepthStencil);
		}
		else
			GetStereoObject(m_pPrimaryDepthStencil, &m_pSecondaryDepthStencil);

		INC_DEBUG_COUNTER(GetBaseDevice()->m_nDepthStencilSurfacesStereoSize, GetSurfaceSize(m_pSecondaryDepthStencil));
		SetMcClaudFlag(m_pPrimaryDepthStencil);
		if (DEBUGVER)
		{
			TCHAR buffer[128];
			_sntprintf_s(buffer, _TRUNCATE, _T("PrimaryDepthStencil-SC%d"), m_Index);
			SetObjectName(m_pPrimaryDepthStencil, buffer);
			_sntprintf_s(buffer, _TRUNCATE, _T("SecondaryDepthStencil-SC%d"), m_Index);
			SetObjectName(m_pSecondaryDepthStencil, buffer);
		}
		GetBaseDevice()->m_DepthStencil.Init(m_pPrimaryDepthStencil, m_pSecondaryDepthStencil);
	}

	return hResult;
}

void CSwapChain::Clear()
{
	ClearMcClaudFlag(m_pPrimaryDepthStencil);
	CBaseSwapChain::Clear();
	ClearMcClaudFlag(m_pStereoBBLeft);
	m_pStereoBBLeft.Release();
	m_pStereoBBRight.Release();
}

void CSwapChain::CheckDepthBuffer()
{
	INSIDE;
	DWORD DrawCountAfterClearDepthBuffer = m_DrawCountAfterClearDepthBuffer;
	m_DrawCountAfterClearDepthBuffer = 0;

	if (DrawCountAfterClearDepthBuffer < 5)
		return;

	if (m_bDepthBufferFounded && m_PrevDrawCountAfterClearDepthBuffer < DrawCountAfterClearDepthBuffer)
	{
		m_bDepthBufferFounded = FALSE;
		m_pLeftDepthStencilCorrectTexture.Release();
		m_pRightDepthStencilCorrectTexture.Release();
	}

	if (m_bDepthBufferFounded)
	{
		DEBUG_TRACE3(_T("Not Synchronized Depth Buffer: draw count %d\n"), DrawCountAfterClearDepthBuffer);
		return;
	}

	m_PrevDrawCountAfterClearDepthBuffer = DrawCountAfterClearDepthBuffer;

	HRESULT hResult = S_OK;
	D3DSURFACE_DESC desc;
	m_pLeftDepthStencilTexture->GetLevelDesc(0, &desc);
	NSCALL(GetBaseDevice()->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, 
		D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pLeftDepthStencilCorrectTexture, NULL )); // use internal method
	GetStereoObject(m_pLeftDepthStencilCorrectTexture, &m_pRightDepthStencilCorrectTexture);

	vbs* vb;
	GetBaseDevice()->CaptureAll(vb);
	NSCALL(GetBaseDevice()->m_pCopyState->Apply());
	DWORD rt[4] = { true, false, false, false };
	CComPtr<IDirect3DSurface9> pLeftSurf;
	CComPtr<IDirect3DSurface9> pRightSurf;
	if (!GetBaseDevice()->m_bRenderScene)
		GetD3D9Device().BeginScene();
	if (GetBaseDevice()->RenderToRight())
	{
		m_pRightDepthStencilCorrectTexture->GetSurfaceLevel(0, &pRightSurf);
		GetD3D9Device().SetRenderTarget(0, pRightSurf);
		GetBaseDevice()->OneToOneRender(m_pRightDepthStencilTexture, NULL, NULL);
	}
	m_pLeftDepthStencilCorrectTexture->GetSurfaceLevel(0, &pLeftSurf);
	GetD3D9Device().SetRenderTarget(0, pLeftSurf);
	GetBaseDevice()->OneToOneRender(m_pLeftDepthStencilTexture, NULL, NULL);
	if (!GetBaseDevice()->m_bRenderScene)
		GetD3D9Device().EndScene();
	GetBaseDevice()->ApplyAll(vb);

#ifndef FINAL_RELEASE
	DEBUG_TRACE3(_T("Synchronized Depth Buffer: draw count %d\n"), DrawCountAfterClearDepthBuffer);
	if (DO_RTDUMP && pLeftSurf)
	{
		TCHAR szFileName[MAX_PATH];
		_stprintf_s(szFileName, L"%s\\%.4d.%s.%s.%s", 
			GetBaseDevice()->m_DumpDirectory, GetBaseDevice()->m_nEventCounter,  _T("Synchronize.DS"), 
			GetBaseDevice()->GetModeString(), GetObjectName(pLeftSurf));
		if (GetBaseDevice()->RenderToRight())
			GetBaseDevice()->SaveStereoSurfaceToFile(szFileName, pLeftSurf, pRightSurf);
		else
		{
			_tcscpy(szFileName, L".Left");
			if (SUCCEEDED(SaveSurfaceToFile(szFileName, pLeftSurf))) {
				DEBUG_TRACE3(_T("Dumping %s\n"), GetFileNameFromPath(szFileName));
			} else {
				DEBUG_TRACE3(_T("Dumping error %s\n"), GetFileNameFromPath(szFileName));
			}
		}
	}
#endif

	m_bDepthBufferFounded = true;
}
