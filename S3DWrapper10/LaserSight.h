#pragma once
#include "..\S3DAPI\LaserSightData.h"
#include "..\Font\acgfx_dynrender.h"
#include "acgfx_dynrender_dx10UM.h"

class D3DSwapChain;

class LaserSight
{
public:
	LaserSight() { m_Renderer = NULL; m_TextureSize.cx = m_TextureSize.cy = 0; }
	~LaserSight() { Clear(); }

	HRESULT Initialize(acGraphics::CDynRender_dx10UM* renderer);
	void	Clear();

	void	Draw(D3DSwapChain* pRes, float frameTimeDelta);
	SIZE	GetTextureSize() { return m_TextureSize; }

protected:
	SIZE							m_TextureSize;
	acGraphics::CDynRender_dx10UM*	m_Renderer;
	acGraphics::CDynTexture			m_pSightTexture;
};
