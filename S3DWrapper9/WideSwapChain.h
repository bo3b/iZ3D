/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "BaseSwapChain.h"

class CWideStereoRenderer;

class CWideSwapChain : public CBaseSwapChain
{
public:
	CWideStereoRenderer*		GetWideD3D9Device() { return (CWideStereoRenderer*)m_pD3D9Device; };

	CComPtr<IDirect3DSurface9>	m_pWidePrimaryBackBufferBeforeScaling;
	CComPtr<IDirect3DSurface9>	m_pWidePrimaryBackBuffer;
	CComPtr<IDirect3DSurface9>	m_pWidePrimaryDepthStencil;

public:
	CWideSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index);
	~CWideSwapChain(void);
	virtual HRESULT InitializeMode(D3DSURFACE_DESC &RenderTargetDesc);
	virtual void	Clear();
	virtual void	CheckDepthBuffer();
	friend CWideStereoRenderer;
};

// {0CC0C043-2083-48e8-B51C-4A3F6DA06743}
static const GUID DebugTexture_GUID = 
{ 0xcc0c043, 0x2083, 0x48e8, { 0xb5, 0x1c, 0x4a, 0x3f, 0x6d, 0xa0, 0x67, 0x43 } };
