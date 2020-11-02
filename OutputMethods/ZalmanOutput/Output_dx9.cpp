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
// InterlacedOutput.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"

#include "Shaders_dx9/shaderMono.h"
#include "Shaders_dx9/shaderHorizontalInterlaced.h"
#include "Shaders_dx9/shaderHorizontalInterlacedOptimized.h"

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
	return new ZalmanOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return 0;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Zalman");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Horizontal (Optimized)");
		return TRUE;
	case 1:
		strcpy_s(name, size, "Horizontal");
		return TRUE;
	default:
		return FALSE;
	}
}

ZalmanOutput::ZalmanOutput(DWORD mode, DWORD spanMode)
: PostprocessedOutput(mode, spanMode)
{
}

ZalmanOutput::~ZalmanOutput(void)
{
}

HRESULT ZalmanOutput::InitializeResources()
{
	HRESULT res = S_OK;
	res = m_pd3dDevice->CreatePixelShader((DWORD*)g_ps20_shaderMono, &m_pShaderMono);
	switch(m_OutputMode)
	{
	case 1:
		res = m_pd3dDevice->CreatePixelShader((DWORD*)g_ps20_shaderHorizontalInterlaced, &m_pShader1stPass);
		break;
	default: //0
		res = m_pd3dDevice->CreatePixelShader((DWORD*)g_ps20_shaderHorizontalInterlacedOptimized, &m_pShader1stPass);
		break;
	}
	if (FAILED(res))
		return res;

	CComPtr<IDirect3DStateBlock9> pSavedState;
	res = m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &pSavedState);
	{
		m_pd3dDevice->SetTexture(0, 0);
		m_pd3dDevice->SetTexture(1, 0);
		m_pd3dDevice->SetTexture(2, 0);
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
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, 0 );
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXMIPLEVEL, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, 1);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_SRGBTEXTURE, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_ELEMENTINDEX, 0);
			m_pd3dDevice->SetSamplerState( i, D3DSAMP_DMAPOFFSET, 0);
		}
		m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		m_pd3dDevice->SetFVF( D3DFVF_D3DTVERTEX_1T );
		m_pd3dDevice->SetPixelShader(m_pShader1stPass);

		res = m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pState);
	}
	res = pSavedState->Apply();

	return res;
}

HRESULT ZalmanOutput::SetStates(CBaseSwapChain* pSwapChain)
{
	HRESULT res = S_OK;
	D3DSURFACE_DESC desc;
	res = pSwapChain->m_pPrimaryBackBuffer->GetDesc(&desc);
	float c[8] = { desc.Width * 0.5f, desc.Height * 0.5f, 0.5f / desc.Width, 0.5f / desc.Height,
		(pSwapChain->m_DstMonitorPoint.x % 2) * 0.5f, 
		(pSwapChain->m_DstMonitorPoint.y % 2) * 0.5f, 
		0.0f, 0.0f};  
	m_pd3dDevice->SetPixelShaderConstantF(2, c, 2);
	return res;
}

bool ZalmanOutput::FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] )
{
	OutputMethod::FindAdapter(pDirect3D9, DeviceNum, DeviceType, Adapters);

//	// Find the DirectX method of adapter ordinal, that is the VR920's
//	UINT AdapterCount = pDirect3D9->GetAdapterCount();
//	DISPLAY_DEVICEA			MonitorDevice;
//	MonitorDevice.cb = sizeof( DISPLAY_DEVICEA );
//	for( UINT i = 0; i < AdapterCount; i++ ) {
//		D3DADAPTER_IDENTIFIER9	Adapter_Identify;
//		pDirect3D9->GetAdapterIdentifier( i, 0, &Adapter_Identify );
//		for( int j=0; EnumDisplayDevicesA(Adapter_Identify.DeviceName, j, &MonitorDevice, 0x0 ); j++ ) {
//			if( (MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
//				strstr( MonitorDevice.DeviceID, "ZMT2200" ) ) {
//					Adapters[0] = i;
//					return true;
//			}
//		}
//	}
//#ifndef FINAL_RELEASE
	return true;
//#else
//	return false;
//#endif
}
