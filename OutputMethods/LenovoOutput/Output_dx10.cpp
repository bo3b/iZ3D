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
// LenovoOutput.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx10.h"

using namespace DX10Output;

#include "Shaders_dx10/shaderAnaglyph.h"
#include "Shaders_dx10/shaderColorAnaglyph.h"
#include "Shaders_dx10/shaderYellowBlueAnaglyph.h"
#include "Shaders_dx10/shaderOptAnaglyph.h"
#include "Shaders_dx10/shaderOptAnaglyphAndGamma.h"

#include "Shaders_dx10/shaderFullScreenQuadVS.h"


OUTPUT_API void* CALLBACK CreateOutputDX10(DWORD mode, DWORD spanMode)
{
	return new LenovoOutput(mode, spanMode);
}

LenovoOutput::LenovoOutput(DWORD mode, DWORD spanMode)
: PostprocessedOutput(mode, spanMode)
{
	if(m_OutputMode == 1)
		m_bProcessGammaRamp = true;

	//--- create signature ---
	SigVS.NumInputSignatureEntries  = 1;
	SigVS.NumOutputSignatureEntries = 2;
	SigVS.pInputSignature  = &SigEntryVS[0];
	SigEntryVS[0].SystemValue = D3D10_SB_NAME_VERTEX_ID;
	SigEntryVS[0].Register    = 0;
	SigEntryVS[0].Mask        = 1;
	SigVS.pOutputSignature = &SigEntryVS[SigVS.NumInputSignatureEntries];
	SigEntryVS[1].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryVS[1].Register    = 0;
	SigEntryVS[1].Mask        = 0x03;
	SigEntryVS[2].SystemValue = D3D10_SB_NAME_POSITION;
	SigEntryVS[2].Register    = 1;
	SigEntryVS[2].Mask        = 0x0F;

	SigPS.NumInputSignatureEntries  = 1;
	SigPS.NumOutputSignatureEntries = 1;
	SigPS.pInputSignature  = &SigEntryPS[0];
	SigEntryPS[0].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryPS[0].Register    = 0;
	SigEntryPS[0].Mask        = 0x03;
	SigPS.pOutputSignature = &SigEntryPS[SigPS.NumInputSignatureEntries];
	SigEntryPS[1].SystemValue = D3D10_SB_NAME_UNDEFINED;
	SigEntryPS[1].Register    = 0;
	SigEntryPS[1].Mask        = 0x07;
}

LenovoOutput::~LenovoOutput(void)
{
}

void LenovoOutput::InitializeResources(D3DDeviceWrapper* pD3DWrapper)
{	
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper->hDevice;
	CONST UINT* pCode;
	ParseShaderBlob(g_shaderFullScreenQuadVS, sizeof(g_shaderFullScreenQuadVS), &pCode);
	SIZE_T size = GET_ORIG.pfnCalcPrivateShaderSize(hDevice, pCode, &SigVS);
	NEW_HANDLE(m_hVSShader, size);
	D3D10DDI_HRTSHADER hRTShader = { NULL };
	GET_ORIG.pfnCreateVertexShader(hDevice, pCode, m_hVSShader, hRTShader, &SigVS);

	size = 0;
	pCode = NULL;
	switch(m_OutputMode)
	{
	case 1: // Optimized anaglyph
		ParseShaderBlob(g_shaderOptAnaglyph, sizeof(g_shaderOptAnaglyph), &pCode);
		break;
	case 2: // Black & White anaglyph
		ParseShaderBlob(g_shaderAnaglyph, sizeof(g_shaderAnaglyph), &pCode);
		break;
	case 3: // Yellow/Blue anaglyph
		ParseShaderBlob(g_shaderYellowBlueAnaglyph, sizeof(g_shaderYellowBlueAnaglyph), &pCode);
		break;
	default: // Color anaglyph
		ParseShaderBlob(g_shaderColorAnaglyph, sizeof(g_shaderColorAnaglyph), &pCode);
		break;
	}
	size = GET_ORIG.pfnCalcPrivateShaderSize(hDevice, pCode, &SigPS);
	NEW_HANDLE(m_hPSShader, size);
	GET_ORIG.pfnCreatePixelShader(hDevice, pCode, m_hPSShader, hRTShader, &SigPS);
}

void LenovoOutput::ClearResources( D3DDeviceWrapper* pD3DWrapper )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper->hDevice;

	GET_ORIG.pfnDestroyShader(hDevice, m_hVSShader);
	DELETE_HANDLE(m_hVSShader);
	GET_ORIG.pfnDestroyShader(hDevice, m_hPSShader);
	DELETE_HANDLE(m_hPSShader);
}