#include "DX9Device.h"
#include "DX9Texture.h"
#include "DX9Line.h"
#include "DX9Sprite.h"
#include <algorithm>
#include "Framework.h"
DX9Device::DX9Device( LPDIRECT3DDEVICE9 pDevice ):
m_pDevice( pDevice )
{
}

DX9Device::~DX9Device()
{
}

HRESULT DX9Device::SetNeededRenderState()
{
	m_pDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &m_AlphaTestEnable );
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	m_pDevice->GetRenderState( D3DRS_ALPHAREF, &m_AlphaRef );
	m_pDevice->SetRenderState( D3DRS_ALPHAREF, 0 );

	m_pDevice->GetRenderState( D3DRS_SRCBLEND, &m_SrcBlend );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) ;

	m_pDevice->GetRenderState( D3DRS_DESTBLEND, &m_DestBlend ) ;
	m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) ;

	return S_OK;
}

HRESULT DX9Device::ReturnRenderState()
{
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, m_AlphaTestEnable);
	m_pDevice->SetRenderState( D3DRS_ALPHAREF, m_AlphaRef );
	m_pDevice->SetRenderState( D3DRS_SRCBLEND, m_SrcBlend );
	m_pDevice->SetRenderState( D3DRS_DESTBLEND, m_DestBlend );

	return S_OK;
}

HRESULT DX9Device::SetRenderTarget( void* pHandle )
{
	return m_pDevice->SetRenderTarget( 0, (IDirect3DSurface9*)pHandle );
}

HRESULT DX9Device::BeginScene()
{
	return m_pDevice->BeginScene();
}

HRESULT DX9Device::EndScene()
{
	return m_pDevice->EndScene();
}

HRESULT DX9Device::CreateLine( ICommonLine** ppLine )
{
	LPD3DXLINE pLine = 0;
	HRESULT hr = D3DXCreateLine( m_pDevice, &pLine );
	*ppLine = 0;
	if( SUCCEEDED( hr ) )
		*ppLine = new DX9Line( pLine );
	return hr;
}

HRESULT DX9Device::CreateFont( const LOGFONT& Font, ICommonFont** ppFont )
{
	ID3DXFont* pFont = 0;
	HRESULT hr = D3DXCreateFont(m_pDevice, Font.lfWidth , Font.lfHeight / 9, Font.lfWeight, 1, 
		Font.lfItalic, Font.lfCharSet, Font.lfOutPrecision, Font.lfQuality,
		Font.lfPitchAndFamily, Font.lfFaceName, &pFont );
	DX9Font* pDX9Font = new DX9Font( pFont );
	*ppFont = pDX9Font;
	return hr;
}

HRESULT DX9Device::CreateTextureFromMemory( const void* pData, size_t s, ICommonTexture** ppTexture )
{
	LPDIRECT3DTEXTURE9 pText = 0;
	HRESULT hr = LoadTextureFromMemory(m_pDevice, pData, s, &pText);
	*ppTexture = 0;
	if( SUCCEEDED( hr ) )
		*ppTexture = new DX9Texture( pText );
	return hr;
}

HRESULT DX9Device::CreateTextureFromResource( HMODULE module, LPCTSTR pSrcResource, ICommonTexture** ppTexture )
{
	LPDIRECT3DTEXTURE9 pText = 0;
	HRESULT hr = D3DXCreateTextureFromResource( m_pDevice, module, pSrcResource, &pText );
	*ppTexture = 0;
	if( SUCCEEDED( hr ) )
		*ppTexture = new DX9Texture( pText );
	return hr;
}

HRESULT DX9Device::CreateSprite( ICommonSprite** ppSprite )
{
	LPD3DXSPRITE pSpr = 0;
	HRESULT hr = D3DXCreateSprite( m_pDevice, &pSpr );
	*ppSprite = 0;
	if( SUCCEEDED( hr ) )
		*ppSprite = new DX9Sprite( pSpr );
	return hr;
}

HRESULT DX9Device::DeleteSprite( ICommonSprite** ppSprite )
{
	if( !ppSprite )
		return S_OK;
	DX9Sprite* pToDelete = static_cast<DX9Sprite*>(*ppSprite);
	delete pToDelete;
	return S_OK;
}
HRESULT DX9Device::CreateTextureFromFile( LPCTSTR pSrcFile, ICommonTexture** ppTexture )
{
	ppTexture = 0;
	LPDIRECT3DTEXTURE9 pText;
	HRESULT hr = D3DXCreateTextureFromFile(m_pDevice, pSrcFile, &pText);
	DX9Texture* pDX9Texture = new DX9Texture( pText );
	*ppTexture = pDX9Texture;
	return hr;
}

SIZE DX9Device::GetViewportWidthHeight()
{
	D3DVIEWPORT9 pViewport;
	m_pDevice->GetViewport( &pViewport );
	SIZE retPoint;
	retPoint.cx = pViewport.Width;
	retPoint.cy = pViewport.Height;
	return retPoint;
}

HRESULT DX9Device::SetCurrentSwapChain( void* /*pSwapChain*/ )
{
	// nothing to do
	return E_NOTIMPL;
}

void DX9Device::Release()
{
	delete this;
}