/* IZ3D_FILE: $Id: PostprocessedOutput.cpp 1540 2008-12-25 14:41:36Z kostik $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: kostik $
* $Revision: 1540 $
* $Date: 2008-12-25 17:41:36 +0300 (Чт, 25 дек 2008) $
* $LastChangedBy: kostik $
* $URL: svn://svn.neurok.ru/dev/driver/trunk/S3DWrapper9/PostprocessedOutput.cpp $
*/
#include "StdAfx.h"
#include "PostprocessedOutput_dx9.h"

using namespace DX9Output;

PostprocessedOutput::PostprocessedOutput(DWORD mode, DWORD spanMode)
:	OutputMethod(mode, spanMode)
,	m_bMultiPassRendering(false)
{
	m_Caps = ocOutputFromTexture;
}

PostprocessedOutput::~PostprocessedOutput(void)
{
}

HRESULT PostprocessedOutput::Initialize(IDirect3DDevice9* dev, bool MultiPass)
{
	HRESULT res = S_OK;
	Clear();
	OutputMethod::Initialize(dev, MultiPass);

	m_bMultiPassRendering = MultiPass;
	if (m_bMultiPassRendering)
	{
		D3DCAPS9 caps;
		m_pd3dDevice->GetDeviceCaps(&caps);
		if (GetOutputChainsNumber() == 1)
			m_bMultiPassRendering = false;
		else if (!MultiPass && GetOutputChainsNumber() > caps.NumSimultaneousRTs)
			m_bMultiPassRendering = true;
	}
	res = InitializeResources();
	return res;
}

HRESULT PostprocessedOutput::InitializeSCData(CBaseSwapChain* pSwapChain)
{
	HRESULT hResult = S_OK;
	SIZE bb = pSwapChain->m_BackBufferSize;

	pSwapChain->m_Vertex[0].x   = 0 - .5f;
	pSwapChain->m_Vertex[0].y   = 0 - .5f;
	pSwapChain->m_Vertex[0].z   = 0;
	pSwapChain->m_Vertex[0].rhw = 1;
	pSwapChain->m_Vertex[0].tu  = 0;
	pSwapChain->m_Vertex[0].tv  = 0;

	pSwapChain->m_Vertex[1].x   = bb.cx - .5f;
	pSwapChain->m_Vertex[1].y   = 0 - .5f;
	pSwapChain->m_Vertex[1].z   = 0;
	pSwapChain->m_Vertex[1].rhw = 1;
	pSwapChain->m_Vertex[1].tu  = 1;
	pSwapChain->m_Vertex[1].tv  = 0;

	pSwapChain->m_Vertex[2].x   = bb.cx - .5f;
	pSwapChain->m_Vertex[2].y   = bb.cy - .5f;
	pSwapChain->m_Vertex[2].z   = 0;
	pSwapChain->m_Vertex[2].rhw = 1;
	pSwapChain->m_Vertex[2].tu  = 1;
	pSwapChain->m_Vertex[2].tv  = 1;

	pSwapChain->m_Vertex[3].x   = 0 - .5f;
	pSwapChain->m_Vertex[3].y   = bb.cy - .5f;
	pSwapChain->m_Vertex[3].z   = 0;
	pSwapChain->m_Vertex[3].rhw = 1;
	pSwapChain->m_Vertex[3].tu  = 0;
	pSwapChain->m_Vertex[3].tv  = 1;
	hResult = InitializeSCResources(pSwapChain);
	return hResult;
}

void PostprocessedOutput::Clear()
{
	ClearResources();
	m_pShaderMono.Release();
	m_pShaderMonoAndGamma.Release();
	m_pShader1stPass.Release();
	m_pShader1stPassAndGamma.Release();
	m_pShader2ndPass.Release();
	m_pShader2ndPassAndGamma.Release();
	m_pState.Release();
	m_pStateWithGamma.Release();
	OutputMethod::Clear();
}

