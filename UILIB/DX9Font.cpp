#include "DX9Font.h"

DX9Font::DX9Font( ID3DXFont* pFont ):
m_pFont( pFont )
{

}

HRESULT DX9Font::TextOut( LPCTSTR text,
						 int length,
						 RECT* pRect,
						 DWORD Format,
						 DWORD Colour )
{
	return m_pFont->DrawText( 0, text, length, pRect, Format, Colour );
}

DX9Font::~DX9Font()
{
	m_pFont->Release();
}