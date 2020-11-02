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
// Direct3D8.cpp : Implementation of CDx8wrpApp and DLL registration.

#include "stdafx.h"
#include "d3d9_8.h"
#include "Direct3D8.h"
#include "Direct3DDevice8.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////

CDirect3D8::CDirect3D8()
: m_Adapter(-1)
{

}

STDMETHODIMP  CDirect3D8::RegisterSoftwareDevice(void* pInitializeFunction)
{
	DEBUG_TRACE3("RegisterSoftwareDevice(pInitializeFunction = %p)\n", pInitializeFunction);
	return m_pReal->RegisterSoftwareDevice(pInitializeFunction);
}

STDMETHODIMP_(UINT) CDirect3D8::GetAdapterCount()
{
	DEBUG_TRACE3("GetAdapterCount()\n");
	return m_pReal->GetAdapterCount();
}

STDMETHODIMP  CDirect3D8::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER8* pIdentifier)
{
	DEBUG_TRACE3("GetAdapterIdentifier(Adapter = %d, Flags = %d, pIdentifier = %p)\n", Adapter, Flags, pIdentifier);
	D3DADAPTER_IDENTIFIER9 ai;
	HRESULT hr;
	if(!pIdentifier) return E_POINTER;
	hr=m_pReal->GetAdapterIdentifier(Adapter,Flags & ~D3DENUM_WHQL_LEVEL,&ai);
	if(SUCCEEDED(hr))
	{
		strcpy(pIdentifier->Driver, ai.Driver);
		strcpy(pIdentifier->Description, ai.Description);
		pIdentifier->DriverVersion = ai.DriverVersion;
		pIdentifier->VendorId = ai.VendorId;
		pIdentifier->DeviceId = ai.DeviceId;
		pIdentifier->SubSysId = ai.SubSysId;
		pIdentifier->Revision = ai.Revision;
		pIdentifier->DeviceIdentifier = ai.DeviceIdentifier;
		pIdentifier->WHQLLevel = ai.WHQLLevel;
	}
	return hr;
}

static D3DFORMAT adapterFmts[] = 
{
	D3DFMT_R5G6B5,		D3DFMT_X1R5G5B5,	
	D3DFMT_X8R8G8B8
};

STDMETHODIMP_(UINT) CDirect3D8::GetAdapterModeCount(UINT Adapter)
{
	DEBUG_TRACE3("GetAdapterModeCount(Adapter = %d)\n", Adapter);
	UINT res = 0;
	for (int i = 0; i < _countof(adapterFmts); i++)
	{
		UINT cnt = m_pReal->GetAdapterModeCount(Adapter, adapterFmts[i]);
		DEBUG_TRACE3("\t%s: %d\n", GetFormatString(adapterFmts[i]), cnt);
		res += cnt;
	}
	return res;
}

void CDirect3D8::BuildAdaperModesList( int adapter )
{
	if (adapter != m_Adapter)
	{
		m_Adapter = adapter;
		m_AdapterModes.clear();

		D3DDISPLAYMODE d3ddm;
		int ci[3] = { 0, 0, 0 };
		D3DDISPLAYMODE curdm;
		HRESULT hr = m_pReal->EnumAdapterModes(adapter, adapterFmts[0], ci[0], &curdm);
		int added;
		do
		{
			added = 0;
			D3DDISPLAYMODE newdm;
			for (int i = 0; i < _countof(adapterFmts); i++)
			{
				while (true) 
				{
					hr = m_pReal->EnumAdapterModes(adapter, adapterFmts[i], ci[i], &d3ddm);
					if (SUCCEEDED(hr))
					{
						if (d3ddm.Width == curdm.Width && d3ddm.Height == curdm.Height)
						{
							m_AdapterModes.push_back(d3ddm);
							ci[i]++;
							added++;
						}
						else
						{
							newdm = d3ddm;
							break;
						}
					}
					else
						break;
				};
			}
			curdm = newdm;
		}
		while(added != 0);
	}
}

STDMETHODIMP  CDirect3D8::EnumAdapterModes(UINT Adapter,UINT Mode,D3DDISPLAYMODE* pMode)
{
	DEBUG_TRACE3("EnumAdapterModes(Adapter = %d, Mode = %d, pMode = %p)\n", Adapter, Mode, pMode);
	BuildAdaperModesList((int)Adapter);
	if (Mode < m_AdapterModes.size())
	{
		*pMode = m_AdapterModes[Mode];
		DEBUG_TRACE3("\t%s\n", GetDisplayModeString(pMode));
		return D3D_OK;
	}
	else
		return D3DERR_NOTAVAILABLE;
}

