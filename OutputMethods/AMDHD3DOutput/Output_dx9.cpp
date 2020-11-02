/* IZ3D_FILE: $Id: Output_dx9.cpp$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
*/
// CMOShutterOutput.cpp : Defines the entry point for the DLL application.
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
	return new AMDHD3DOutput(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterMode | odShutterModeAMDOnly; // Use Presenter
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "AMD HD3D Output");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "120Hz");
	default:
		return FALSE;
	}
}

AMDHD3DOutput::AMDHD3DOutput( DWORD mode, DWORD spanMode )
:	OutputMethod(mode, spanMode)
{
}

AMDHD3DOutput::~AMDHD3DOutput	()
{
}

int AMDHD3DOutput::GetPreferredRefreshRate() const
{
	return 0;
}

HRESULT AMDHD3DOutput::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();
	NSCALL(m_pd3dDevice->StretchRect( view, pViewRect, primary, pDestRect, D3DTEXF_NONE ));
	return	hResult;
}
