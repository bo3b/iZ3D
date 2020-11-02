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
#include "stdafx.h"
#include <MadCHook.h>
#include "Hook.h"
#include "WDirect3DCreate9.h"
#include "BaseStereoRenderer.h"

RenderDLLInfo			g_RenderInfo;
AddressSetMap			g_UniqueHooks;
DirectWrapperClass		g_pDirectWrapperList;
SwapChainWrapperClass	g_pSwapChainWrapperList;
RendererListClass		g_pRendererList;
#ifdef _DEBUG
volatile long			ProxyGuard::callLevel = 0;
#endif

RenderDLLInfo::RenderDLLInfo()
{
	m_D3D9StartAddress = 0;
	m_Wrapper9StartAddress = 0;
	m_Wrapper9OutputStartAddress = 0;
	m_D3D9EndAddress = -1;
	m_Wrapper9EndAddress = -1; 
	m_Wrapper9OutputEndAddress = 0; // should be 0
}

void RenderDLLInfo::OneTimeInitialize()
{
	//--- determine dll address range for D3D9.dll and S3DWrapperD3D9.dll ---
	initializeAddress(GetD3D9HMODULE(), m_D3D9StartAddress, m_D3D9EndAddress); 
	initializeAddress(gData.hModule, m_Wrapper9StartAddress, m_Wrapper9EndAddress); 
}

void RenderDLLInfo::initializeAddress(HMODULE hDLL, LONG_PTR& start, LONG_PTR& end)
{
	MODULEINFO	mInfo;
	HANDLE hProcess = GetCurrentProcess();
	if(GetModuleInformation(hProcess, hDLL, &mInfo, sizeof(MODULEINFO)))
	{
		start = (LONG_PTR&)mInfo.lpBaseOfDll;
		end   = (LONG_PTR&)mInfo.lpBaseOfDll + mInfo.SizeOfImage;
	}
}

DWORD __declspec(dllexport) WINAPI InitializeExchangeServer()
{
	switch(gInfo.RouterType)
	{
	case 0:
	case 1: //--- RouterType = 1 not supported now ---
		{
			//--- Injector will return object, created by WDirect3DCreate9 ---
			DEBUG_MESSAGE(_T("Creating IDirect3D9 wrapper interface.\n"));
			break;
		}
	default:
		DEBUG_MESSAGE(_T("Creating original D3D9 interface.\n"));
		break;
	}
	return gInfo.RouterType; 
}
