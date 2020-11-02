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
	return new ShutterOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterModeSimpleOnly;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Shutter mode");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Default");
		return TRUE;
	case 1:
		strcpy_s(name, size, "Blue line coded");
		return TRUE;
	case 2:
		strcpy_s(name, size, "White line coded");
		return TRUE;
	default:
		return FALSE;
	}
}

ShutterOutput::ShutterOutput(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
}

ShutterOutput::~ShutterOutput(void)
{
}

HRESULT ShutterOutput::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();

	NSCALL(m_pd3dDevice->StretchRect(view, pViewRect, primary, pDestRect, D3DTEXF_NONE));
	D3DCOLOR lineColor;
	if (m_OutputMode == 1 || m_OutputMode == 2) {
		if(pSwapChain->IsStereoActive())
		{	if (m_OutputMode == 1)
				lineColor = D3DCOLOR_XRGB(0x00, 0x00, 0xFF); // blue
			else
				lineColor = D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF); // white
		}
		else
			lineColor = D3DCOLOR_XRGB(0x00, 0x00, 0x00);	 // black for switchig glasses in mono

		SIZE bbSize = pSwapChain->m_BackBufferSize;
		RECT rect;
		rect.top	= bbSize.cy - 1;
		rect.bottom	= bbSize.cy;
		rect.left	= 0;
		rect.right	= bbSize.cx / 4 * (bLeft ? 1 : 3);
		if (pDestRect)
		{
			rect.left	+= pDestRect->left;
			rect.top	+= pDestRect->top;
			rect.right	+= pDestRect->left;
			rect.bottom	+= pDestRect->top;
		}
		NSCALL(m_pd3dDevice->ColorFill(primary, &rect, lineColor));
		rect.left	= bbSize.cx / 4 * (bLeft ? 1 : 3);
		rect.right	= bbSize.cx;
		if (pDestRect)
		{
			rect.left	+= pDestRect->left;
			rect.right	+= pDestRect->left;
		}
		NSCALL(m_pd3dDevice->ColorFill(primary, &rect, D3DCOLOR_XRGB(0x00, 0x00, 0x00)));
	}
	return hResult;
}
