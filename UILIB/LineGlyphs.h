#pragma once
#include <vector>
#include <windows.h>
#include "CommonTexture.h"
class LineGlyphs
{
	typedef std::vector<RECT> TGlyphs;
	TGlyphs m_Glyphs;
	size_t m_Width;
	void calculateWidth();
public:
	//typedef void (TDrawingFunction)( RECT& fromCh, RECT& toCh );
	LineGlyphs( const wchar_t* startChar, const wchar_t* endChar );
	~LineGlyphs(void);

	size_t getWidth() const;
	size_t getHeight() const;

	void DrawText( RECT& toCh, ICommonTexture::TDrawFunction* pDrawFunc );
};

