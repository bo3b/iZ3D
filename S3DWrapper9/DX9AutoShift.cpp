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
#include <emmintrin.h>
#include <intrin.h>
#include <Shlobj.h>
#include "ProductNames.h"
#include "DX9AutoShift.h"
#include "BaseSwapChain.h"
#include "CommandDumper.h"

void ShiftFinder9::Clear()
{
	ShiftFinder::Clear();

	m_pMemoryBuffer = NULL;
	m_pPool = NULL;
	m_pDevice = NULL;
}

HRESULT ShiftFinder9::Initialize(IDirect3DDevice9* pDevice, UINT dwCreateDeviceFlags, RECT* pLeftRect, RECT* pRightRect)
{
	if (m_bInitializationSuccessful)
		return S_OK;

	ShiftFinder::Initialize(pLeftRect, pRightRect);
	if(m_bInitializationSuccessful)
	{
		m_pDevice = pDevice;
		
		m_bDeviceIsMultithreaded = false;
		if(dwCreateDeviceFlags & D3DCREATE_MULTITHREADED)
			m_bDeviceIsMultithreaded = true;

		SetRect(&m_dstRectLeft,  0, 0, m_FrameSizeX, m_FrameSizeY);
		SetRect(&m_dstRectRight, 0, m_FrameSizeY, m_SearchAreaSizeX, 2 * m_FrameSizeY);

		HRESULT hResult;
		if(FAILED(hResult = pDevice->CreateRenderTarget(m_SearchAreaSizeX, 2 * m_FrameSizeY, D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pPool, 0)))
		{
			m_bInitializationSuccessful = false;
			return hResult;
		}
		if(FAILED(hResult = pDevice->CreateOffscreenPlainSurface(m_SearchAreaSizeX, 2 * m_FrameSizeY, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pMemoryBuffer, 0)))
		{
			m_bInitializationSuccessful = false;
			return hResult;
		}
		return S_OK;
	}
	return E_FAIL;
}

void ShiftFinder9::FindShift(CBaseSwapChain* pSwapChain)
{
	INSIDE;
	if(m_bInitializationSuccessful)
	{
		if(m_bThreadReady)
		{
			LPDIRECT3DSURFACE9 left = pSwapChain->m_pLeftBackBuffer;
			LPDIRECT3DSURFACE9 right = pSwapChain->m_pRightBackBuffer;
			UINT currentFrame = pSwapChain->m_nPresentCounter;
			if(FAILED(m_pDevice->StretchRect(left,  &m_srcRectLeft,  m_pPool, &m_dstRectLeft,  D3DTEXF_LINEAR)))
			{
				DEBUG_MESSAGE(_T("Autofocus StretchRect() for left view failed.\n"));
				return;
			}
			if(FAILED(m_pDevice->StretchRect(right, &m_srcRectRight, m_pPool, &m_dstRectRight, D3DTEXF_LINEAR)))
			{
				DEBUG_MESSAGE(_T("Autofocus StretchRect() for right view failed.\n"));
				return;
			}

			//----------------------------- just for debugging ------------------------------------
			//     D3DXSaveSurfaceToFile("SelectedLines.bmp", D3DXIFF_BMP, m_pPool, NULL, NULL);
			//-------------------------------------------------------------------------------------

			ShiftFinder::FindShift(currentFrame);
		}
	}
}

bool ShiftFinder9::GetRTData()
{
	INSIDE;
	HRESULT hResult = S_OK;
	if(SUCCEEDED(m_pDevice->GetRenderTargetData(m_pPool, m_pMemoryBuffer)))
	{
		D3DLOCKED_RECT lockedRect;
		while ((hResult = m_pMemoryBuffer->LockRect(&lockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_DONOTWAIT)) == D3DERR_WASSTILLDRAWING)
		{
			Sleep(0);
		}
		BYTE* srcL = (BYTE*)lockedRect.pBits;
		BYTE* srcR = (BYTE*)lockedRect.pBits + m_FrameSizeY * lockedRect.Pitch;
		convert_X8R8G8B8_ToMonochrome(srcL, lockedRect.Pitch, m_ImageLeft,  m_FrameSizeX);
		convert_X8R8G8B8_ToMonochrome(srcR, lockedRect.Pitch, m_ImageRight, m_SearchAreaSizeX);
		m_pMemoryBuffer->UnlockRect();
		return true;
	}
	return false;
}