STDMETHODIMP  CDirect3D8::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE* pMode)
{
	DEBUG_TRACE3("GetAdapterDisplayMode(Adapter = %d, pMode = %p)\n", Adapter, pMode);
	HRESULT hr = m_pReal->GetAdapterDisplayMode(Adapter,pMode);
	DEBUG_TRACE3("\t%s\n", GetDisplayModeString(pMode));
	return hr;
}

STDMETHODIMP  CDirect3D8::CheckDeviceType(UINT Adapter,D3DDEVTYPE CheckType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{
	DEBUG_TRACE3("CheckDeviceType(Adapter = %d, CheckType = %d, DisplayFormat = %s, BackBufferFormat = %s, Windowed = %d)\n", Adapter, CheckType, 
		GetFormatString(DisplayFormat), GetFormatString(BackBufferFormat), Windowed);
	return m_pReal->CheckDeviceType(Adapter,CheckType,DisplayFormat,BackBufferFormat,Windowed);
}

STDMETHODIMP  CDirect3D8::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	DEBUG_TRACE3("CheckDeviceFormat(Adapter = %d, DeviceType = %d, AdapterFormat = %s, Usage = %d, RType = %d, CheckFormat = %s)\n", Adapter, DeviceType, 
		GetFormatString(AdapterFormat), Usage, RType, GetFormatString(CheckFormat));
	HRESULT hr = m_pReal->CheckDeviceFormat(Adapter,DeviceType,AdapterFormat,Usage,RType,CheckFormat);
	return hr;
}

STDMETHODIMP  CDirect3D8::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType)
{
	DEBUG_TRACE3("CheckDeviceMultiSampleType(Adapter = %d, DeviceType = %d, SurfaceFormat = %s, Windowed = %d, MultiSampleType = %d)\n", Adapter, DeviceType, 
		GetFormatString(SurfaceFormat), Windowed, MultiSampleType);
	return m_pReal->CheckDeviceMultiSampleType(Adapter,DeviceType,SurfaceFormat,Windowed,MultiSampleType,NULL);
}

STDMETHODIMP  CDirect3D8::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	DEBUG_TRACE3("CheckDepthStencilMatch(Adapter = %d, DeviceType = %d, AdapterFormat = %s, RenderTargetFormat = %s, DepthStencilFormat = %s)\n", Adapter, DeviceType, 
		GetFormatString(AdapterFormat), GetFormatString(RenderTargetFormat), GetFormatString(DepthStencilFormat));
	return m_pReal->CheckDepthStencilMatch(Adapter,DeviceType,AdapterFormat,RenderTargetFormat,DepthStencilFormat);
}

STDMETHODIMP  CDirect3D8::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS8* pCaps)
{
	DEBUG_TRACE3("GetDeviceCaps(Adapter = %d, DeviceType = %d, pCaps = %p)\n", Adapter, DeviceType, pCaps);
	D3DCAPS9 caps9;
	HRESULT hr;
	hr=m_pReal->GetDeviceCaps(Adapter,DeviceType,&caps9);
	if(SUCCEEDED(hr))
		ConvertCaps(caps9, pCaps);
	return hr;
}

STDMETHODIMP_(HMONITOR) CDirect3D8::GetAdapterMonitor(UINT Adapter)
{
	DEBUG_TRACE3("GetAdapterMonitor(Adapter = %d)\n", Adapter);
	return m_pReal->GetAdapterMonitor(Adapter);
}

STDMETHODIMP  CDirect3D8::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS8* pPresentationParameters,IDirect3DDevice8** ppReturnedDeviceInterface)
{
	HRESULT hr;
	if (ppReturnedDeviceInterface == NULL)
	{
		return E_POINTER;
	}
	CComPtr<IDirect3DDevice9> pDev9;
	D3DPRESENT_PARAMETERS pp9;
	ConvertPresentParameters(pPresentationParameters,&pp9);

	hr = m_pReal->CreateDevice(Adapter,DeviceType,hFocusWindow,BehaviorFlags,&pp9,&pDev9);
	DEBUG_TRACE1("CDirect3D8::CreateDevice(Adapter=%d, DeviceType=%d,  hFocusWindow=%d, BehaviorFlags=%d, ppReturnedDeviceInterface=%d)=%s\n", 
		Adapter, DeviceType, hFocusWindow, BehaviorFlags,  ppReturnedDeviceInterface, GetResultString(hr));

	if (FAILED(hr))
		DEBUG_FAILED(hr)
	else if (pDev9)
	{
		CreateWrapper(pDev9.p, ppReturnedDeviceInterface);
		CDirect3DDevice8* pDev8Wrap = (CDirect3DDevice8*)*ppReturnedDeviceInterface;
		pDev8Wrap->Init(this, Adapter, DeviceType, pp9.BackBufferFormat);
		pDev8Wrap->CreateMcClaud();
	}
	return hr;
}
