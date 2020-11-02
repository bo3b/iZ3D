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
#include "StereoRenderer.h"
#include "StereoRenderer_Hook.h"
#include "MonoRenderer_Hook.h"
#include "CommandDumper.h"

using namespace NonWide;

CStereoRenderer::CStereoRenderer(void)
	:CBaseStereoRenderer()
{
	m_Offset.cx = 0;
	m_Offset.cy = 0;
	m_pCurOffset = &m_Offset;
}

CStereoRenderer::~CStereoRenderer(void)
{
	UniqueUnhookCode(this, (PVOID*)&Original_Surface_LockRect);
	UniqueUnhookCode(this, (PVOID*)&Original_TexSurface_PDef_LockRect);
	UniqueUnhookCode(this, (PVOID*)&Original_DepthSurface_LockRect);	
	UniqueUnhookCode(this, (PVOID*)&Original_Texture_LockRect);	
	UniqueUnhookCode(this, (PVOID*)&Original_Surface_UnlockRect);
	UniqueUnhookCode(this, (PVOID*)&Original_TexSurface_PDef_UnlockRect);
	UniqueUnhookCode(this, (PVOID*)&Original_DepthSurface_UnlockRect);	
	UniqueUnhookCode(this, (PVOID*)&Original_Texture_UnlockRect);		
}

CMonoSwapChain* CStereoRenderer::CreateSwapChainWrapper(UINT index)
{ 
	return DNew CSwapChain(this, index);
}

HRESULT CStereoRenderer::StartEngineMode() 
{
	INSIDE;
	HRESULT hResult = S_OK;

	m_RenderTarget[0].Init(GetBaseSC()->m_pLeftBackBufferBeforeScaling, GetBaseSC()->m_pRightBackBufferBeforeScaling);
	
	return hResult;
}

void CStereoRenderer::StopEngineMode() 
{
	m_pStereoBBLeft.Release();
	m_pStereoBBRight.Release();
}

// Should be synchronized with CWideStereoRenderer::SelectOffset
bool CStereoRenderer::RenderInStereo(bool bCW)
{
	bool result;
	if (m_RenderTarget[0].m_pRightSurf != NULL)
	{
		if (bCW || m_DepthStencil.m_pRightSurf != NULL)
			result = true;
		else
			result = false;
	}
	else 
	{
		if (bCW)
			result = false;
		else
			result = m_DepthStencil.m_pRightSurf != NULL;
	}
	DEBUG_TRACE3(_T("\t RenderInStereo(bCW=%d)=%d\n"), bCW, result);
	return result;
}

bool CStereoRenderer::RenderInStereo()
{
	return RenderInStereo(m_dwColorWriteEnable[0] != 0);
}

bool CStereoRenderer::StereoDS()
{
	return (m_DepthStencil.m_pMainSurf == NULL) || (m_DepthStencil.m_pRightSurf != NULL);    
}

void CStereoRenderer::GetStereoSurfRects( IDirect3DSurface9* pLeft, IDirect3DSurface9* &pRight, D3DFORMAT &format, RECT *pLeftRect, RECT *pRightRect )
{
	D3DSURFACE_DESC desc;
	pLeft->GetDesc(&desc);
	format = desc.Format;
	SetRect(pLeftRect, 0, 0, desc.Width, desc.Height);
	SetRect(pRightRect, 0, 0, desc.Width, desc.Height);
}

template <VIEWINDEX viewIndex>
void CStereoRenderer::SetStereoRenderTarget() 
{
	INSIDE;
	HRESULT hResult = S_OK;

	bool bUpdatedPrimaryRT = false;

	if (m_pUpdateRTs[0] && m_RenderTarget[0].m_pRightSurf) {
		bUpdatedPrimaryRT = true;
		if (viewIndex == VIEWINDEX_RIGHT)	m_RenderTarget[0].m_pMainSurf->AddRef();
		NSCALL(m_Direct3DDevice.SetRenderTarget(0, m_RenderTarget[0].GetViewSurf<viewIndex>()));
		if (viewIndex == VIEWINDEX_LEFT)	m_RenderTarget[0].m_pMainSurf->Release();
	}
	if (m_pUpdateRTs[1] && m_RenderTarget[1].m_pRightSurf) {
		if (viewIndex == VIEWINDEX_RIGHT)	m_RenderTarget[1].m_pMainSurf->AddRef();
		NSCALL(m_Direct3DDevice.SetRenderTarget(1, m_RenderTarget[1].GetViewSurf<viewIndex>()));
		if (viewIndex == VIEWINDEX_LEFT)	m_RenderTarget[1].m_pMainSurf->Release();
	}
	if (m_pUpdateRTs[2] && m_RenderTarget[2].m_pRightSurf) {
		if (viewIndex == VIEWINDEX_RIGHT)	m_RenderTarget[2].m_pMainSurf->AddRef();
		NSCALL(m_Direct3DDevice.SetRenderTarget(2, m_RenderTarget[2].GetViewSurf<viewIndex>()));
		if (viewIndex == VIEWINDEX_LEFT)	m_RenderTarget[2].m_pMainSurf->Release();
	}
	if (m_pUpdateRTs[3] && m_RenderTarget[3].m_pRightSurf) {
		if (viewIndex == VIEWINDEX_RIGHT)	m_RenderTarget[3].m_pMainSurf->AddRef();
		NSCALL(m_Direct3DDevice.SetRenderTarget(3, m_RenderTarget[3].GetViewSurf<viewIndex>()));
		if (viewIndex == VIEWINDEX_LEFT)	m_RenderTarget[3].m_pMainSurf->Release();
	}
	if (m_UpdateDS && m_DepthStencil.m_pRightSurf) {
		if (viewIndex == VIEWINDEX_RIGHT)	m_DepthStencil.m_pMainSurf->AddRef(); // for iRacing
		NSCALL(m_Direct3DDevice.SetDepthStencilSurface(m_DepthStencil.GetViewSurf<viewIndex>()));
		if (viewIndex == VIEWINDEX_LEFT)	m_DepthStencil.m_pMainSurf->Release();
	}

	if (bUpdatedPrimaryRT)
	{
		//if (!m_bFullSizeViewport) 	// disabled because of AA surfaces
		{
			NSCALL(m_Direct3DDevice.SetViewport(&m_ViewPort));
		}
		if (m_dwScissorTestEnable
			//&& !m_bFullSizeScissorRect	// disabled because of AA surfaces
			) {
			NSCALL(m_Direct3DDevice.SetScissorRect(&m_ScissorRect));
		}
	}
}

