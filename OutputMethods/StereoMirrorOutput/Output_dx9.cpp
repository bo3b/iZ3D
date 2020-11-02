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
// Output.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"
#include <d3dx9.h>

#include "Shaders_dx9/shaderStereoMirror.h"

using namespace DX9Output;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

OUTPUT_API void* CALLBACK CreateOutputDX9(DWORD mode, DWORD spanMode)
{
	return new StereoMirrorOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return 0;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Stereo Mirror");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Horizontal flip secondary monitor");
		return TRUE;
	case 1:
		strcpy_s(name, size, "Vertical flip secondary monitor");
		return TRUE;
	case 2:
		strcpy_s(name, size, "Horizontal flip primary and vertical flip secondary");
		return TRUE;
	case 3:
		strcpy_s(name, size, "Vertical flip primary and horizontal flip secondary");
		return TRUE;
	default:
		return FALSE;
	}
}

StereoMirrorOutput::StereoMirrorOutput(DWORD mode, DWORD spanMode)
: PostprocessedOutput(mode, spanMode)
{
}

StereoMirrorOutput::~StereoMirrorOutput(void)
{
}

bool StereoMirrorOutput::InvertPrimaryMonitorX()
{
	return m_OutputMode == 2 || m_OutputMode == 4;
}

bool StereoMirrorOutput::InvertPrimaryMonitorY()
{
	return m_OutputMode == 3 || m_OutputMode == 5;
}

bool StereoMirrorOutput::InvertSecondaryMonitorX()
{
	return m_OutputMode == 0 || m_OutputMode == 3 || m_OutputMode == 4;
}

bool StereoMirrorOutput::InvertSecondaryMonitorY()
{
	return m_OutputMode == 1 || m_OutputMode == 2 || m_OutputMode == 5;
}

HRESULT StereoMirrorOutput::InitializeSCResources(CBaseSwapChain* pSwapChain)
{
	HRESULT res = S_OK;
	for(int i = 0; i< 4; i++)
	{
		pSwapChain->m_Vertex[i].tu2  = pSwapChain->m_Vertex[i].tu;    
		pSwapChain->m_Vertex[i].tv2  = pSwapChain->m_Vertex[i].tv;
		if (InvertSecondaryMonitorX())
			pSwapChain->m_Vertex[i].tu2 = 1 - pSwapChain->m_Vertex[i].tu2;
		if (InvertSecondaryMonitorY())
			pSwapChain->m_Vertex[i].tv2 = 1 - pSwapChain->m_Vertex[i].tv2;
		if (InvertPrimaryMonitorX())
			pSwapChain->m_Vertex[i].tu = 1 - pSwapChain->m_Vertex[i].tu;
		if (InvertPrimaryMonitorY())
			pSwapChain->m_Vertex[i].tv = 1 - pSwapChain->m_Vertex[i].tv;
	}
	return res;
}

HRESULT StereoMirrorOutput::SetStates( CBaseSwapChain* pSwapChain )
{
	if(m_SpanMode != 0)
		return S_OK;

	RECT rect;
	if ( pSwapChain->m_pSourceRect )
		rect = *pSwapChain->m_pSourceRect;
	else
		SetRect(&rect, 0, 0, pSwapChain->m_BackBufferSize.cx, pSwapChain->m_BackBufferSize.cy);

	float tu0 = (float)rect.left / pSwapChain->m_BackBufferSize.cx;
	float tu1 = (float)rect.right / pSwapChain->m_BackBufferSize.cx;
	float tv0 = (float)rect.top / pSwapChain->m_BackBufferSize.cy;
	float tv1 = (float)rect.bottom / pSwapChain->m_BackBufferSize.cy;
	pSwapChain->m_Vertex[0].x   = rect.left - .5f;
	pSwapChain->m_Vertex[0].y   = rect.top - .5f;
	pSwapChain->m_Vertex[0].tu  = tu0;
	pSwapChain->m_Vertex[0].tv  = tv0;

	pSwapChain->m_Vertex[1].x   = rect.right - .5f;
	pSwapChain->m_Vertex[1].y   = rect.top - .5f;
	pSwapChain->m_Vertex[1].tu  = tu1;
	pSwapChain->m_Vertex[1].tv  = tv0;

	pSwapChain->m_Vertex[2].x   = rect.right - .5f;
	pSwapChain->m_Vertex[2].y   = rect.bottom - .5f;
	pSwapChain->m_Vertex[2].tu  = tu1;
	pSwapChain->m_Vertex[2].tv  = tv1;

	pSwapChain->m_Vertex[3].x   = rect.left - .5f;
	pSwapChain->m_Vertex[3].y   = rect.bottom - .5f;
	pSwapChain->m_Vertex[3].tu  = tu0;
	pSwapChain->m_Vertex[3].tv  = tv1;

	for(int i = 0; i< 4; i++)
	{
		pSwapChain->m_Vertex[i].tu2  = pSwapChain->m_Vertex[i].tu;    
		pSwapChain->m_Vertex[i].tv2  = pSwapChain->m_Vertex[i].tv;
		if (InvertSecondaryMonitorX())
			pSwapChain->m_Vertex[i].tu2 = (tu0 + tu1) - pSwapChain->m_Vertex[i].tu2;
		if (InvertSecondaryMonitorY())
			pSwapChain->m_Vertex[i].tv2 = (tv0 + tv1) - pSwapChain->m_Vertex[i].tv2;
		if (InvertPrimaryMonitorX())
			pSwapChain->m_Vertex[i].tu = (tu0 + tu1) - pSwapChain->m_Vertex[i].tu;
		if (InvertPrimaryMonitorY())
			pSwapChain->m_Vertex[i].tv = (tv0 + tv1) - pSwapChain->m_Vertex[i].tv;
	}
	return S_OK;
}

HRESULT StereoMirrorOutput::InitializeResources()
{
	HRESULT res = S_OK;

	if(FAILED(res = m_pd3dDevice->CreatePixelShader((DWORD*)g_ps20_shaderStereoMirror, &m_pShader1stPass)))
		return res;

	CComPtr<IDirect3DStateBlock9> pSavedState;
	res = m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &pSavedState);
	{
		m_pd3dDevice->SetTexture(0, 0);
		m_pd3dDevice->SetTexture(1, 0);
		m_pd3dDevice->SetTexture(2, 0);
		m_pd3dDevice->SetTexture(3, 0);
		m_pd3dDevice->SetRenderState(D3DRS_WRAP0,               0);
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,             FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,    FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,     FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,           FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE,            D3DFILL_SOLID);
		m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,            D3DCULL_NONE);
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING,			FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,		FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_CLIPPING,			FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,		FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_SRGBWRITEENABLE,		FALSE);
		for(int i=0; i < 3; i++)
		{
			m_pd3dDevice->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); 
			m_pd3dDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );	
			m_pd3dDevice->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			m_pd3dDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, 0 );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXMIPLEVEL, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, 1);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_SRGBTEXTURE, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_ELEMENTINDEX, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_DMAPOFFSET, 0);
		}

		m_pd3dDevice->SetFVF( D3DFVF_D3DTVERTEX_2T );		
		m_pd3dDevice->SetPixelShader(m_pShader1stPass);
		res = m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pState);
	}
	res = pSavedState->Apply();

	return res;
}

DWORD StereoMirrorOutput::GetSecondWindowCaps()
{
	DWORD c = 0;
	if (InvertSecondaryMonitorX())
		c |= swcInvertHorizontal;
	if (InvertSecondaryMonitorY())
		c |= swcInvertVertical;
	return c;
}
