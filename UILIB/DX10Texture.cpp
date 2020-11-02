#include "DX10Texture.h"


DX10Texture::DX10Texture( ICommonDrawer* pCommonDrawer, UINT Width, UINT Height )
	: m_pCommonDrawer( pCommonDrawer ), m_Width(Width), m_Height( Height ), m_pTextID( 0 )
{
}


DX10Texture::~DX10Texture(void)
{
}

SIZE DX10Texture::GetWidthHeight() const
{
	SIZE toRet = {m_Width, m_Height};
	return toRet;
}

HRESULT DX10Texture::LoadTextureFromMemory( void* pData,
	size_t dataSize,
	UINT bytesPerPixel,
	UINT Width, UINT Height )
{
	m_Width = Width;
	m_Height = Height;
	m_pCommonDrawer->CreateTextureFromMemory ( pData, (UINT)dataSize, Width, Height, bytesPerPixel, m_pTextID );
	return S_OK;
}

void DX10Texture::Draw( RECT* pFrom, RECT* pTo )
{
	BeginTexture();
	POINTF TextureCoord;
	TextureCoord.x = ( float )pFrom->left / m_Width;
	TextureCoord.y = ( float )pFrom->top / m_Height;
	POINTF RTCoord;
	RTCoord.x = (FLOAT)pTo->left;
	RTCoord.y = (FLOAT)-pTo->top;
	m_pCommonDrawer->DrawTexture( &TextureCoord, &RTCoord );

	TextureCoord.x = ( float )pFrom->right / m_Width;
	RTCoord.x = (FLOAT)pTo->right;
	m_pCommonDrawer->DrawTexture( &TextureCoord, &RTCoord );

	TextureCoord.y = ( float )pFrom->bottom / m_Height;
	RTCoord.y = (FLOAT)-pTo->bottom;
	m_pCommonDrawer->DrawTexture( &TextureCoord, &RTCoord );

	TextureCoord.x = ( float )pFrom->left / m_Width;
	RTCoord.x = (FLOAT)pTo->left;
	m_pCommonDrawer->DrawTexture( &TextureCoord, &RTCoord );
	EndTexture();
}

void DX10Texture::BeginTexture()
{
	m_pCommonDrawer->PrepareTexture( m_pTextID );
}

void DX10Texture::EndTexture()
{
	m_pCommonDrawer->EndTexture( m_pTextID );
}