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
// DualProjectionOutput.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"

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
	return new DualProjectionOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return 0;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Dual Projector");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Default");
		return TRUE;
	default:
		return FALSE;
	}
}

DualProjectionOutput::DualProjectionOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
	m_Caps = ocRenderDirectlyToBB;
}

DualProjectionOutput::~DualProjectionOutput(void)
{
}

HRESULT DualProjectionOutput::Output(CBaseSwapChain* pSwapChain)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* left = pSwapChain->GetLeftBackBufferRT();
	IDirect3DSurface9* right = pSwapChain->GetRightBackBufferRT();
	RECT* pLeftRect = pSwapChain->GetLeftBackBufferRect();
	RECT* pRightRect = pSwapChain->GetRightBackBufferRect();
	IDirect3DSurface9* primary = pSwapChain->m_pPrimaryBackBuffer;
	IDirect3DSurface9* secondary = pSwapChain->m_pSecondaryBackBuffer;
	if (left != primary)
	{
		NSCALL(m_pd3dDevice->StretchRect(left,
			pLeftRect, primary, NULL, D3DTEXF_NONE));
	}
	if (secondary && right != secondary)
	{
		NSCALL(m_pd3dDevice->StretchRect(right,
			pRightRect, secondary, NULL, D3DTEXF_NONE));
	}
	return hResult;
}
