/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <d3d9.h>
#include <d3dx9tex.h>
#include "..\CommonUtils\System.h"
#include "..\S3DAPI\ImageProcessing.h"

class CBaseSwapChain;

//--- must be one object per swapchain ---
class ShiftFinder9 : public ShiftFinder
{
public:
	~ShiftFinder9() { Clear(); }

	HRESULT Initialize(IDirect3DDevice9* pDevice, UINT dwCreateDeviceFlags, RECT* pLeftRect, RECT* pRightRect);
	void	Clear();

	void	FindShift(CBaseSwapChain* pSwapChain);

protected:
	virtual bool GetRTData();

	CComPtr<IDirect3DDevice9>	m_pDevice;
	CComPtr<IDirect3DSurface9>	m_pPool;
	CComPtr<IDirect3DSurface9>	m_pMemoryBuffer;

	//--- view selection rect's ---
	RECT m_dstRectLeft, m_dstRectRight;
};
