#include "DX9Texture.h"

DX9Texture::DX9Texture(LPDIRECT3DTEXTURE9 pTexture):
m_pTexture( pTexture )
{
}

DX9Texture::~DX9Texture(void)
{
	m_pTexture->Release();
}

SIZE DX9Texture::GetWidthHeight() const
{
	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc( 0, &desc );
	SIZE toRet;
	toRet.cx = desc.Width;
	toRet.cy = desc.Height;
	return toRet;
}

LPDIRECT3DTEXTURE9 DX9Texture::GetTexture9()
{
	return m_pTexture;
}