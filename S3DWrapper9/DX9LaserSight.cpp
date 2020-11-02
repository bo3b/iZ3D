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
#include "DX9LaserSight.h"
#include "BaseSwapChain.h"
#include "..\ArchiveFile\blob.h"

DX9LaserSight::DX9LaserSight(void)
{  
}

void DX9LaserSight::Clear()
{
	m_pSightTexture = 0;
	m_pDrawStates = 0;
	m_pDevice = 0;
}

HRESULT DX9LaserSight::Initialize(IDirect3DDevice9* pDevice)
{	
	HRESULT hResult;
	m_pDevice = pDevice;

	D3DXIMAGE_INFO pSrcInfo;
	if(gInfo.CustomLaserSightIconPath[0] != 0)
	{	
		NSCALL(D3DXCreateTextureFromFileEx(pDevice, gInfo.CustomLaserSightIconPath, 
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &pSrcInfo, NULL, &m_pSightTexture)); 
	}
	else  
	{	
		Blob b;
		if (b.Initialize( _T("Driver/LaserSight.png") ))
		{
			NSCALL(D3DXCreateTextureFromFileInMemoryEx( pDevice, b.GetData(), (UINT)b.GetSize(), 
				D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &pSrcInfo, NULL, &m_pSightTexture ));
		}
		else
		{
			DEBUG_MESSAGE(_T("Reading of Driver/LaserSight.png failed."));
			return E_FAIL;
		}
	}

	
	CComPtr<IDirect3DStateBlock9> pSavedState;
	NSCALL(m_pDevice->CreateStateBlock(D3DSBT_ALL, &pSavedState));

	//--- create draw states ---
	pDevice->SetTexture(0, m_pSightTexture);
	pDevice->SetFVF(D3DFVF_D3DTVERTEX_1T);
	pDevice->SetPixelShader(NULL);
	pDevice->SetVertexShader(NULL);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND,	 D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_WRAP0,            0);
	pDevice->SetRenderState(D3DRS_ZENABLE,          FALSE);
	pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE); 
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
	pDevice->SetRenderState(D3DRS_FOGENABLE,        FALSE);
	pDevice->SetRenderState(D3DRS_FILLMODE,         D3DFILL_SOLID);
	pDevice->SetRenderState(D3DRS_CULLMODE,         D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_LIGHTING,			FALSE);
	pDevice->SetRenderState(D3DRS_STENCILENABLE,	FALSE);
	pDevice->SetRenderState(D3DRS_CLIPPING,			FALSE);
	pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,	FALSE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE,	FALSE);

	pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); 
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
	pDevice->SetSamplerState(0, D3DSAMP_MAXMIPLEVEL,   0);
	pDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1);
	pDevice->SetSamplerState(0, D3DSAMP_SRGBTEXTURE,   0);
	pDevice->SetSamplerState(0, D3DSAMP_ELEMENTINDEX,  0);
	pDevice->SetSamplerState(0, D3DSAMP_DMAPOFFSET,    0);

	NSCALL(pDevice->CreateStateBlock(D3DSBT_ALL, &m_pDrawStates));
	NSCALL(pSavedState->Apply());

	return S_OK;
}

void DX9LaserSight::Draw(CBaseSwapChain* pSwapChain, float frameTimeDelta)
{
	HRESULT hResult;
	pSwapChain->m_LaserSightData.SelectFrame(frameTimeDelta);
	NSCALL(m_pDrawStates->Apply());

	if (pSwapChain->IsStereoActive())
	{
		//--- Draw right sight ---
		D3DTVERTEX_1T* r = (D3DTVERTEX_1T*)pSwapChain->m_LaserSightData.GetLocationRight();
		NSCALL(m_pDevice->SetRenderTarget(0, pSwapChain->m_pRightBackBuffer));
		NSCALL(m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, r, sizeof(D3DTVERTEX_1T)));
	}

	//--- Draw left sight ---
	D3DTVERTEX_1T* l = (D3DTVERTEX_1T*)pSwapChain->m_LaserSightData.GetLocationLeft();
	NSCALL(m_pDevice->SetRenderTarget(0, pSwapChain->m_pLeftBackBuffer));
	NSCALL(m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, l, sizeof(D3DTVERTEX_1T)));
}

SIZE DX9LaserSight::GetTextureSize()
{
	D3DSURFACE_DESC desc;
	m_pSightTexture->GetLevelDesc(0, &desc);
	SIZE s = { desc.Width, desc.Height };
	return s;		
}
