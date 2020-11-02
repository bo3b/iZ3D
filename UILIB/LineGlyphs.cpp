#include "LineGlyphs.h"
#include "FontTextureManager.h"

LineGlyphs::LineGlyphs( const wchar_t* startChar, const wchar_t* endChar )
	: m_Width( 0 )
{
	auto sz = endChar - startChar;
	m_Glyphs.reserve( sz );
	for( auto ch = startChar; ch != endChar; ++ ch ){
		m_Glyphs.push_back( iz3d::graphicUI::FontTextureManager::instance().GetCharRect( *ch ) );
	}
	calculateWidth();
}

LineGlyphs::~LineGlyphs(void)
{
}

size_t LineGlyphs::getWidth() const
{
	return m_Width;
}

size_t LineGlyphs::getHeight() const
{
	if( m_Glyphs.empty() )
		return 0;
	auto firstCh = m_Glyphs.begin();
	return firstCh->bottom - firstCh->top;
}

void LineGlyphs::calculateWidth()

{
	if( m_Glyphs.empty() )
	{
		m_Width = 0;
		return;
	}
	size_t width = 0;
	for( auto i = m_Glyphs.begin(); i != m_Glyphs.end(); ++i ){
		width += i->right - i->left;
	}
	m_Width = width;
}

void LineGlyphs::DrawText( RECT& toCh, ICommonTexture::TDrawFunction* pDrawFunc )
{
	auto xScale = (toCh.right - toCh.left) / ( float )m_Width;
	auto unScaledLeft = 0;
	auto unScaletRight = 0;
	RECT toRect = toCh;
	for( auto i = m_Glyphs.begin(); i != m_Glyphs.end(); ++i ){
		unScaletRight += ( i->right - i->left );
		toRect.left = toCh.left + ( LONG )( unScaledLeft * xScale );
		toRect.right = toCh.left + ( LONG )( unScaletRight * xScale );
		pDrawFunc->func( &*i, &toRect );
		unScaledLeft = unScaletRight;
	}
}