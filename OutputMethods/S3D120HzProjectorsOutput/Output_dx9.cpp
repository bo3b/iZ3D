/* IZ3D_FILE: $Id: Output_dx9.cpp$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
*/
// S3D120HzProjectorsOutput.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"
#include <HighLevelMonitorConfigurationAPI.h>
#include "MonitorsPNPID.h"

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
		DisableThreadLibraryCalls(hModule);
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
	return new S3D120HzProjectorsOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterMode; // Use Presenter
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "3D 120Hz Projectors");
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

S3D120HzProjectorsOutput::S3D120HzProjectorsOutput( DWORD mode, DWORD spanMode )
:	OutputMethod(mode, spanMode)
{
}

S3D120HzProjectorsOutput::~S3D120HzProjectorsOutput	()
{
}

HRESULT S3D120HzProjectorsOutput::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();

	/*D3DCOLOR lineColor;
	if (m_OutputMode == 1 || m_OutputMode == 2) {
		if (m_OutputMode == 1)
			lineColor = D3DCOLOR_XRGB(0x00, 0x00, 0xFF); // blue
		else
			lineColor = D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF); // white
		
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
		//--- Color fill the original view, due to ATI stereo presenter ---
		NSCALL(m_pd3dDevice->ColorFill(view, &rect, lineColor));
		rect.left	= bbSize.cx / 4 * (bLeft ? 1 : 3);
		rect.right	= bbSize.cx;
		if (pDestRect)
		{
			rect.left	+= pDestRect->left;
			rect.right	+= pDestRect->left;
		}
		//--- Color fill the original view, due to ATI stereo presenter ---
		NSCALL(m_pd3dDevice->ColorFill(view, &rect, D3DCOLOR_XRGB(0x00, 0x00, 0x00)));
	}*/

	NSCALL(m_pd3dDevice->StretchRect( view, pViewRect, primary, pDestRect, D3DTEXF_NONE ));
	return	hResult;
}

bool S3D120HzProjectorsOutput::FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] )
{
	OutputMethod::FindAdapter(pDirect3D9, DeviceNum, DeviceType, Adapters);
	return true;
}
