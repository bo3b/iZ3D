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
#include "IWear\IWRsdk.h"

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
	return new VR920Output(mode, spanMode);
}

OUTPUT_API DWORD CALLBACK GetOutputCaps()
{
	return odShutterModeSimpleOnly;
}

OUTPUT_API void CALLBACK GetOutputName(char* name, DWORD size)
{
	strcpy_s(name, size, "VR920");
}

OUTPUT_API BOOL CALLBACK EnumOutputModes(DWORD num, char* name, DWORD size)
{
	switch(num)
	{
	case 0:
		strcpy_s(name, size, "Default");
		return TRUE;
	case 1:
		strcpy_s(name, size, "W/O Flush");
		return TRUE;
	default:
		return FALSE;
	}
}

VR920Output::VR920Output(DWORD mode, DWORD spanMode)
: OutputMethod(mode, spanMode)
{
	IWRLoadDll();
	m_bAdapterFound = false;
	m_StereoHandle = NULL;
	m_pLeftEyeQuery = NULL;
	m_pRightEyeQuery = NULL;
	m_bLastFrameStereoOn = false;
}

VR920Output::~VR920Output(void)
{
	if (m_StereoHandle && IWRSTEREO_Close)
	{
		IWRSTEREO_SetStereo( m_StereoHandle, false );
		IWRSTEREO_Close(m_StereoHandle);
		m_StereoHandle = NULL;
	}
	IWRFreeDll();
}

HRESULT VR920Output::Initialize(IDirect3DDevice9* dev, bool MultiPass)
{
	HRESULT res = S_OK;
	Clear();
	m_pd3dDevice = dev;
	if (m_bAdapterFound && IWRSTEREO_Open)
	{
		m_StereoHandle = IWRSTEREO_Open();
	}
	if (m_StereoHandle)
	{
		IWRSTEREO_SetStereo( m_StereoHandle, false );
	}
	if (m_OutputMode == 0)
	{
		res = m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_pLeftEyeQuery);
		res = m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &m_pRightEyeQuery);
	}
	return res;
}

void VR920Output::Clear()
{
	m_pLeftEyeQuery.Release(); 
	m_pRightEyeQuery.Release();
	m_pd3dDevice.Release(); 
}

void VR920Output::StereoModeChanged(bool bNewMode)
{
	if (m_StereoHandle)
	{
		IWRSTEREO_SetStereo(m_StereoHandle, bNewMode);
		m_bLastFrameStereoOn = bNewMode;
	}
}

enum { LEFT_EYE=0, RIGHT_EYE, MONO_EYES };

HRESULT VR920Output::Output(bool bLeft, CBaseSwapChain* pSwapChain, RECT* pDestRect)
{
	HRESULT hResult = S_OK;
	IDirect3DSurface9* view = pSwapChain->GetViewRT(bLeft);
	RECT* pViewRect = pSwapChain->GetViewRect(bLeft);
	IDirect3DSurface9* primary = pSwapChain->GetPresenterBackBuffer();

	NSCALL(m_pd3dDevice->StretchRect(view, pViewRect, primary, pDestRect, D3DTEXF_NONE));
	if (m_bLastFrameStereoOn && m_StereoHandle)
		IWRSTEREO_WaitForAck( m_StereoHandle, bLeft ? LEFT_EYE : RIGHT_EYE );
	return hResult;
}

void VR920Output::AfterPresent( bool bLeft )
{
	if (!m_bLastFrameStereoOn)
		return;

	LPDIRECT3DQUERY9 pEyeQuery = bLeft ? m_pLeftEyeQuery : m_pRightEyeQuery;
	if (pEyeQuery)
	{
		pEyeQuery->Issue(D3DISSUE_END);
		while(S_FALSE == pEyeQuery->GetData( NULL, 0, D3DGETDATA_FLUSH )) ;
	}
	if (m_StereoHandle)
		IWRSTEREO_SetLR( m_StereoHandle, bLeft ? LEFT_EYE : RIGHT_EYE );
}

bool VR920Output::FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] )
{
	OutputMethod::FindAdapter(pDirect3D9, DeviceNum, DeviceType, Adapters);

	// Find the DirectX method of adapter ordinal, that is the VR920's
	UINT AdapterCount = pDirect3D9->GetAdapterCount();
	DISPLAY_DEVICEA			MonitorDevice;
	MonitorDevice.cb = sizeof( DISPLAY_DEVICEA );
	for( UINT i = 0; i < AdapterCount; i++ ) {
		D3DADAPTER_IDENTIFIER9	Adapter_Identify;
		pDirect3D9->GetAdapterIdentifier( i, 0, &Adapter_Identify );
		for( int j=0; EnumDisplayDevicesA(Adapter_Identify.DeviceName, j, &MonitorDevice, 0x0 ); j++ ) {
			if( (MonitorDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
				strstr( MonitorDevice.DeviceID, "IWR0002" ) ) {
					// Found the VR920 PnP id.
					Adapters[0] = i;
					m_bAdapterFound = true;
					return true;
			}
		}
	}
#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif
}
