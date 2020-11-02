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
// Z800Output.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Output_dx9.h"
#include "S3DWrapper9\BaseSwapChain.h"
#define  EMA_STATIC_LINK 
#include "EMADevice.h"

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
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
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
	return new Z800Output(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterModeSimpleOnly;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "Z800");
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

Z800Output::Z800Output(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
	m_pZ800 = NULL; 
	m_dwFrameCount = 0;
	m_bAdapterFound = false;
	m_bLastFrameStereoOn = true;
}

Z800Output::~Z800Output(void)
{
	if (m_pZ800)
	{
		// Turn off the headset
		HRESULT hr = m_pZ800->GoSleep ( ) ; 
		// destroy device 
		delete m_pZ800; 
	}
}

HRESULT Z800Output::Initialize(IDirect3DDevice9* dev, bool MultiPass)
{
	HRESULT res = S_OK;
	Clear();
	m_pd3dDevice = dev;
	if (m_pZ800)
	{
		HRESULT hr = m_pZ800->EnableStereoVision ( false ) ;
	}
	return res;
}

void Z800Output::Clear()
{
	m_pd3dDevice.Release(); 
}

void Z800Output::StereoModeChanged(bool bNewMode)
{
	HRESULT hr = m_pZ800->EnableStereoVision ( bNewMode ) ;
	m_bLastFrameStereoOn = bNewMode;
}

enum { LEFT_EYE=0, RIGHT_EYE, MONO_EYES };

HRESULT Z800Output::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();
	if (m_pZ800)
	{
		++m_dwFrameCount ; 
		if ( 0 == ( m_dwFrameCount % 20000 ) ) 
			m_pZ800->KeepAlive ( ) ;
	}

	NSCALL(m_pd3dDevice->StretchRect(view, pViewRect, primary, pDestRect, D3DTEXF_NONE));
	return hResult;
}

bool Z800Output::FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] )
{
	OutputMethod::FindAdapter(pDirect3D9, DeviceNum, DeviceType, Adapters);

	m_pZ800 = new EMADevice ( EMA_NO_HEADTRACKING ) ;
	if ( m_pZ800 ) 
	{ 
		// Turn on the headset if it was sleeping 
		HRESULT hr = m_pZ800->KeepAlive ( ) ; 

		m_bAdapterFound = true;
		return true;
	}

#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}
