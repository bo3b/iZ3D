#include "TextGlyphs.h"
#include "FontTextureManager.h"
#include "CommonTexture.h"
TextGlyphs::TextGlyphs( LPCTSTR text )
{
	const wchar_t* startCh = &text[0];
	const wchar_t* endCh = startCh;
	for( ; *endCh != L'\0'; ++endCh )
	{
		if( *endCh == L'\n')
		{
			m_Lines.push_back( LineGlyphs( startCh, endCh ) );
			startCh = endCh + 2;
		}
	}
	m_Lines.push_back( LineGlyphs( startCh, endCh ) );
	calculateWidth();
	calculateHeight();
}

TextGlyphs::~TextGlyphs(void)
{
}

void TextGlyphs::calculateWidth()
{
	size_t width (0);
	for( auto i = m_Lines.begin(); i != m_Lines.end(); ++i )
	{
		width = max( i->getWidth(), width );
	}
	m_Width = width;
}

void TextGlyphs::calculateHeight()
{
	size_t height (0);
	for( auto i = m_Lines.begin(); i != m_Lines.end(); ++i )
	{
		height += i->getHeight();
	}
	m_Height = height;
}

size_t TextGlyphs::getWidth() const
{
	return m_Width;
}

size_t TextGlyphs::getHeight() const
{
	return m_Height;
}

void TextGlyphs::DrawText( RECT& toCh, ICommonTexture::TDrawFunction* pDrawFunc, unsigned Format )
{
	size_t width = toCh.right - toCh.left;
	size_t height = toCh.bottom - toCh.top;
	float xScale = 1.0f, yScale = 1.0f;
	RECT resRect = toCh;
	if( width && height )
	{
		xScale = ( float )( toCh.right - toCh.left ) / m_Width;
		yScale = (float)( toCh.bottom - toCh.top ) / m_Height;
		yScale = min( xScale, yScale );
		xScale = yScale;
	}
	if( xScale > 1.0 / 4 )
		xScale = 1.0 / 4;
	if( yScale > 1.0 / 4 )
		yScale = 1.0 / 4;
	int horizontalMultiplier = 0;
	// horizontal position
	if( Format & DT_LEFT )
	{
		resRect.right = resRect.left + LONG( m_Width * xScale );
	}
	else if( Format & DT_CENTER )
	{
		horizontalMultiplier = 1;
		resRect.left = ( resRect.left + resRect.right - LONG( m_Width * xScale ) ) / 2;
		resRect.right = resRect.left + LONG( m_Width * xScale );
	}
	else if( Format & DT_RIGHT )
	{
		horizontalMultiplier = 2;
		resRect.left = resRect.right - LONG( m_Width * xScale );
	}
	// vertical position
	if( Format & DT_TOP )
	{
		resRect.bottom = resRect.top + LONG(m_Height * yScale);
	}
	else if( Format & DT_VCENTER )
	{
		resRect.top = (resRect.top + resRect.bottom - LONG( m_Height * yScale ) ) / 2;
		resRect.bottom = resRect.top + LONG( m_Height * yScale );
	}
	else if( Format & DT_BOTTOM )
	{
		resRect.top = resRect.bottom - LONG( m_Height * yScale );
	}
	auto yStep = ( LONG )( m_Lines.begin()->getHeight() * yScale );
	RECT lineRect;
	lineRect.top = resRect.top;
	lineRect.bottom = resRect.top + yStep;
	auto xStep = 0;
	for( auto i = m_Lines.begin(); i != m_Lines.end(); ++i )
	{
		xStep = ( LONG ) ( ( m_Width - i->getWidth() ) * xScale ) / 2;
		lineRect.left = resRect.left + xStep * horizontalMultiplier;
		lineRect.right = resRect.right - xStep * horizontalMultiplier;
		i->DrawText( lineRect, pDrawFunc );
		lineRect.top = lineRect.bottom;
		lineRect.bottom += yStep;
	}
	// if return rect calculation
	if( Format & DT_CALCRECT )
	{
		toCh = resRect;
	}
}