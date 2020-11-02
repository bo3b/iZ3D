#pragma once
#include <windows.h>
#include <vector>
#include "LineGlyphs.h"
#include "CommonTexture.h"
#include "../CommonUtils/Function.h"
class TextGlyphs
{
	typedef std::vector<LineGlyphs> TLines;
	TLines m_Lines;
	size_t m_Width;
	size_t m_Height;
	void calculateWidth();
	void calculateHeight();
public:
	TextGlyphs( LPCTSTR text );
	~TextGlyphs(void);
	size_t getWidth() const;
	size_t getHeight() const;
	void DrawText( RECT& toCh, ICommonTexture::TDrawFunction* pF,
		unsigned Format );
};

