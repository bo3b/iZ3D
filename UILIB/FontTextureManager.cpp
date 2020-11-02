#include "FontTexture.h"
#include "FontTextureManager.h"
#include <algorithm>
iz3d::graphicUI::FontTextureManager::FontTextureManager():m_pCurTexture( 0 )
{

}

iz3d::graphicUI::FontTextureManager::~FontTextureManager()
{
	class Deleter{
	public:
		void operator()( TFonts::value_type& el )
		{
			delete el.second;
		}
	};
	std::for_each(m_Manager.begin(), m_Manager.end(), Deleter());
	m_Manager.clear();
}

bool iz3d::graphicUI::FontTextureManager::AddFont( const LOGFONT* pLogFont )
{
	if( m_Manager.find( pLogFont ) != m_Manager.end() )
		return false;
	auto pNewFontTexture = new FontTexture( *pLogFont );
	if( !m_pCurTexture )
		m_pCurTexture = pNewFontTexture;
	m_Manager[pLogFont] = pNewFontTexture;
	return true;
}

void iz3d::graphicUI::FontTextureManager::SetCurFont( const LOGFONT* pLogFont )
{
	if (m_Manager.find( pLogFont ) == m_Manager.end() )
		AddFont(pLogFont);
	m_pCurTexture = m_Manager[pLogFont];
}

RECT iz3d::graphicUI::FontTextureManager::GetCharRect( const wchar_t ch ) const
{
	return m_pCurTexture->GetCharRect( ch );
}

const void* iz3d::graphicUI::FontTextureManager::getFontTextureBinaryData() const
{
	return m_pCurTexture->GetBinaryFontData();
}

void iz3d::graphicUI::FontTextureManager::CalculateTextSize( LPCTSTR text, size_t len, SIZE& textSize )
{
	return m_pCurTexture->CalculateTextSize( text, len, textSize );
}

size_t iz3d::graphicUI::FontTextureManager::CalculateLineSize( LPCTSTR text, size_t len, SIZE& lineSize )
{
	return m_pCurTexture->CalculateLineSize( text, len, lineSize );
}

size_t iz3d::graphicUI::FontTextureManager::getFontTextureSize() const
{
	return m_pCurTexture->GetBinaryFontSize();
}
