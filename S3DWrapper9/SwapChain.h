/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseSwapChain.h"

class CStereoRenderer;

class CSwapChain : public CBaseSwapChain
{
public:
	CStereoRenderer*	GetRendD3D9Device() { return (CStereoRenderer*)m_pD3D9Device; };
	CComPtr<IDirect3DSurface9>	m_pStereoBBLeft;
	CComPtr<IDirect3DSurface9>	m_pStereoBBRight;

public:
	CSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index);
	~CSwapChain(void);
	virtual HRESULT InitializeMode(D3DSURFACE_DESC &RenderTargetDesc);
	virtual void	Clear();
	virtual void	CheckDepthBuffer();
	friend CStereoRenderer;
};