STDMETHODIMP CStereoRenderer::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9 *pRenderTarget) 
{
	DUMP_CMD(SetRenderTarget,RenderTargetIndex,pRenderTarget);

	HRESULT hResult = S_OK;
	NSCALL_TRACE2(m_Direct3DDevice.SetRenderTarget(RenderTargetIndex, pRenderTarget),
		DEBUG_MESSAGE(_T("SetRenderTarget(RenderTargetIndex = %d, pRenderTarget = %p [%s])"), 
		RenderTargetIndex, pRenderTarget, GetObjectName(pRenderTarget)));

	if (FAILED(hResult))
		return hResult;

	m_RenderTarget[RenderTargetIndex].Init(pRenderTarget);
	if (RenderTargetIndex == 0 && pRenderTarget)
	{
		D3DSURFACE_DESC desc;
		pRenderTarget->GetDesc(&desc);
		m_PrimaryRTSize.cx = desc.Width;
		m_PrimaryRTSize.cy = desc.Height;
		InitViewport(&m_ViewPort, 0, 0, desc.Width, desc.Height);
		m_bFullSizeViewport = TRUE;
		SetRect(&m_ScissorRect, 0, 0, desc.Width, desc.Height);
		m_bFullSizeScissorRect = TRUE;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::SetDepthStencilSurface(IDirect3DSurface9 * pNewZStencil) 
{
	DUMP_CMD(SetDepthStencilSurface,pNewZStencil);

	HRESULT hResult = S_OK;

	NSCALL_TRACE2(m_Direct3DDevice.SetDepthStencilSurface(pNewZStencil),
		DEBUG_MESSAGE(_T("SetDepthStencilSurface(pNewZStencil = %p [%s])"), 
		pNewZStencil, GetObjectName(pNewZStencil)));

	if (FAILED(hResult))
		return hResult;
	
	m_DepthStencil.Init(pNewZStencil);
	return hResult;
}

STDMETHODIMP CStereoRenderer::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) 
{
	DUMP_CMD_STATE(SetTexture,Stage,pTexture);

	HRESULT hResult = S_OK;
	if(!m_bInStateBlock)
	{
		NSCALL_TRACE2(m_Direct3DDevice.SetTexture(Stage, pTexture),
			DEBUG_MESSAGE(_T("SetTexture(Stage = %d, pTexture = %p [%s])"), 
			Stage, pTexture, GetObjectName(pTexture)));

		if(SUCCEEDED(hResult))
		{
			if (Stage < D3DDMAPSAMPLER)
			{
				int ind = Stage;
				SetPipelineTexture(m_PSPipelineData, ind, pTexture);
				UpdateYouTubeTexture(Stage);
			}
			else
			{
				int ind = Stage - D3DDMAPSAMPLER;
				SetPipelineTexture(m_VSPipelineData, ind, pTexture);
			}
		}
	}
	else
		hResult = m_Direct3DDevice.SetTexture(Stage, pTexture);
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateRenderTargetEx(UINT Width,UINT Height,D3DFORMAT Format,
												   D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,
												   IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	if (pSharedHandle == NULL)
	{
		return CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, 
			Lockable, ppSurface, pSharedHandle);
	}

	HRESULT hResult = m_Direct3DDevice.CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, 
		Lockable, ppSurface, pSharedHandle, Usage);
	if (Succeded(hResult))
	{
		IDirect3DSurface9 *pSurface = *ppSurface;
		DUMP_CMD_ALL(CreateRenderTargetEx,Width,Height,Format,MultiSample,MultisampleQuality,Lockable,pSurface,pSharedHandle,Usage);
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, 
													 D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
													 BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE * pSharedHandle) 
{
	HRESULT hResult = S_OK;
	if (!gInfo.EnableAA)
	{
		MultiSample = D3DMULTISAMPLE_NONE;
		MultisampleQuality = 0;
	};
	NSCALL_TRACE1(m_Direct3DDevice.CreateRenderTarget( Width, Height, Format, MultiSample, 
		MultisampleQuality, Lockable, ppSurface, pSharedHandle ),
		DEBUG_MESSAGE(_T("CreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
		_T("MultisampleQuality = %08X, Lockable = %1d, *ppSurface = %p, pSharedHandle = %p)"), 
		Width, Height, GetFormatString(Format), GetMultiSampleString(MultiSample), 
		MultisampleQuality, Lockable, Indirect(ppSurface), pSharedHandle));

	if(!Succeded(hResult))
		return hResult;

	IDirect3DSurface9 *pSurface = *ppSurface;
	DUMP_CMD_ALL(CreateRenderTarget,Width,Height,Format,MultiSample,MultisampleQuality,Lockable,pSurface,pSharedHandle);

	CComPtr<IDirect3DSurface9> pRightSurface;
	if (IsStereoRenderTargetSurface(Width, Height))
	{
		SIZE size = { Width, Height };
		bool bAAMode = (MultiSample != D3DMULTISAMPLE_NONE);
		if (bAAMode)
			ChangeSizeOfRightRTWithAA(Width, Height);
		NSCALL_TRACE1(m_Direct3DDevice.CreateRenderTarget( Width, Height, Format, MultiSample, 
			MultisampleQuality, Lockable, &pRightSurface, pSharedHandle ),
			DEBUG_MESSAGE(_T("\tCreateRenderTarget(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Lockable = %1d, pRightSurface = %p, pSharedHandle = %p)"), 
			Width, Height, GetFormatString(Format), GetMultiSampleString(MultiSample), 
			MultisampleQuality, Lockable, (IDirect3DSurface9 *)pRightSurface, pSharedHandle));
		if(pRightSurface)
			pRightSurface->SetPrivateData(OriginalSurfaceSize_GUID, &size, sizeof(SIZE), 0);
		if (!Succeded(hResult))
			(*ppSurface)->Release();

	}
	if (SUCCEEDED(hResult))
	{
		SetStereoObject(*ppSurface, pRightSurface);

		//--- It's OK here, because RenderTarget is always D3DPOOL_DEFAULT ---
		void** p = *(void***)(*ppSurface);
		if( !IsCodeHookedByProxy(p[13], Proxy_DepthSurface_LockRect) )
			UniqueHookCode(this, p[13], Proxy_Surface_LockRect, (PVOID*)&Original_Surface_LockRect);
		if( !IsCodeHookedByProxy(p[14], Proxy_DepthSurface_UnlockRect) )
			UniqueHookCode(this, p[14], Proxy_Surface_UnlockRect, (PVOID*)&Original_Surface_UnlockRect);

#ifndef FINAL_RELEASE
		m_nRenderTargetsMonoSize += GetSurfaceSize(*ppSurface);
		TCHAR buffer[128];
		if(pRightSurface)
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("RenderTarget%d"), m_nRenderTargetCounter);
			SetObjectName(pRightSurface, buffer);
			m_nRenderTargetsStereoSize += GetSurfaceSize(pRightSurface);
		}
		else 
			_sntprintf_s(buffer, _TRUNCATE, _T("RenderTargetMono%d"), m_nRenderTargetCounter);
		SetObjectName(*ppSurface, buffer);
		DEBUG_TRACE1(_T("\tSetObjectName(*ppSurface = %p [%s])\n"), *ppSurface, GetObjectName(*ppSurface));
#endif
		m_nRenderTargetCounter++;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateDepthStencilSurfaceEx(UINT Width,UINT Height,D3DFORMAT Format,
										 D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,
										 BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	if (pSharedHandle == NULL)
	{
		return CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, 
			Discard, ppSurface, pSharedHandle);
	}

	HRESULT hResult = m_Direct3DDevice.CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, 
		Discard, ppSurface, pSharedHandle, Usage);
	if (Succeded(hResult))
	{
		IDirect3DSurface9 *pSurface = *ppSurface;
		DUMP_CMD_ALL(CreateDepthStencilSurfaceEx,Width,Height,Format,MultiSample,MultisampleQuality,Discard,pSurface,pSharedHandle,Usage);
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, 
															D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
															BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle ) 
{
	HRESULT hResult = S_OK;
	if (!gInfo.MonoDepthStencilSurfaces)
		Discard = FALSE;
	if (!gInfo.EnableAA)
	{
		MultiSample = D3DMULTISAMPLE_NONE;
		MultisampleQuality = 0;
	}
	if (GINFO_LOCKABLEDEPTHSTENCIL && MultiSample == D3DMULTISAMPLE_NONE)
		Format = D3DFMT_D16_LOCKABLE;

	bool bCreateStereo = IsStereoDepthStencilSurface(Width, Height);

	if (!gInfo.MonoDepthStencilTextures && MultiSample == D3DMULTISAMPLE_NONE && bCreateStereo &&
		(GINFO_REPLACEDSSURFACES || (gInfo.ExtractDepthBuffer && !GetBaseSC()->m_pLeftDepthStencilTexture)))
	{
		// debug texture
		CComPtr<IDirect3DTexture9> pLeftTex;
		NSCALL(GetBaseDevice()->CreateTexture(Width, Height, 1, D3DUSAGE_DEPTHSTENCIL, 
			GetBaseDevice()->m_DepthTexturesFormat24, D3DPOOL_DEFAULT, &pLeftTex, NULL )); // use internal method
		if (Succeded(hResult))
		{
			NSCALL(pLeftTex->GetSurfaceLevel(0, ppSurface));
			DEBUG_TRACE1(_T("\tReplaced DS surface by texture\n"));
		}
		return hResult;
	}

	NSCALL_TRACE1(m_Direct3DDevice.CreateDepthStencilSurface(Width, Height, Format, 
		MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle ),
		DEBUG_MESSAGE(_T("CreateDepthStencilSurface(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
		_T("MultisampleQuality = %08X, Discard = %1d, *ppSurface = %p, ")
		_T("pSharedHandle = %p)"), Width, Height, GetFormatString(Format), GetMultiSampleString(MultiSample), 
		MultisampleQuality, Discard, Indirect(ppSurface), pSharedHandle));

	if(!Succeded(hResult))
		return hResult;

	IDirect3DSurface9 *pSurface = *ppSurface;
	DUMP_CMD_ALL(CreateDepthStencilSurface,Width,Height,Format,MultiSample,MultisampleQuality,Discard,pSurface,pSharedHandle);

	CComPtr<IDirect3DSurface9> pRightSurface;
	if (bCreateStereo) 
	{
		//--- It's OK here, because DepthStencil is always D3DPOOL_DEFAULT ---
		void** p = *(void***)(*ppSurface);
		if( !IsCodeHookedByProxy(p[13], Proxy_Surface_LockRect) )
			UniqueHookCode(this, p[13], Proxy_DepthSurface_LockRect, (PVOID*)&Original_DepthSurface_LockRect);
		if( !IsCodeHookedByProxy(p[14], Proxy_Surface_UnlockRect) )
			UniqueHookCode(this, p[14], Proxy_DepthSurface_UnlockRect, (PVOID*)&Original_DepthSurface_UnlockRect);
		SIZE size = { Width, Height };
		if (gInfo.EnableAA)
			ChangeSizeOfRightRTWithAA(Width, Height);
		NSCALL_TRACE1(m_Direct3DDevice.CreateDepthStencilSurface(Width, Height, Format, 
			MultiSample, MultisampleQuality, Discard, &pRightSurface, pSharedHandle ),
			DEBUG_MESSAGE(_T("\tCreateDepthStencilSurface(Width = %d, Height = %d, Format = %s, MultiSample = %s,")
			_T("MultisampleQuality = %08X, Discard = %1d, pRightSurface = %p, ")
			_T("pSharedHandle = %p)"), Width, Height, GetFormatString(Format), GetMultiSampleString(MultiSample), 
			MultisampleQuality, Discard, (IDirect3DSurface9 *)pRightSurface, pSharedHandle));
		if(pRightSurface)
			pRightSurface->SetPrivateData(OriginalSurfaceSize_GUID, &size, sizeof(SIZE), 0);
		if (!Succeded(hResult))
			(*ppSurface)->Release();
	}
	if (SUCCEEDED(hResult))
	{
		SetStereoObject(*ppSurface, pRightSurface);
		SetSurfaceType(*ppSurface, Mono);

#ifndef FINAL_RELEASE
		m_nDepthStencilSurfacesMonoSize += GetSurfaceSize(*ppSurface);
		TCHAR buffer[128];
		if(pRightSurface)
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("DepthStencilSurface%d"), m_nDepthStencilSurfaceCounter);
			SetObjectName(pRightSurface, buffer);
			m_nDepthStencilSurfacesStereoSize += GetSurfaceSize(pRightSurface);
		}
		else
			_sntprintf_s(buffer, _TRUNCATE, _T("DepthStencilSurfaceMono%d"), m_nDepthStencilSurfaceCounter);
		SetObjectName(*ppSurface, buffer);
		DEBUG_TRACE1(_T("\tSetObjectName(*ppSurface = %p [%s])\n"), *ppSurface, GetObjectName(*ppSurface));
#endif
		m_nDepthStencilSurfaceCounter++;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
												D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE * pSharedHandle) 
{
	HRESULT hResult = S_OK;
	//if (GINFO_LOCKABLEDEPTHSTENCIL && (Usage & D3DUSAGE_DEPTHSTENCIL) > 0)
	//{
	//	if (SUCCEEDED(m_pDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,	D3DDEVTYPE_HAL,	
	//		D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D32F_LOCKABLE)))
	//		Format = D3DFMT_D32F_LOCKABLE;
	//}
	NSCALL_TRACE1(m_Direct3DDevice.CreateTexture(Width, Height, Levels, Usage, Format, 
		Pool, ppTexture, pSharedHandle),
		DEBUG_MESSAGE(_T("CreateTexture(Width = %d, Height = %d, Levels = %d, Usage = %s, ")
		_T("Format = %s, Pool = %s, *ppTexture = %p, pSharedHandle = %p)"), 
		Width, Height, Levels, GetUsageString(Usage), GetFormatString(Format), 
		GetPoolString(Pool), Indirect(ppTexture), pSharedHandle));

	if(!Succeded(hResult))
		return hResult;

	IDirect3DTexture9 *pTexture = *ppTexture;
	{
		DUMP_CMD_ALL(CreateTexture,Width,Height,Levels,Usage,Format,Pool,pTexture,pSharedHandle);
	}
	OnNewTextureCreated(pTexture,Levels);

	bool CreateInStereo = false;
	if (Usage & D3DUSAGE_RENDERTARGET) 
		CreateInStereo = IsStereoRenderTargetTexture(Format, Usage, Width, Height);
	else if (Usage & D3DUSAGE_DEPTHSTENCIL)
		CreateInStereo = IsStereoDepthStencilTexture(Width, Height);
	if (!CreateInStereo)
	{
		SetStereoObject(*ppTexture, 0);
		for (DWORD i = 0; i < (*ppTexture)->GetLevelCount(); i++)
		{
			CComPtr<IDirect3DSurface9> pLeftSurface;
			NSCALL((*ppTexture)->GetSurfaceLevel( i, &pLeftSurface ));
			SetStereoObject(pLeftSurface, 0);
		}
#ifndef FINAL_RELEASE
		TCHAR buffer[128];
		if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("RenderTargetTextureMono%d"), m_nRenderTargetTextureCounter);
			m_nRenderTargetTexturesMonoSize += GetTextureSize(*ppTexture);
		}
		else if ((Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("DepthStencilTextureMono%d"), m_nDepthStencilTextureCounter);
			m_nDepthStencilTexturesMonoSize += GetTextureSize(*ppTexture);
		}
		else if (Pool != D3DPOOL_DEFAULT)
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("Texture%d"), m_nTextureCounter);
			m_nTexturesSize += GetTextureSize(*ppTexture);
		}
		else
		{
			_sntprintf_s(buffer, _TRUNCATE, _T("TextureDef%d"), m_nTextureDefaultCounter);
			m_nTexturesDefaultSize += GetTextureSize(*ppTexture);
		}
		SetObjectName(*ppTexture, buffer);
		DEBUG_TRACE1(_T("\tSetObjectName(*ppTexture = %p [%s])\n"), *ppTexture, GetObjectName(*ppTexture));
