#include "DX9Sprite.h"
#include "DX9Texture.h"
DX9Sprite::DX9Sprite( LPD3DXSPRITE pSprite ):
m_pSprite( pSprite )
{
}

DX9Sprite::~DX9Sprite(void)
{
	m_pSprite->Release();
}

HRESULT DX9Sprite::Begin()
{
	return m_pSprite->Begin( 0 );
}

HRESULT DX9Sprite::End()
{
	return m_pSprite->End();
}

HRESULT DX9Sprite::Draw( ICommonTexture* pText, RECT* rect )
{
	if( !pText )
		return S_OK;
	HRESULT hr = m_pSprite->Draw( ((DX9Texture*)pText)->GetTexture9(),
		rect, NULL,	NULL, 0xFFFFFFFF );
	return hr;
}

HRESULT DX9Sprite::SetTransform( POINTF& scale, POINT& translation )
{
	D3DXMATRIX Mat;
	D3DXVECTOR2 Scale(scale.x, scale.y);
	D3DXVECTOR2	fTrans = D3DXVECTOR2((FLOAT)translation.x, (FLOAT)translation.y);
	D3DXMatrixTransformation2D(&Mat, NULL, 0, &Scale, NULL, NULL, &fTrans);
	return m_pSprite->SetTransform( &Mat );
}