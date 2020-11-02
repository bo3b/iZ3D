#include "StdAfx.h"
#include "WizardDrawer.h"
#include "../Font/acgfx_dynrender.h"

WizardDrawer::WizardDrawer( acGraphics::CDynRender_dx10UM* pRender ) 
	: m_pRender( pRender )
{
}


WizardDrawer::~WizardDrawer()
{
	for( auto i = m_Textures.begin(); i != m_Textures.end(); ++ i )
	{
		m_pRender->DeleteTexture( **i );
		delete *i;
	}
	m_Textures.clear();
}

void WizardDrawer::PrepareTexture( void* pTextureID )
{
	m_pRender->SetTexture( *( ( acGraphics::CDynTexture* )pTextureID ) );
	m_pRender->Begin( acGraphics::RENDER_QUAD_LIST );
}

void WizardDrawer::DrawTexture( POINTF* pTextureData, POINTF* pRenderTarget )
{
	m_pRender->VtxTexCoord	( pTextureData->x, pTextureData->y );
	m_pRender->VtxPos( pRenderTarget->x, pRenderTarget->y, 1.0f ); 	
}

void WizardDrawer::EndTexture( void* pTextureID )
{
	m_pRender->End();
}

void WizardDrawer::SetLeft( bool isLeft )
{
	VIEWINDEX vi = isLeft ? VIEWINDEX_LEFT : VIEWINDEX_RIGHT;
	m_pCurSwapChain->SetStereoRenderTarget(vi);
}

void WizardDrawer::CreateTextureFromMemory( const void* pData,
	UINT nDataSize_,
	UINT Width, 
	UINT Height, 
	UINT bytesPerPixel, 
	void*& pTextID )
{
	acGraphics::CDynTexture * pTexture = new acGraphics::CDynTexture();
	m_pRender->CreateTexture( Width, Height, Width* bytesPerPixel, (BYTE*)pData, *pTexture  );
	pTextID = pTexture;
	m_Textures.push_back( pTexture );
}

void WizardDrawer::SetCurrentSwapChain( void* pSwapChain )
{
	m_pCurSwapChain = static_cast<SwapChainResources*>( pSwapChain );
}

void WizardDrawer::Release()
{
	delete this;
}