#include "StdAfx.h"
#include "LaserSight.h"
#include "..\ArchiveFile\blob.h"
#include "D3DSwapChain.h"

HRESULT LaserSight::Initialize(acGraphics::CDynRender_dx10UM* renderer)
{
	m_Renderer = renderer;
	if(gInfo.CustomLaserSightIconPath[0] != 0)
	{	
		renderer->GetTextureSizeFromFile(gInfo.CustomLaserSightIconPath, m_TextureSize);
		return renderer->CreateTextureFromFile(gInfo.CustomLaserSightIconPath, m_pSightTexture);
	}
	else  
	{	
		Blob b;
		if (b.Initialize( _T("Driver/LaserSight.png") ))
		{
			renderer->GetTextureSizeFromMemory(b.GetData(), (UINT)b.GetSize(), m_TextureSize);
			return renderer->CreateTextureFromMemory(b.GetData(), (UINT)b.GetSize(), m_pSightTexture);
		}
		else
		{
			DEBUG_MESSAGE(_T("Reading of Driver/LaserSight.png failed."));
			return E_FAIL;
		}
	}
}

void LaserSight::Clear()
{
	if(m_Renderer)
		m_Renderer->DeleteTexture(m_pSightTexture);
	m_Renderer = NULL;
}

void LaserSight::Draw(D3DSwapChain* pSwapChain, float frameTimeDelta)
{
	LS_LOCATION* v; 
	LaserSightData* pSight = pSwapChain->GetLaserSightData();
	pSight->SelectFrame(frameTimeDelta);
	m_Renderer->SetTexture(m_pSightTexture);
	
	// left sight
	v = pSight->GetLocationRight();
	SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
	_ASSERT(pRes != NULL);
	if (pRes->IsStereoActive())
	{
		pRes->SetStereoRenderTarget(VIEWINDEX_RIGHT);
		m_Renderer->Begin( acGraphics::RENDER_QUAD_LIST );
		m_Renderer->VtxTexCoord	( v[0].tu, v[0].tv ); m_Renderer->VtxPos( v[0].x, -v[0].y, 1.0f ); 	
		m_Renderer->VtxTexCoord	( v[1].tu, v[1].tv ); m_Renderer->VtxPos( v[1].x, -v[1].y, 1.0f );		
		m_Renderer->VtxTexCoord	( v[2].tu, v[2].tv ); m_Renderer->VtxPos( v[2].x, -v[2].y, 1.0f );		
		m_Renderer->VtxTexCoord	( v[3].tu, v[3].tv ); m_Renderer->VtxPos( v[3].x, -v[3].y, 1.0f );		
		m_Renderer->End();
	}

	v = pSight->GetLocationLeft();
	pRes->SetStereoRenderTarget(VIEWINDEX_LEFT);
	m_Renderer->Begin( acGraphics::RENDER_QUAD_LIST );
	m_Renderer->VtxTexCoord	( v[0].tu, v[0].tv ); m_Renderer->VtxPos( v[0].x, -v[0].y, 1.0f ); 	
	m_Renderer->VtxTexCoord	( v[1].tu, v[1].tv ); m_Renderer->VtxPos( v[1].x, -v[1].y, 1.0f );		
	m_Renderer->VtxTexCoord	( v[2].tu, v[2].tv ); m_Renderer->VtxPos( v[2].x, -v[2].y, 1.0f );		
	m_Renderer->VtxTexCoord	( v[3].tu, v[3].tv ); m_Renderer->VtxPos( v[3].x, -v[3].y, 1.0f );		
	m_Renderer->End();
	
	/*--- just for renderer test, do not remove this ---
	v = pSwapChain->m_LaserSightData.GetLocationRight();
	pRes->SetStereoRenderTarget(VIEWINDEX_RIGHT);
	m_Renderer->Begin( acGraphics::RENDER_QUAD_LIST );
	m_Renderer->VtxTexCoord	( 0, 0 ); m_Renderer->VtxPos(  10,  -50, 1.0f );
	m_Renderer->VtxTexCoord	( 1, 0 ); m_Renderer->VtxPos( 790,  -50, 1.0f );
	m_Renderer->VtxTexCoord	( 1, 1 ); m_Renderer->VtxPos( 790, -550, 1.0f );
	m_Renderer->VtxTexCoord	( 0, 1 ); m_Renderer->VtxPos(  10, -550, 1.0f );
	m_Renderer->End();

	// left sight
	v = pSwapChain->m_LaserSightData.GetLocationLeft();
	pRes->SetStereoRenderTarget(VIEWINDEX_LEFT);
	m_Renderer->Begin( acGraphics::RENDER_QUAD_LIST );
	m_Renderer->VtxTexCoord	( 0, 0 ); m_Renderer->VtxPos(  10,  -50, 1.0f );
	m_Renderer->VtxTexCoord	( 1, 0 ); m_Renderer->VtxPos( 790,  -50, 1.0f );
	m_Renderer->VtxTexCoord	( 1, 1 ); m_Renderer->VtxPos( 790, -550, 1.0f );
	m_Renderer->VtxTexCoord	( 0, 1 ); m_Renderer->VtxPos(  10, -550, 1.0f );
	m_Renderer->End();
	*/
}
