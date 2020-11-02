#include "DX10Font.h"
#include <tchar.h>
#include "TextGlyphs.h"
#include <boost/bind.hpp>


DX10Font::DX10Font( const LOGFONT& Font, DX10Texture* pTexture ) :
m_pFont( &Font ), m_pFontTexture(pTexture)
{
	
}


DX10Font::~DX10Font(void)
{
}

HRESULT DX10Font::TextOut( LPCTSTR text, int /*length*/, RECT* pRect, DWORD Format, DWORD /*Colour*/ )
{
	iz3d::graphicUI::FontTextureManager::instance().SetCurFont(m_pFont);
	TextGlyphs tg( text );
	TFuncImpl fImpl( &DX10Texture::Draw, m_pFontTexture );
	tg.DrawText(*pRect, &fImpl, Format );
	return S_OK;
}
