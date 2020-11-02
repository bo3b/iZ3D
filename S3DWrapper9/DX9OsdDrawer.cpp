#include "StdAfx.h"
#include "DX9OsdDrawer.h"

DX9OsdDrawer::DX9OsdDrawer( IDirect3DDevice9* pDevice, RECT* pLeftRect, RECT* pRightRect, IDirect3DSurface9* pLeftBuffer, IDirect3DSurface9* pRightBuffer ):
m_pDevice( pDevice ),
	m_pSwapChain( 0 ),
	m_pLeftRect( pLeftRect ),
	m_pRightRect( pRightRect ),
	m_pLeftBuffer( pLeftBuffer ),
	m_pRightBuffer( pRightBuffer ),
	m_pCurRect( pLeftRect )
{
}


DX9OsdDrawer::~DX9OsdDrawer(void)
{
	for( auto i = m_Textures.begin(); i != m_Textures.end(); ++i )
	{
		(*i)->Release();
	}
	m_Textures.clear();
}

void DX9OsdDrawer::PrepareTexture( void* pTextureID )
{
	m_VerticesToDraw.clear();
	m_pDevice->SetTexture( 0, (IDirect3DBaseTexture9*) pTextureID );
}

void DX9OsdDrawer::DrawTexture( POINTF* pTextureData, POINTF* pRenderTarget )
{
	CustomVertex bufVer = { pRenderTarget->x + m_pCurRect->left, pRenderTarget->y + m_pCurRect->top, 0.0f, pTextureData->x, pTextureData->y };
	m_VerticesToDraw.push_back( bufVer );
}

void DX9OsdDrawer::EndTexture( void* pTextureID )
{
	// vertex buffer
	IDirect3DVertexBuffer9* pVB;
	void* pVertices;
	size_t size = m_VerticesToDraw.size() * sizeof( CustomVertex );
	m_pDevice->CreateVertexBuffer( size, 0, VERTEX_FORMAT, D3DPOOL_DEFAULT, &pVB, 0 );
	pVB->Lock( 0, size, &pVertices, 0 );
	memcpy( pVertices, &m_VerticesToDraw[0], size );
	pVB->Unlock();
	// end vertex buffer

	DWORD zEnable;
	DWORD zWriteEnable;
	m_pDevice->GetRenderState( D3DRS_ZENABLE, &zEnable );
	m_pDevice->GetRenderState( D3DRS_ZWRITEENABLE, &zWriteEnable );
	m_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pDevice->BeginScene();
	m_pDevice->SetStreamSource( 0, pVB, 0, sizeof( CustomVertex ) );
	m_pDevice->SetFVF( VERTEX_FORMAT );
	m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	m_pDevice->EndScene();
	m_pDevice->SetRenderState( D3DRS_ZENABLE, zEnable );
	m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, zWriteEnable );
}

void DX9OsdDrawer::SetLeft( bool isLeft )
{
	if( isLeft )
	{
		m_pDevice->SetRenderTarget( 0, m_pLeftBuffer );
		m_pCurRect = m_pLeftRect;
	}
	else{
		m_pDevice->SetRenderTarget( 0, m_pRightBuffer );
		m_pCurRect = m_pRightRect;
	}
}

void DX9OsdDrawer::CreateTextureFromMemory( const void* pData, UINT nDataSize_, UINT Width, UINT Height, UINT bytesPerPixel, void*& pTextID )
{
	LPDIRECT3DTEXTURE9 pTexture = 0;
	D3DXCreateTextureFromFileInMemory( m_pDevice, pData, nDataSize_, &pTexture );
	pTextID = pTexture;
	m_Textures.push_back( pTexture );
}

void DX9OsdDrawer::SetCurrentSwapChain( void* pSwapChain )
{
	m_pSwapChain = pSwapChain;
}

void DX9OsdDrawer::Release()
{
	delete this;
}
