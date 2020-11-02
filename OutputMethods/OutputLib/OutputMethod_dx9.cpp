/* IZ3D_FILE: $Id: OutputMethod.cpp 1558 2008-12-26 17:36:36Z kostik $ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author: kostik $
* $Revision: 1558 $
* $Date: 2008-12-26 20:36:36 +0300 (Пт, 26 дек 2008) $
* $LastChangedBy: kostik $
* $URL: svn://svn.neurok.ru/dev/driver/trunk/S3DWrapper9/OutputMethod.cpp $
*/
#include "StdAfx.h"
#include "OutputMethod_dx9.h"
#include "CommonUtils\ResourceLoader.h"

using namespace DX9Output;

#pragma warning( disable : 4100 )

OutputMethod::OutputMethod(DWORD mode, DWORD spanMode)
: m_bProcessGammaRamp(false)
, m_OutputMode(mode)
, m_SpanMode(spanMode)
, m_Caps(0)
{
	m_SubMode = 0;
	m_bTrialMode = (GetTrialDaysLeft() >= 0);
}

OutputMethod::~OutputMethod(void)
{
	Clear();
	m_pd3dDevice.Release(); 
}

HRESULT OutputMethod::Initialize( IDirect3DDevice9* dev, bool MultiPass /*= false*/ )
{
	m_pd3dDevice = dev;
	return S_OK;
}

HRESULT OutputMethod::InitializeSCData( CBaseSwapChain* pSwapChain )
{
	return S_OK;
}

void OutputMethod::Clear()
{
	m_pLogo.Release();
}

void OutputMethod::ModifyPresentParameters(IDirect3D9* pd3d, UINT nAdapter, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (!m_SpanMode)
		return;

	if (pPresentationParameters[0].Windowed)
		pPresentationParameters[0].BackBufferWidth *= 2;
	else
	{
		bool found = false;
		D3DFORMAT fmt = pPresentationParameters[0].BackBufferFormat;
		if (pPresentationParameters[0].BackBufferFormat == D3DFMT_A8R8G8B8)
			fmt = D3DFMT_X8R8G8B8;
		D3DDISPLAYMODE mode;
		DWORD mc = pd3d->GetAdapterModeCount(nAdapter, fmt);
		for (DWORD i = 0; i < mc; i++)
		{
			pd3d->EnumAdapterModes(nAdapter, fmt, i, &mode);
			if (pPresentationParameters[0].BackBufferHeight == mode.Height && pPresentationParameters[0].BackBufferWidth * 2 == mode.Width)
			{
				found = true;
				pPresentationParameters[0].BackBufferWidth *= 2;
				pPresentationParameters[0].BackBufferFormat = fmt;
				break;
			}
		}
		//#ifdef FINAL_RELEASE
		if(!found)
			m_SpanMode = false;
		//#endif
	}
}

bool OutputMethod::FindAdapter( LPDIRECT3D9 pDirect3D9, UINT DeviceNum, D3DDEVTYPE &DeviceType, UINT Adapters[2] )
{
	if (GetOutputChainsNumber() == 1)
	{
		// PerfHUD not support SwapChains
#ifndef FINAL_RELEASE
		// Look for 'NVIDIA PerfHUD' adapter
		// If it is present, override default settings
		for (UINT Adapter = 0; Adapter < pDirect3D9->GetAdapterCount(); Adapter++)
		{
			D3DADAPTER_IDENTIFIER9 Identifier;
			HRESULT Res = pDirect3D9->GetAdapterIdentifier(Adapter, 0, &Identifier);
			if (strstr(Identifier.Description, "PerfHUD") != 0)
			{
				Adapters[0] = Adapter;
				DeviceType = D3DDEVTYPE_REF;
				break;
			}
		}
#endif
		return true;
	}
	else
	{
		D3DCAPS9 D3DCaps;
		//--- search second adapter ordinal for current master adapter ---
		for(UINT i=0; i< pDirect3D9->GetAdapterCount(); i++)
		{
			pDirect3D9->GetDeviceCaps(i, DeviceType, &D3DCaps);
			if(D3DCaps.MasterAdapterOrdinal == Adapters[0] && D3DCaps.AdapterOrdinalInGroup == 1)
			{
				Adapters[1] = i;
				return true;
			}
		}
		return false;
	}
}

int OutputMethod::GetTrialDaysLeft()
{
	int DaysLeft = -1; 
	TCHAR TempBuffer[100];
	if (GetEnvironmentVariable(_T("PCGI_ApplicationStatus"), TempBuffer, 100))
	{
		UINT ApplicationStatus = 1;
		_stscanf_s(TempBuffer, _T("%d"), &ApplicationStatus);
		if (ApplicationStatus == 0 && GetEnvironmentVariable(_T("PCGI_DemoModeActive"), TempBuffer, 100))
		{
			BOOL DemoActive = FALSE;
			_stscanf_s(TempBuffer, _T("%d"), &DemoActive);
			if (DemoActive && GetEnvironmentVariable(_T("PCGI_DemoDaysLeft"), TempBuffer, 100))
				_stscanf_s(TempBuffer, _T("%d"), &DaysLeft);
		}
	}
	return DaysLeft;
}