HRESULT PostprocessedOutput::Output(CBaseSwapChain* pSwapChain)
{
	HRESULT hResult = S_OK;

	SetStatesBeforeDraw(pSwapChain);

	IDirect3DSurface9* primary = pSwapChain->m_pPrimaryBackBuffer;
	IDirect3DSurface9* secondary = pSwapChain->m_pSecondaryBackBuffer;

	NSCALL(m_pd3dDevice->BeginScene());
	if(SUCCEEDED(hResult))
	{
		DrawFirstPass(pSwapChain, primary, secondary);

		//--- no MRT supported ---
		if (m_bMultiPassRendering)
			DrawSecondPass(pSwapChain, secondary);

		NSCALL(m_pd3dDevice->EndScene());
	}
	return hResult;
}

HRESULT PostprocessedOutput::DrawFirstPass( CBaseSwapChain* pSwapChain, IDirect3DSurface9* primary, IDirect3DSurface9* secondary )
{
	HRESULT hResult = S_OK;
	NSCALL(m_pd3dDevice->SetRenderTarget(0, primary));
	if (!m_bMultiPassRendering && GetOutputChainsNumber() > 1)
		NSCALL(m_pd3dDevice->SetRenderTarget(1, secondary));
	NSCALL(m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, pSwapChain->m_VertexSinglePass1Pass, sizeof(pSwapChain->m_VertexSinglePass1Pass[0])));
	return hResult;
}

HRESULT PostprocessedOutput::DrawSecondPass( CBaseSwapChain* pSwapChain, IDirect3DSurface9* secondary )
{
	HRESULT hResult = S_OK;
	//DEBUG_TRACE2("Two pass rendering\n");
	NSCALL(m_pd3dDevice->SetRenderTarget(0, secondary));
	if(pSwapChain->m_CurrentRAMPisIdentity || !m_pShader2ndPassAndGamma)
		m_pd3dDevice->SetPixelShader(m_pShader2ndPass);
	else
		m_pd3dDevice->SetPixelShader(m_pShader2ndPassAndGamma);

	NSCALL(m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, pSwapChain->m_VertexSinglePass2Pass, sizeof(pSwapChain->m_VertexSinglePass2Pass[0])));
	return hResult;
}

HRESULT PostprocessedOutput::SetStatesBeforeDraw( CBaseSwapChain* pSwapChain )
{
	HRESULT hResult = S_OK;
	IDirect3DTexture9* left = pSwapChain->GetLeftBackBufferTex();
	IDirect3DTexture9* right = pSwapChain->GetRightBackBufferTex();
	IDirect3DTexture9* leftDepth = pSwapChain->GetLeftDepthStencilTex();
	IDirect3DTexture9* rightDepth = pSwapChain->GetRightDepthStencilTex();

	if(pSwapChain->m_CurrentRAMPisIdentity || m_pStateWithGamma == NULL)
		m_pState->Apply();
	else
		m_pStateWithGamma->Apply();

	if (left == right && m_pShaderMono && !m_bMultiPassRendering)
	{
		// Mono Mode
		if(pSwapChain->m_CurrentRAMPisIdentity || m_pStateWithGamma == NULL) {
			NSCALL(m_pd3dDevice->SetPixelShader(m_pShaderMono));
		} else {
			NSCALL(m_pd3dDevice->SetPixelShader(m_pShaderMonoAndGamma));
		}
	}
	if (m_bProcessGammaRamp)
		m_pd3dDevice->SetTexture(2, pSwapChain->m_GammaRAMPTexture);
	SetStates(pSwapChain);

	NSCALL(m_pd3dDevice->SetTexture(0, left));
	NSCALL(m_pd3dDevice->SetTexture(1, right));
	if (m_Caps & ocRequiredDepthMap)
	{
		NSCALL(m_pd3dDevice->SetTexture(3, leftDepth));
		NSCALL(m_pd3dDevice->SetTexture(4, rightDepth));
	}
	ProcessSubMode();
	return hResult;
}
