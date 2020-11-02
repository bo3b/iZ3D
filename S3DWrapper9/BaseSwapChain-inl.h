/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

inline ProxyDevice9& CBaseSwapChain::GetD3D9Device()
{
	return GetBaseDevice()->m_Direct3DDevice;
}

inline bool CBaseSwapChain::IsStereoActive()
{
	return (GetBaseDevice()->m_Input.StereoActive != 0);
}

inline bool CBaseSwapChain::IsSwapEyes()
{
	return (GetBaseDevice()->m_Input.SwapEyes != 0);
}

inline IDirect3DSurface9* CBaseSwapChain::GetLeftBackBufferRT()
{
	if (!IsStereoActive() || !IsSwapEyes())
		return m_pLeftBackBuffer;
	else
		return m_pRightBackBuffer;
}

inline IDirect3DTexture9* CBaseSwapChain::GetLeftBackBufferTex()
{
	if (!IsStereoActive() || !IsSwapEyes())
		return m_pLeftMethodTexture;
	else
		return m_pRightMethodTexture;
}

inline IDirect3DTexture9* CBaseSwapChain::GetLeftDepthStencilTex()
{
	if (!IsStereoActive() || !IsSwapEyes())
		return m_pLeftDepthStencilCorrectTexture;
	else
		return m_pRightDepthStencilCorrectTexture;
}

inline RECT* CBaseSwapChain::GetLeftBackBufferRect()
{
	if (!IsStereoActive() || !IsSwapEyes())
		return &m_LeftViewRect;
	else
		return &m_RightViewRect;
}

inline IDirect3DSurface9* CBaseSwapChain::GetRightBackBufferRT()
{
	if (IsStereoActive() && !IsSwapEyes())
		return m_pRightBackBuffer;
	else
		return m_pLeftBackBuffer;
}

inline IDirect3DTexture9* CBaseSwapChain::GetRightBackBufferTex()
{
	if (IsStereoActive() && !IsSwapEyes())
		return m_pRightMethodTexture;
	else
		return m_pLeftMethodTexture;
}

inline IDirect3DTexture9* CBaseSwapChain::GetRightDepthStencilTex()
{
	if (IsStereoActive() && !IsSwapEyes())
		return m_pRightDepthStencilCorrectTexture;
	else
		return m_pLeftDepthStencilCorrectTexture;
}

inline RECT* CBaseSwapChain::GetRightBackBufferRect()
{
	if (IsStereoActive() && !IsSwapEyes())
		return &m_RightViewRect;
	else
		return &m_LeftViewRect;
}

inline IDirect3DSurface9* CBaseSwapChain::GetViewRT(bool bLeft)
{
	if (!m_pPresenterData)
		return bLeft ? GetLeftBackBufferRT() : GetRightBackBufferRT();
	else
		return m_pPresenterData->GetViewRT(bLeft);
}

inline RECT* CBaseSwapChain::GetViewRect(bool bLeft)
{
	if (!m_pPresenterData)
		return bLeft ? GetLeftBackBufferRect() : GetRightBackBufferRect();
	else
		return NULL;
}

inline IDirect3DSurface9* CBaseSwapChain::GetPresenterBackBuffer()
{
	if (m_pWidePresenterSurface)
		return m_pWidePresenterSurface;
	else
	{
		if (!m_pPresenterData)
			return m_pPrimaryBackBuffer;
		else
			return m_pPresenterData->m_pPrimaryBackBuffer;
	}
}
