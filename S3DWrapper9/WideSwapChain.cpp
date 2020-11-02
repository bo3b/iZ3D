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
#include "WideSwapChain.h"
#include "WideStereoRenderer.h"
#include "WideStereoRenderer_Hook.h"
#include "CommandDumper.h"

using namespace Wide;

CWideSwapChain::CWideSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index)
: CBaseSwapChain(pDirect3DDevice9, index)
{
}

CWideSwapChain::~CWideSwapChain(void)
{

}

HRESULT CWideSwapChain::InitializeMode( D3DSURFACE_DESC &RenderTargetDesc )
{
	HRESULT hResult = S_OK;
	GetWideD3D9Device()->m_pOffset = NULL;
	UINT Width, Height;

	if(m_bScalingEnabled)
	{
		GetWideD3D9Device()->InitializeOffset(m_BackBufferSize.cx, m_BackBufferSize.cy, &m_BBOffset, Width, Height);
		NSCALL_TRACE_RES(GetD3D9Device().CreateRenderTarget(
			Width, Height, RenderTargetDesc.Format, m_MultiSampleType, m_MultiSampleQuality, 
			m_bLockableBackBuffer, &m_pWidePrimaryBackBuffer, 0), m_pWidePrimaryBackBuffer,
			DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Lockable = %1d, *ppSurface = %p, pSharedHandle = %p)"), 
			Width, Height, GetFormatString(RenderTargetDesc.Format), 
			GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality,
			m_bLockableBackBuffer, m_pWidePrimaryBackBuffer, 0));
		if(SUCCEEDED(hResult) && m_pWidePrimaryBackBuffer)
		{
			INC_DEBUG_COUNTER(GetBaseDevice()->m_nRenderTargetsStereoSize, GetSurfaceSize(m_pWidePrimaryBackBuffer));
			CComPtr<IWideSurfaceData> pWideSurfaceData;
			CWideSurfaceData::CreateInstance(&pWideSurfaceData);
			pWideSurfaceData->SetOffset(&m_BBOffset);
			SIZE Size = m_BackBufferSize;
			pWideSurfaceData->SetSize(&Size);
			pWideSurfaceData->SetFormat(RenderTargetDesc.Format);
			pWideSurfaceData->SetUsage(RenderTargetDesc.Usage);
			NSCALL(m_pWidePrimaryBackBuffer->SetPrivateData(__uuidof(IWideSurfaceData), (void *)pWideSurfaceData, sizeof(IUnknown *), D3DSPD_IUNKNOWN)); 

			SetStereoObject(m_pWidePrimaryBackBuffer, 0);

			m_pLeftBackBuffer = m_pWidePrimaryBackBuffer;
			m_pRightBackBuffer = m_pWidePrimaryBackBuffer;

			GetNarrowRect(m_pWidePrimaryBackBuffer, &m_LeftViewRect);
			GetRightViewRect(&m_RightViewRect, &m_LeftViewRect, &m_BBOffset);

			if (DEBUGVER)
			{
				TCHAR buffer[128];
				_sntprintf_s(buffer, _TRUNCATE, _T("WidePrimaryBackBuffer-SC%d"), m_Index);
				SetObjectName(m_pWidePrimaryBackBuffer, buffer);
			}
		}
	}

	GetWideD3D9Device()->InitializeOffset(m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, &m_BBOffsetBeforeScaling, Width, Height);
	NSCALL_TRACE_RES(GetD3D9Device().CreateRenderTarget(
		Width, Height, RenderTargetDesc.Format, m_MultiSampleType, m_MultiSampleQuality, 
		m_bLockableBackBuffer, &m_pWidePrimaryBackBufferBeforeScaling, 0), m_pWidePrimaryBackBufferBeforeScaling,
		DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
		_T("MultisampleQuality = %08X, Lockable = %1d, *ppSurface = %p, pSharedHandle = %p)"), 
		Width, Height, GetFormatString(RenderTargetDesc.Format), 
		GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality,
		m_bLockableBackBuffer, m_pWidePrimaryBackBufferBeforeScaling, 0));
	if(SUCCEEDED(hResult) && m_pWidePrimaryBackBufferBeforeScaling)
	{
		SetMcClaudFlag(m_pWidePrimaryBackBufferBeforeScaling);
		INC_DEBUG_COUNTER(GetBaseDevice()->m_nRenderTargetsStereoSize, GetSurfaceSize(m_pWidePrimaryBackBufferBeforeScaling));
		CComPtr<IWideSurfaceData> pWideSurfaceData;
		CWideSurfaceData::CreateInstance(&pWideSurfaceData);
		pWideSurfaceData->SetOffset(&m_BBOffsetBeforeScaling);
		GetWideD3D9Device()->m_pOffset = pWideSurfaceData->GetOffset();
		SIZE Size = m_BackBufferSizeBeforeScaling;
		pWideSurfaceData->SetSize(&Size);
		pWideSurfaceData->SetFormat(RenderTargetDesc.Format);
		pWideSurfaceData->SetUsage(RenderTargetDesc.Usage);
		NSCALL(m_pWidePrimaryBackBufferBeforeScaling->SetPrivateData(__uuidof(IWideSurfaceData), (void *)pWideSurfaceData, sizeof(IUnknown *), D3DSPD_IUNKNOWN)); 

		SetStereoObject(m_pWidePrimaryBackBufferBeforeScaling, 0);

		m_pLeftBackBufferBeforeScaling = m_pWidePrimaryBackBufferBeforeScaling;
		m_pRightBackBufferBeforeScaling = m_pWidePrimaryBackBufferBeforeScaling;

		GetNarrowRect(m_pWidePrimaryBackBufferBeforeScaling, &m_LeftViewRectBeforeScaling);
		GetRightViewRect(&m_RightViewRectBeforeScaling, &m_LeftViewRectBeforeScaling, &m_BBOffsetBeforeScaling);
		if(m_bScalingEnabled)
		{
			TDisplayScalingMode mode = (TDisplayScalingMode)gInfo.DisplayScalingMode;
			ScaleRects(mode, &m_LeftViewRectBeforeScaling,  &m_LeftViewRect,  &m_SrcLeft,  &m_DstLeft);
			ScaleRects(mode, &m_RightViewRectBeforeScaling, &m_RightViewRect, &m_SrcRight, &m_DstRight);
		}

		if (DEBUGVER)
		{
			TCHAR buffer[128];
			_sntprintf_s(buffer, _TRUNCATE, _T("WidePrimaryBackBufferBeforeScaling-SC%d"), m_Index);
			SetObjectName(m_pWidePrimaryBackBufferBeforeScaling, buffer);
		}

		void** p = *(void***)m_pWidePrimaryBackBufferBeforeScaling.p;
		UniqueHookCode(GetBaseDevice(), p[12], Proxy_Surface_GetDesc, (PVOID*)&Original_Surface_GetDesc);
		if( !IsCodeHookedByProxy(p[13], Proxy_DepthSurface_LockRect) )
			UniqueHookCode(GetBaseDevice(), p[13], Proxy_Surface_LockRect, (PVOID*)&Original_Surface_LockRect);
		if( !IsCodeHookedByProxy(p[14], Proxy_DepthSurface_UnlockRect) )
			UniqueHookCode(GetBaseDevice(), p[14], Proxy_Surface_UnlockRect, (PVOID*)&Original_Surface_UnlockRect);
	}

	GetWideD3D9Device()->m_pDepthOffset = NULL;
	if (m_pPrimaryDepthStencil)
	{
		D3DSURFACE_DESC DepthStencilDesc;
		ZeroMemory( &DepthStencilDesc, sizeof(DepthStencilDesc) );
		NSCALL(m_pPrimaryDepthStencil->GetDesc( &DepthStencilDesc ));
		if (!gInfo.MonoDepthStencilTextures && (GINFO_REPLACEDSSURFACES || (gInfo.ExtractDepthBuffer && (gInfo.ModeObtainingDepthBuffer == 0))) && 
			m_MultiSampleType == D3DMULTISAMPLE_NONE)
		{
			// debug texture
			CComPtr<IDirect3DTexture9> pWideTex;
			NSCALL(GetBaseDevice()->CreateTexture(m_BackBufferSizeBeforeScaling.cx, m_BackBufferSizeBeforeScaling.cy, 1, D3DUSAGE_DEPTHSTENCIL, 
				GetBaseDevice()->m_DepthTexturesFormat24, D3DPOOL_DEFAULT, &pWideTex, NULL )); // use internal method
			m_pWidePrimaryDepthStencil = m_pMainDepthStencilSurface;
			DEBUG_TRACE1(_T("\tReplaced DS surface by texture\n"));
		}
		else
		{
			NSCALL_TRACE_RES(GetD3D9Device().CreateDepthStencilSurface(
				Width, Height, DepthStencilDesc.Format, 
				m_MultiSampleType, m_MultiSampleQuality, 
				m_PresentationParameters[0].Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL, 
				&m_pWidePrimaryDepthStencil, NULL ), m_pWidePrimaryDepthStencil, 
				DEBUG_MESSAGE(_T("CreateDepthStencilSurface(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
				_T("MultisampleQuality = %08X, Discard = %1d, *ppSurface = %p, ")
				_T("pSharedHandle = %p)"), Width, Height, GetFormatString(DepthStencilDesc.Format), 
				GetMultiSampleString(m_MultiSampleType), m_MultiSampleQuality, 
				m_OriginalPresentationParameters.Flags & D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL, 
				m_pWidePrimaryDepthStencil, NULL));

			if (SUCCEEDED(hResult) && m_pWidePrimaryDepthStencil)
			{		
				INC_DEBUG_COUNTER(GetBaseDevice()->m_nDepthStencilSurfacesStereoSize, GetSurfaceSize(m_pWidePrimaryDepthStencil));
				CComPtr<IWideSurfaceData> pWideSurfaceData;
				CWideSurfaceData::CreateInstance(&pWideSurfaceData);
				pWideSurfaceData->SetOffset(&m_BBOffsetBeforeScaling);
				GetWideD3D9Device()->m_pDepthOffset = pWideSurfaceData->GetOffset();
				SIZE Size = m_BackBufferSizeBeforeScaling;
				pWideSurfaceData->SetSize(&Size);
				pWideSurfaceData->SetFormat(DepthStencilDesc.Format);
				pWideSurfaceData->SetUsage(D3DUSAGE_DEPTHSTENCIL);
				NSCALL(m_pWidePrimaryDepthStencil->SetPrivateData(__uuidof(IWideSurfaceData), (void *)pWideSurfaceData, sizeof(IUnknown *), D3DSPD_IUNKNOWN)); 
				SetStereoObject(m_pWidePrimaryDepthStencil, 0);

				if (DEBUGVER)
				{
					TCHAR buffer[128];
					_sntprintf_s(buffer, _TRUNCATE, _T("WideDepthStencil-SC%d"), m_Index);
					SetObjectName(m_pWidePrimaryDepthStencil, buffer);
				}

				void** p = *(void***)m_pWidePrimaryDepthStencil.p;
				UniqueHookCode(GetBaseDevice(), p[12], Proxy_DepthSurface_GetDesc, (PVOID*)&Original_DepthSurface_GetDesc);
				if( !IsCodeHookedByProxy(p[13], Proxy_Surface_LockRect) )
					UniqueHookCode(GetBaseDevice(), p[13], Proxy_DepthSurface_LockRect, (PVOID*)&Original_DepthSurface_LockRect);
				if( !IsCodeHookedByProxy(p[14], Proxy_Surface_UnlockRect) )
					UniqueHookCode(GetBaseDevice(), p[14], Proxy_DepthSurface_UnlockRect, (PVOID*)&Original_DepthSurface_UnlockRect);
			}
		}
		SetMcClaudFlag(m_pWidePrimaryDepthStencil);
		GetBaseDevice()->m_DepthStencil.Init(m_pWidePrimaryDepthStencil, NULL);
	}

	return hResult;
}