#endif
		if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
			m_nRenderTargetTextureCounter++;
		else if ((Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
			m_nDepthStencilTextureCounter++;
		else if (Pool != D3DPOOL_DEFAULT)
			m_nTextureCounter++;
		else
			m_nTextureDefaultCounter++;
		if ((Usage & D3DUSAGE_RENDERTARGET) > 0 || (Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
		{
			if (Width == Height) 
				m_nRenderTargetSquareTextureCounter++;
			else if (IsLessThanBB(Width, Height)) 
				m_nRenderTargetLessThanBBTextureCounter++;
		}
		if (m_Direct3DDevice.getExMode() == EXMODE_EMULATE && Pool == D3DPOOL_MANAGED)
		{
			void** p = *(void***)(*ppTexture);
			UniqueHookCode(this, p[19], Proxy_Texture_LockRect, (PVOID*)&Original_Texture_LockRect);
			UniqueHookCode(this, p[20], Proxy_Texture_UnlockRect, (PVOID*)&Original_Texture_UnlockRect);
			CComPtr<IDirect3DSurface9> pFirstSurf;
			(*ppTexture)->GetSurfaceLevel(0, &pFirstSurf);
			if(pFirstSurf)
			{
				// Pool changed to D3DPOOL_DEFAULT
				p = *(void***)(pFirstSurf.p);
				UniqueHookCode(this, p[13], Proxy_TexSurface_LockRect, (PVOID*)&Original_TexSurface_PDef_LockRect);
				UniqueHookCode(this, p[14], Proxy_TexSurface_UnlockRect, (PVOID*)&Original_TexSurface_PDef_UnlockRect);
			}
		}

		CheckYouTubeTexture(Width, Height, Usage, Format, Pool, *ppTexture);
	}
	else
	{
		IDirect3DTexture9* pLeftTexture = *ppTexture;
		void** p = *(void***)pLeftTexture;
		UniqueHookCode(this, p[19], Proxy_Texture_LockRect, (PVOID*)&Original_Texture_LockRect);
		UniqueHookCode(this, p[20], Proxy_Texture_UnlockRect, (PVOID*)&Original_Texture_UnlockRect);
		CComPtr<IDirect3DSurface9> pFirstSurf;
		pLeftTexture->GetSurfaceLevel(0, &pFirstSurf);
		if(pFirstSurf)
		{
			//--- It's OK here, because RT Texture is always D3DPOOL_DEFAULT ---
			p = *(void***)(pFirstSurf.p);
			UniqueHookCode(this, p[13], Proxy_TexSurface_LockRect, (PVOID*)&Original_TexSurface_PDef_LockRect);
			UniqueHookCode(this, p[14], Proxy_TexSurface_UnlockRect, (PVOID*)&Original_TexSurface_PDef_UnlockRect);
		}
		//if (gInfo.EnableAA && (Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
		//	ChangeSizeOfRightRTWithAA(Width, Height);

		CComPtr<IDirect3DTexture9> pRightTexture;
		NSCALL_TRACE1(m_Direct3DDevice.CreateTexture(Width, Height, Levels, Usage, Format, 
			Pool, &pRightTexture, pSharedHandle),
			DEBUG_MESSAGE(_T("\tCreateTexture(Width = %d, Height = %d, Levels = %d, Usage = %s, ")
			_T("Format = %s, Pool = %s, pRightTexture = %p, pSharedHandle = %p)"), 
			Width, Height, Levels, GetUsageString(Usage), GetFormatString(Format), 
			GetPoolString(Pool), (IDirect3DTexture9 *)pRightTexture, pSharedHandle));
		if (Succeded(hResult))
		{
			SetStereoObject(pLeftTexture, pRightTexture);
			SetTextureType(pLeftTexture, Mono);

#ifndef FINAL_RELEASE
			TCHAR buffer[128];
			if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
			{
				_sntprintf_s(buffer, _TRUNCATE, _T("RenderTargetTexture%d"), m_nRenderTargetTextureCounter);
				m_nRenderTargetTexturesMonoSize += GetTextureSize(pRightTexture);
				m_nRenderTargetTexturesStereoSize += GetTextureSize(pRightTexture);
			}
			else
			{
				_sntprintf_s(buffer, _TRUNCATE, _T("DepthStencilTexture%d"), m_nDepthStencilTextureCounter);
				m_nDepthStencilTexturesMonoSize += GetTextureSize(pRightTexture);
				m_nDepthStencilTexturesStereoSize += GetTextureSize(pRightTexture);
			}
			SetObjectName(pLeftTexture, buffer);
			DEBUG_TRACE1(_T("\tSetObjectName(*ppTexture = %p [%s])\n"), *ppTexture, GetObjectName(*ppTexture));
			if(pRightTexture)
				SetObjectName(pRightTexture, buffer);
#endif
			if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
				m_nRenderTargetTextureCounter++;
			else
				m_nDepthStencilTextureCounter++;
			if (Width == Height) 
				m_nRenderTargetSquareTextureCounter++;
			else if (IsLessThanBB(Width, Height)) 
				m_nRenderTargetLessThanBBTextureCounter++;

			if ((Usage & D3DUSAGE_DEPTHSTENCIL) &&
				(Format != D3DFMT_D16) && (Format != D3DFMT_D24S8) &&
				(Format != D3DFMT_D32) && (Format != D3DFMT_D24X8) &&
				(GINFO_REPLACEDSSURFACES || gInfo.ExtractDepthBuffer) && 
				!GetBaseSC()->m_pLeftDepthStencilTexture)
			{
				GetBaseSC()->m_pMainDepthStencilSurface = pFirstSurf;
				GetBaseSC()->m_pLeftDepthStencilTexture = pLeftTexture;
				GetBaseSC()->m_pRightDepthStencilTexture = pRightTexture;
			}
		}
		else
			(*ppTexture)->Release();
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, 
													D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* 
													pSharedHandle) 
{
	HRESULT hResult;
	NSCALL_TRACE1(m_Direct3DDevice.CreateCubeTexture(EdgeLength, Levels, Usage, Format, 
		Pool, ppCubeTexture, pSharedHandle),
		DEBUG_MESSAGE(_T("CreateCubeTexture(EdgeLength = %u, Levels = %u, Usage = %s, ")
		_T("Format = %s, Pool = %s, *ppCubeTexture = %p, pSharedHandle = %p)"), 
		EdgeLength, Levels, GetUsageString(Usage), GetFormatString(Format), 
		GetPoolString(Pool), Indirect(ppCubeTexture), pSharedHandle));

	if(!Succeded(hResult))
		return hResult;

	IDirect3DCubeTexture9 *pCubeTexture = *ppCubeTexture;
	{
		DUMP_CMD_ALL(CreateCubeTexture,EdgeLength,Levels,Usage,Format,Pool,pCubeTexture,pSharedHandle);
	}
	OnNewTextureCreated(pCubeTexture,Levels);

	HOOK_GET_DEVICE(ppCubeTexture, Proxy_CubeTexture_GetDevice, Original_CubeTexture_GetDevice);

	SetStereoObject(*ppCubeTexture, 0);
	for (DWORD j = 0; j < 6; j++)
	{
		for (DWORD i = 0; i < (*ppCubeTexture)->GetLevelCount(); i++)
		{
			CComPtr<IDirect3DSurface9> pLeftSurface;
			NSCALL((*ppCubeTexture)->GetCubeMapSurface((D3DCUBEMAP_FACES)j, i, &pLeftSurface));
			SetStereoObject(pLeftSurface, 0);
		}
	}
#ifndef FINAL_RELEASE
	TCHAR buffer[128];
	if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
	{
		_sntprintf_s(buffer, _TRUNCATE, _T("RenderTargetCubeTextureMono%d"), m_nRenderTargetCubeTextureCounter);
		m_nRenderTargetCubeTexturesMonoSize += GetCubeTextureSize(*ppCubeTexture);
	}
	else if ((Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
	{
		_sntprintf_s(buffer, _TRUNCATE, _T("DepthStencilCubeTextureMono%d"), m_nDepthStencilCubeTextureCounter);
		m_nDepthStencilCubeTexturesMonoSize += GetCubeTextureSize(*ppCubeTexture);
	}
	else if (Pool != D3DPOOL_DEFAULT)
	{
		_sntprintf_s(buffer, _TRUNCATE, _T("CubeTexture%d"), m_nCubeTextureCounter);
		m_nCubeTexturesSize += GetCubeTextureSize(*ppCubeTexture);
	}
	else
	{
		_sntprintf_s(buffer, _TRUNCATE, _T("CubeTextureDef%d"), m_nCubeTextureDefaultCounter);
		m_nCubeTexturesDefaultSize += GetCubeTextureSize(*ppCubeTexture);
	}
	SetObjectName(*ppCubeTexture, buffer);
	DEBUG_TRACE1(_T("\tSetObjectName(*ppTexture = %p [%s])\n"), *ppCubeTexture, GetObjectName(*ppCubeTexture));
#endif
	if ((Usage & D3DUSAGE_RENDERTARGET) > 0) 
		m_nRenderTargetCubeTextureCounter++;
	else if ((Usage & D3DUSAGE_DEPTHSTENCIL) > 0) 
		m_nDepthStencilCubeTextureCounter++;
	else if (Pool != D3DPOOL_DEFAULT)
		m_nCubeTextureCounter++;
	else
		m_nCubeTextureDefaultCounter++;
	return hResult;
}

STDMETHODIMP CStereoRenderer::Clear(DWORD Count, CONST D3DRECT * pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil ) 
{
	DUMP_CMD(Clear,Count,pRects,Flags,Color,Z,Stencil);

	HRESULT hResult = S_OK;
	if (GINFO_LOCKABLEDEPTHSTENCIL)
	{
		Flags &= ~D3DCLEAR_STENCIL;
	}
	if ((Flags & D3DCLEAR_ZBUFFER) && gInfo.ExtractDepthBuffer &&
		(GetBaseDevice()->m_DepthStencil.m_pMainSurf == GetBaseSC()->m_pMainDepthStencilSurface))
		GetSC()->CheckDepthBuffer();
	bool bRenderToRight = RenderToRight();
	if (bRenderToRight && 
		((Flags & D3DCLEAR_TARGET) == 0 || m_RenderTarget[0].m_pRightSurf == NULL) &&
		((Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) == 0 || m_DepthStencil.m_pRightSurf == NULL))
		bRenderToRight = false;
	if (bRenderToRight)
	{
		DWORD updateRTs[4];
		updateRTs[0] = updateRTs[1] = updateRTs[2] = updateRTs[3] = Flags & D3DCLEAR_TARGET;
		bool updateDS = (Flags & D3DCLEAR_ZBUFFER || Flags & D3DCLEAR_STENCIL);

		m_pUpdateRTs = updateRTs;
		m_UpdateDS = updateDS;

		if (!DEBUG_SKIP_SET_STEREO_RENDERTARGET)
			SetStereoRenderTarget<VIEWINDEX_RIGHT>();
		DWORD RightFlags = Flags;
		if (!RenderInStereo((Flags & D3DCLEAR_TARGET) != 0))
		{
			if (updateRTs[0] != 0 && m_RenderTarget[0].m_pRightSurf != NULL)
				RightFlags = Flags & D3DCLEAR_TARGET;
			else if	(updateDS != 0 && m_DepthStencil.m_pRightSurf != NULL)
				RightFlags = Flags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
		}
		NSCALL_TRACE2(m_Direct3DDevice.Clear(Count, pRects, RightFlags, Color, Z, Stencil),
			DEBUG_MESSAGE(_T("Clear(%d, %s, %s, %08X, %12.7f, %ld)"), 
			Count, GetClearRectString(Count, pRects),
			GetClearFlagsString(RightFlags), Color, Z, Stencil));

		if (!DEBUG_SKIP_SET_STEREO_RENDERTARGET)
			SetStereoRenderTarget<VIEWINDEX_LEFT>();
		NSCALL_TRACE2(m_Direct3DDevice.Clear(Count, pRects, Flags, Color, Z, Stencil),
			DEBUG_MESSAGE(_T("\tClear(%d, %s, %s, %08X, %12.7f, %ld)"), 
			Count, GetClearRectString(Count, pRects),
			GetClearFlagsString(Flags), Color, Z, Stencil));

		if (Count == 0)
		{
			if (Flags & D3DCLEAR_TARGET)
			{
				m_RenderTarget[0].SetType(Cleared);
				m_RenderTarget[1].SetType(Cleared);
				m_RenderTarget[2].SetType(Cleared);
				m_RenderTarget[3].SetType(Cleared);
			}
			if (Flags & D3DCLEAR_ZBUFFER && Flags & D3DCLEAR_STENCIL)
				m_DepthStencil.SetSurfaceType(Cleared);
		}
	}
	else
	{
		NSCALL_TRACE2(m_Direct3DDevice.Clear(Count, pRects, Flags, Color, Z, Stencil),
			DEBUG_MESSAGE(_T("Clear(%d, %s, %s, %08X, %12.7f, %ld)"), 
			Count, GetClearRectString(Count, pRects),
			GetClearFlagsString(Flags), Color, Z, Stencil));

		if (Flags & D3DCLEAR_TARGET)
		{
			m_RenderTarget[0].SetType(Mono);
			m_RenderTarget[1].SetType(Mono);
			m_RenderTarget[2].SetType(Mono);
			m_RenderTarget[3].SetType(Mono);
		}
		if (Flags & D3DCLEAR_ZBUFFER && Flags & D3DCLEAR_STENCIL)
			m_DepthStencil.SetSurfaceType(Mono);
	}

	if (DO_RTDUMP)
	{
		if (Flags & D3DCLEAR_TARGET) {
			DumpRTSurfaces(bRenderToRight, _T("Clear"), _T(""), TRUE);
		}
		if (Flags & D3DCLEAR_ZBUFFER) {
			DumpDSSurfaces(bRenderToRight, _T("Clear"), _T(""));
		}
		DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
		m_nEventCounter++;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::StretchRect( 
	IDirect3DSurface9 *pSourceSurface, CONST RECT * pSourceRect, 
	IDirect3DSurface9 *pDestSurface, CONST RECT * pDestRect, D3DTEXTUREFILTERTYPE Filter ) 
{
	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSurface9> pRightSourceSurface;
	CComPtr<IDirect3DSurface9> pRightDestSurface;
	if (pSourceSurface && pDestSurface)
	{
		GetStereoObject(pSourceSurface, &pRightSourceSurface);
		GetStereoObject(pDestSurface, &pRightDestSurface);
		bool bRenderToRight = RenderToRight();
		if (pRightDestSurface)
		{
			CComPtr<IDirect3DBaseTexture9> pLeftTexDest;
			CComPtr<IDirect3DBaseTexture9> pLeftTexSource;
			TextureType destTextureType = Mono;
			if (bRenderToRight)
			{
				if (GetParentTexture(pSourceSurface, &pLeftTexSource))
				{
					destTextureType = GetTextureType(pLeftTexSource);
					if (destTextureType == Mono && pRightSourceSurface && gInfo.BltDstEye == 1)
						destTextureType = Stereo;
				}
				else if (pRightSourceSurface)
					destTextureType = Stereo;
			}
			else
				pRightDestSurface.Release();
			if (GetParentTexture(pDestSurface, &pLeftTexDest))
			{
				DEBUG_TRACE3(_T("\tSetTextureType %p [%s]: TextureType = %s\n"), 
					pLeftTexDest, GetObjectName(pLeftTexDest), GetTextureTypeString(destTextureType));
				SetTextureType(pLeftTexDest, destTextureType);
			}

			if (destTextureType != Mono && gInfo.BltDstEye == 0)
			{
				if (pRightSourceSurface == NULL)
					pRightSourceSurface = pSourceSurface;
				CONST RECT * pRightSourceRect = pSourceRect;
				RECT RightSourceRect;
				D3DSURFACE_DESC desc;
				pRightSourceSurface->GetDesc(&desc);
				if (pSourceRect == NULL || (desc.Usage & D3DUSAGE_DEPTHSTENCIL))
				{
					SIZE size = {desc.Width, desc.Height};
					DWORD sizeOfData = sizeof(SIZE);
					if (!(desc.Usage & D3DUSAGE_DEPTHSTENCIL)) // DepthStencil surface not support copy using rect's
						pRightSourceSurface->GetPrivateData(OriginalSurfaceSize_GUID, &size, &sizeOfData);
					SetRect(&RightSourceRect, 0, 0, size.cx, size.cy);
					pRightSourceRect = &RightSourceRect;
				}
				CONST RECT * pRightDestRect = pDestRect;
				RECT RightDestRect;
				if (pDestRect == NULL || (desc.Usage & D3DUSAGE_DEPTHSTENCIL))
				{
					pRightDestSurface->GetDesc(&desc);
					SIZE size = {desc.Width, desc.Height};
					DWORD sizeOfData = sizeof(SIZE);
					if (!(desc.Usage & D3DUSAGE_DEPTHSTENCIL)) // DepthStencil surface not support copy using rect's
						pRightDestSurface->GetPrivateData(OriginalSurfaceSize_GUID, &size, &sizeOfData);
					SetRect(&RightDestRect, 0, 0, size.cx, size.cy);
					pRightDestRect = &RightDestRect;
				}

				NSCALL_TRACE2(m_Direct3DDevice.StretchRect( pRightSourceSurface, pRightSourceRect, pRightDestSurface, pRightDestRect, Filter ),
					DEBUG_MESSAGE(_T("\tStretchRect(pSourceSurface = %p [%s], pSourceRect = %s, ")
					_T("pDestSurface = %p [%s], pDestRect = %s, Filter = %s)"), 
					pRightSourceSurface, GetObjectName(pRightSourceSurface), GetRectString(pRightSourceRect), 
					pRightDestSurface, GetObjectName(pRightDestSurface), GetRectString(pRightDestRect), 
					GetTextureFilterTypeString(Filter)));
				if (DO_RTDUMP)
				{
					TCHAR szFileName[MAX_PATH];
					_stprintf_s(szFileName, _T("%s\\%.4d.StretchRect.Source.%s.Right"), 
						DRAW_HISTORY, m_nEventCounter, GetObjectName(pRightSourceSurface));
					SaveSurfaceToFile(szFileName, pRightSourceSurface);
					DEBUG_TRACE2(_T("Dumping %s\n"), szFileName);
					_stprintf_s(szFileName, _T("%s\\%.4d.StretchRect.Dest.%s.Right"), 
						DRAW_HISTORY, m_nEventCounter, GetObjectName(pRightDestSurface));
					SaveSurfaceToFile(szFileName, pRightDestSurface);
					DEBUG_TRACE2(_T("Dumping %s\n"), szFileName);
				}
			}
			if  (bRenderToRight && gInfo.BltDstEye == 1 && pRightDestSurface) // switch dest
				pDestSurface = pRightDestSurface;
		}
		else
		{
			if (bRenderToRight && gInfo.BltSrcEye == 1 && pRightSourceSurface) // switch src
				pSourceSurface = pRightSourceSurface;
		}
	}
	NSCALL_TRACE2(m_Direct3DDevice.StretchRect( pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter ),
		DEBUG_MESSAGE(_T("StretchRect(pSourceSurface = %p [%s], pSourceRect = %s, ")
		_T("pDestSurface = %p [%s], pDestRect = %s, Filter = %s)"), 
		pSourceSurface, GetObjectName(pSourceSurface), GetRectString(pSourceRect), 
		pDestSurface, GetObjectName(pDestSurface), GetRectString(pDestRect), 
		GetTextureFilterTypeString(Filter)));
	if (DO_RTDUMP)
	{
		TCHAR szFileName[MAX_PATH];
		D3DSURFACE_DESC srcDesc;
		pSourceSurface->GetDesc(&srcDesc);
		if (srcDesc.Usage & D3DUSAGE_RENDERTARGET) // otherwise cause looping
		{
			_stprintf_s(szFileName, _T("%s\\%.4d.StretchRect.Source.%s.Left"), 
				DRAW_HISTORY, m_nEventCounter, GetObjectName(pSourceSurface));
			SaveSurfaceToFile(szFileName, pSourceSurface);
			DEBUG_TRACE2(_T("Dumping %s\n"), szFileName);
		}
		_stprintf_s(szFileName, _T("%s\\%.4d.StretchRect.Dest.%s.Left"), 
			DRAW_HISTORY, m_nEventCounter, GetObjectName(pDestSurface));
		SaveSurfaceToFile(szFileName, pDestSurface);
		DEBUG_TRACE2(_T("Dumping %s\n"), szFileName);
		DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
		m_nEventCounter++;
	}
	if (USE_UM_PRESENTER)
	{
		FlushCommands();
	}
	
	if (SUCCEEDED(hResult))
	{
		DUMP_CMD(StretchRect,pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter);
	}

	return hResult;
}

STDMETHODIMP CStereoRenderer::ColorFill(IDirect3DSurface9 * pSurface, CONST RECT * pRect, D3DCOLOR color ) 
{
	DUMP_CMD_ALL(ColorFill,pSurface,pRect,color); // -- shift2u

	HRESULT hResult = S_OK;
	CComPtr<IDirect3DSurface9> pRightSurface;
	NSCALL_TRACE2(m_Direct3DDevice.ColorFill( pSurface, pRect, color ),
		DEBUG_MESSAGE(_T("ColorFill(pSurface = %p [%s], pRect = %s, color = %08X);\n"), 
		pSurface, GetObjectName(pSurface), GetRectString(pRect), color));
	if (RenderToRight() && pSurface)
	{
		GetStereoObject(pSurface, &pRightSurface);
		if (pRightSurface)
		{
			NSCALL_TRACE2(m_Direct3DDevice.ColorFill( pRightSurface, pRect, color ),
				DEBUG_MESSAGE(_T("ColorFill(pSurface = %p [%s], pRect = %s, color = %08X);\n"), 
				pRightSurface, GetObjectName(pRightSurface), GetRectString(pRect), color));
		}
	}
	CComPtr<IDirect3DBaseTexture9> pTexDest;
	if (GetParentTexture(pSurface, &pTexDest))
	{
		if (!RenderToRight())
		{
			DEBUG_TRACE3(_T("\tSetTextureType %p [%s]: TextureType = %s\n"), 
				pTexDest, GetObjectName(pTexDest), GetTextureTypeString(Mono));
			SetTextureType(pTexDest, Mono);
		}
		else if (pRightSurface && pRect != NULL)
		{
			DEBUG_TRACE3(_T("\tSetTextureType %p [%s]: TextureType = %s\n"), 
				pTexDest, GetObjectName(pTexDest), GetTextureTypeString(Cleared));
			SetTextureType(pTexDest, Cleared);
		}
	}
	if (DO_RTDUMP)
	{
		DumpSurfaces(_T("ColorFill"), pSurface, pRightSurface);
		DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
		m_nEventCounter++;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, 
	IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) 
{
	DUMP_CMD_ALL(UpdateSurface,pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);

	HRESULT hResult;
	NSCALL_TRACE2(m_Direct3DDevice.UpdateSurface(pSourceSurface, pSourceRect, 
		pDestinationSurface, pDestPoint),
		DEBUG_MESSAGE(_T("UpdateSurface(pSourceSurface = %p [%s], pSourceRect = %s, ")
		_T("pDestinationSurface = %p [%s], pDestPoint = (%d, &d))"), 
		pSourceSurface, GetObjectName(pSourceSurface), GetRectString(pSourceRect), 
		pDestinationSurface, GetObjectName(pDestinationSurface), 
		pDestPoint? pDestPoint->x: 0, pDestPoint? pDestPoint->y: 0));

	if (FAILED(hResult))
		return hResult;

	if ((!pDestPoint || (pDestPoint->x == 0 && pDestPoint->y == 0)))
	{
		bool bCalculateForAll = 
#ifdef ZLOG_ALLOW_TRACING
			(zlog::GetSeverity() == zlog::SV_FLOOD);
#else
			false;
#endif
		if(!DEBUG_SKIP_CRC_CHECK && (!g_ProfileData.TextureCRCMultiplier.empty() || bCalculateForAll))
		{
			CComPtr<IDirect3DBaseTexture9> pDestBaseTex;
			if (GetParentTexture(pDestinationSurface, &pDestBaseTex))
				CRCTextureFromSurfaceCheck(pSourceSurface, pSourceRect, pDestBaseTex);
		}
	}

	CComPtr<IDirect3DBaseTexture9> pTexDest;
	if (GetParentTexture(pDestinationSurface, &pTexDest))
	{
		DEBUG_TRACE3(_T("\tSetTextureType %p [%s]: TextureType = %s\n"), 
			pTexDest, GetObjectName(pTexDest), GetTextureTypeString(Mono));
		SetTextureType(pTexDest, Mono);
	}
	if (DO_RTDUMP)
	{
		DumpSurfaces(_T("UpdateSurface.Src"), pSourceSurface, NULL);
		DumpSurfaces(_T("UpdateSurface.Dst"), pDestinationSurface, NULL);
		DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
		m_nEventCounter++;
	}
	return hResult;
}

STDMETHODIMP CStereoRenderer::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) 
{
	DUMP_CMD(UpdateTexture,pSourceTexture,pDestinationTexture);

	HRESULT hResult;
	NSCALL_TRACE2(m_Direct3DDevice.UpdateTexture(pSourceTexture, pDestinationTexture),
		DEBUG_MESSAGE(_T("UpdateTexture(pSourceTexture = %p [%s], pDestinationTexture = %p [%s])"), 
		pSourceTexture, GetObjectName(pSourceTexture), pDestinationTexture, GetObjectName(pDestinationTexture)));

	if (FAILED(hResult))
		return hResult;

	bool bCalculateForAll = 
#ifdef ZLOG_ALLOW_TRACING
		(zlog::GetSeverity() == zlog::SV_FLOOD);
#else
		false;
#endif
	if (!DEBUG_SKIP_CRC_CHECK && (!g_ProfileData.TextureCRCMultiplier.empty() || bCalculateForAll))
	{
		CComQIPtr<IDirect3DTexture9> pSrcTexture = pSourceTexture;
		CComQIPtr<IDirect3DTexture9> pDstTexture = pDestinationTexture;
		if (pSrcTexture && pDstTexture)
		{
			CComPtr<IDirect3DSurface9> pSourceSurface;
			pSrcTexture->GetSurfaceLevel(0, &pSourceSurface);
			if (pSourceSurface)
				CRCTextureFromSurfaceCheck(pSourceSurface, NULL, pDstTexture);
		}
		else
		{
			CComQIPtr<IDirect3DCubeTexture9> pSrcCubeTexture = pSourceTexture;
			CComQIPtr<IDirect3DCubeTexture9> pDstCubeTexture = pDestinationTexture;
			if (pSrcCubeTexture && pDstCubeTexture)
			{
				CComPtr<IDirect3DSurface9> pSourceSurface;
				pSrcCubeTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &pSourceSurface);
				if (pSourceSurface)
					CRCTextureFromSurfaceCheck(pSourceSurface, NULL, pDstCubeTexture);
			}
		}
	}
	SetTextureType(pDestinationTexture, Mono);
	if (DO_RTDUMP)
	{
		DumpTextures(_T("UpdateTexture.Src"), pSourceTexture, NULL);
		DumpTextures(_T("UpdateTexture.Dst"), pDestinationTexture, NULL);
		DEBUG_TRACE3(_T("---------- Event %i complete ----------\n"), m_nEventCounter);
		m_nEventCounter++;
	}
	return hResult;
}

template<typename T, typename Q>
void CStereoRenderer::SetPipelineTexture( ShaderPipelineData<T, Q>& pipeline, DWORD ind, IDirect3DBaseTexture9* pTexture )
{
	if (ind < pipeline.m_MaxTextures && pipeline.m_Texture[ind].m_pLeft != pTexture)
	{
		CRCTextureCheck(pipeline, ind, pTexture);
		pipeline.m_Texture[ind].Init(pTexture, pTexture);
		if (pipeline.m_Texture[ind].m_pRight)
			pipeline.m_StereoTexturesMask |= 1 << ind;
		else
			pipeline.m_StereoTexturesMask &= ~(1 << ind);
	}
}

void CStereoRenderer::ChangeSizeOfRightRTWithAA( UINT &Width, UINT &Height )
{
	Height++;
}

void CStereoRenderer::ChangeSizeOfRightRTWithAA( LONG &Width, LONG &Height )
{
	Height++;
}
