#include "FontTexture.h"
#include <xstring>
#include "../CommonUtils/StringResourceManager.h"
#include <math.h>
#include <tchar.h>
#include <../CommonUtils/System.h>

iz3d::graphicUI::FontTexture::FontTexture( const LOGFONT& lf )
	:m_pData(0), m_size(0)
{
	CreateBinaryTextureFromFont( lf );
}


iz3d::graphicUI::FontTexture::~FontTexture()
{
}

void iz3d::graphicUI::FontTexture::CreateBinaryTextureFromFont( const LOGFONT& lf )
{
	std::wstring toStr = iz3d::resources::StringResourceManager::instance().getAllLettersString();
	HDC hdcDisplay = CreateDC( L"DISPLAY", 0, 0, 0 );
	HDC hdc = CreateCompatibleDC(hdcDisplay);
	auto hf = CreateFont( abs(lf.lfHeight),
		abs(lf.lfWidth ),
		abs( lf.lfEscapement),
		abs( lf.lfOrientation ),
		lf.lfWeight,
		lf.lfItalic,
		lf.lfUnderline,
		lf.lfStrikeOut,
		lf.lfCharSet,
		lf.lfOutPrecision,
		lf.lfClipPrecision,
		lf.lfQuality,
		lf.lfPitchAndFamily,
		lf.lfFaceName );//*/
	RECT rc = {0, 0, 1024, 1024};
	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof( bmpInfo.bmiHeader );
	bmpInfo.bmiHeader.biWidth = rc.right;
	bmpInfo.bmiHeader.biHeight = -rc.bottom;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;
	m_size = rc.right * rc.bottom * 3;
	HBITMAP hBmp = CreateDIBSection( hdc, &bmpInfo, DIB_RGB_COLORS, &m_pData, 0, 0 );
	auto oldFont = SelectObject( hdc, hf );
	DBG_UNREFERENCED_LOCAL_VARIABLE(oldFont);
	auto oldBmp = SelectObject( hdc, hBmp);
	DBG_UNREFERENCED_LOCAL_VARIABLE(oldBmp);

	SetTextColor( hdc, RGB( 255, 255, 255) );
	auto oldBkMode = SetBkMode( hdc, TRANSPARENT );
	DBG_UNREFERENCED_LOCAL_VARIABLE(oldBkMode);

	// text metrics
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	// font generation
	size_t quadSize = (size_t)sqrtf( (float)toStr.length() ) + 1;
	int x = 0, y = 0;
	ABC abc;
	RECT bufRect = {0, 0, 0, 0};
	for( size_t i = 0; i < toStr.length(); i++ ){
		if( i % quadSize == 0 ){
			x = 0;
			y += tm.tmHeight;
		}
		auto c = toStr[i]; // character
		TextOut( hdc, x, y, &c, 1 );
		GetCharABCWidths( hdc, c, c, &abc );
		auto w = abs( abc.abcA ) + abc.abcB + abs( abc.abcC );
		bufRect.left = x;
		bufRect.top = y;
		bufRect.right =  x + w;
		bufRect.bottom = y + tm.tmHeight;
		m_CharMap.insert( TCharRectMap::value_type(c, bufRect) );
		x += w;
	}
}

const void* iz3d::graphicUI::FontTexture::GetBinaryFontData() const
{
	return m_pData;
}

RECT iz3d::graphicUI::FontTexture::GetCharRect( const wchar_t ch ) const
{
	return m_CharMap.find( ch )->second;
}

size_t iz3d::graphicUI::FontTexture::GetBinaryFontSize() const
{
	return m_size;
}

void iz3d::graphicUI::FontTexture::CalculateTextSize( LPCTSTR text, size_t len, SIZE& textSize )
{
	SIZE buf = {0, 0};
	size_t curPos = 0;
	while( curPos < len )
	{
		curPos += CalculateLineSize( &text[curPos], len, buf );
		textSize.cy += buf.cy;
		textSize.cx = max( textSize.cx, buf.cx );
		++curPos;
	}
	return;
}

size_t iz3d::graphicUI::FontTexture::CalculateLineSize( LPCTSTR text, size_t len, SIZE& lineSize )
{
	lineSize.cx = lineSize.cy = 0;
	if( !len )
		return 0;
	auto rc = m_CharMap.find( text[0] )->second;
	lineSize.cy = rc.bottom - rc.top;
	lineSize.cx = rc.right - rc.left;
	size_t i = 1;
	for( ; i < len, text[i] != '\n'; ++i )
	{
		rc = m_CharMap.find( text[i] )->second;
		lineSize.cx += rc.right - rc.left;
	}
	return i;
}