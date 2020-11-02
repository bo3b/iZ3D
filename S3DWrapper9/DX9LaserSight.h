/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "VertexType.h"
#include "..\S3DAPI\LaserSightData.h"

class CBaseSwapChain;

class DX9LaserSight
{
public:
	DX9LaserSight(void);
	~DX9LaserSight(void) { Clear(); }

	HRESULT Initialize(IDirect3DDevice9* pDevice);
	void	Clear();

	void	Draw(CBaseSwapChain* pSwapChain, float frameTimeDelta);
	SIZE	GetTextureSize();

protected:

	CComPtr<IDirect3DDevice9>		m_pDevice;
	CComPtr<IDirect3DStateBlock9>	m_pDrawStates;
	CComPtr<IDirect3DTexture9>		m_pSightTexture;
};