void CWideSwapChain::Clear()
{
	CBaseSwapChain::Clear();
	ClearMcClaudFlag(m_pWidePrimaryBackBufferBeforeScaling);
	m_pWidePrimaryBackBufferBeforeScaling.Release();
	m_pWidePrimaryBackBuffer.Release();
	ClearMcClaudFlag(m_pWidePrimaryDepthStencil);
	m_pWidePrimaryDepthStencil.Release();
}

void CWideSwapChain::CheckDepthBuffer()
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
	CComPtr<IDirect3DTexture9> pWideTex;
	NSCALL(GetBaseDevice()->CreateTexture(desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, 
		D3DFMT_R32F, D3DPOOL_DEFAULT, &pWideTex, NULL )); // use internal method
	GetLeftTexture(pWideTex, &m_pLeftDepthStencilCorrectTexture);
	GetStereoObject(m_pLeftDepthStencilCorrectTexture, &m_pRightDepthStencilCorrectTexture);

	CComPtr<IDirect3DSurface9> pLeft;
	CComPtr<IDirect3DSurface9> pRight;
	GetWideD3D9Device()->CopyDepthSurfaces(GetBaseDevice()->m_DepthStencil.m_pMainTex, pLeft, pRight);
	pLeft.Release();
	pRight.Release();

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
		_stprintf_s(szFileName, _TRUNCATE, L"%s\\%.4u.%s.%s.%s", 
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
