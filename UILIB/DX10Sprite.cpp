#include "DX10Sprite.h"
#include "DX10Texture.h"

// main code
DX10Sprite::DX10Sprite( ICommonDrawer* pDrawer )
	:m_pDrawer( pDrawer )
{
}

DX10Sprite::~DX10Sprite(void)
{
}

float CLEAR_COLOR[] = {0.0f, 0.125f, 0.6f, 1.0f};
HRESULT DX10Sprite::Begin()
{
	return S_OK;
}

HRESULT DX10Sprite::End()
{
	// TODO
	return E_NOTIMPL;
}

HRESULT DX10Sprite::Draw( ICommonTexture* pText, RECT* rect )
{
	if( !pText )
		return S_OK;
	auto pTex = (DX10Texture*)pText;
	pTex->BeginTexture();
	//RECT texCoord;
	RECT rtCoord;
	if( m_pScale->x == 0.0 )
		m_pScale->x = 1.0f;
	if( m_pScale->y == 0.0 )
		m_pScale->y = 1.0f;
	rtCoord.left = m_pTransform->x;
	rtCoord.top = m_pTransform->y;
	rtCoord.right = rtCoord.left + (LONG)(( rect->right - rect->left ) * m_pScale->x);
	rtCoord.bottom = rtCoord.top + (LONG)(( rect->bottom - rect->top ) * m_pScale->y);
	pTex->Draw( rect, &rtCoord );
	pTex->EndTexture();
	m_pTransform = 0;
	m_pScale = 0;
	return S_OK;
}

HRESULT DX10Sprite::SetTransform( POINTF& scale, POINT& transform )
{
	m_pScale = &scale;
	m_pTransform = &transform;
	return S_OK;
}

