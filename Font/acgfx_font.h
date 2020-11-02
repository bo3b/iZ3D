/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

/*
AngelCode Tool Box Library
Copyright (c) 2007-2008 Andreas Jonsson

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you 
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product 
documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.

Andreas Jonsson
andreas@angelcode.com
*/

#pragma once

#ifndef FONT_H
#define FONT_H

#include <d3d9.h>
#include <d3d10.h>
#include <vector>
#include <string>
#include <boost/unordered_map.hpp>

#include "acgfx_dynrender.h"


namespace acGraphics
{
	class CDynRender;
	class CFontLoader;

	typedef wchar_t SChar;

	struct SCharDescr
	{
		SCharDescr() : srcX(0), srcY(0), srcW(0), srcH(0), xOff(0), yOff(0), xAdv(0), page(0) {}

		short srcX;
		short srcY;
		short srcW;
		short srcH;
		short xOff;
		short yOff;
		short xAdv;
		short page;
		unsigned int chnl;

		std::vector<int> kerningPairs;
	};

	typedef boost::unordered_map<SChar, SCharDescr*> CharMap_t;

	class CFont
	{
	public:
		CFont();
		~CFont();

		int Init(const TCHAR *fontFile, CDynRender *render);

		float GetTextWidth(const char *text, int count);
		float GetTextWidth(const wchar_t *text, int count);
		void Draw(float x, float y, float z, const char *text, int count, unsigned int mode);
		void Draw(float x, float y, float z, const wchar_t *text, int count, unsigned int mode);

		int DrawML(float x, float y, float z, const char *text, int count, unsigned int mode);
		int DrawML(float x, float y, float z, const wchar_t *text, int count, unsigned int mode);

		void SetHeight(float h);
		float GetHeight();

		float GetBottomOffset();
		float GetTopOffset();

		void SetTexelOffset(float offset);

		void PreparePixelPerfectOutput( CONST D3DVIEWPORT9& vp );
		void PreparePixelPerfectOutput( CONST D3D10_VIEWPORT& vp );

	protected:
		friend class CFontLoader;

		template< class T >  
		float InternalGetTextWidth(const T *text, const T *end);
		template< class T >  
		void InternalDraw(float x, float y, float z, const T *text, int count, unsigned int mode);
		template< class T >  
		int InternalDrawML(float x, float y, float z, const T *text, int count, unsigned int mode);

		template< class T >  
		float GetTextOffset( unsigned int mode, const T * text, const T * end );
		template< class T >  
		void InternalDrawLine(float x, float y, float z, const T *text, const T *end, float spacing = 0);

		float AdjustForKerningPairs(SChar first, SChar second);
		SCharDescr *GetChar(SChar id);

		int GetTextLength(const char *text);
		int GetTextLength(const wchar_t *text);
		template< class T >  
		SChar GetTextChar(const T *text);
		template< class T >
		const T * FindTextChar(const T *text, const T *end, T ch);

		short m_FontHeight; // total height of the font
		short m_Base;       // y of base line
		short m_ScaleW;
		short m_ScaleH;
		SCharDescr m_DefChar;
		bool m_HasOutline;

		float m_Scale;
		CharMap_t m_Chars;
		float m_TexelOffset;

		std::vector< acGraphics::CDynTexture >	m_FontPages;
		CDynRender*					m_pDynRender;
	};

	const int FONT_ALIGN_LEFT   = 0;
	const int FONT_ALIGN_CENTER = 1;
	const int FONT_ALIGN_RIGHT  = 2;
	const int FONT_ALIGN_JUSTIFY = 3;
}

// 2008-09-11 Removed UTF8 support
// 2008-05-11 Storing the characters in a map instead of an array
// 2008-05-17 Added support for writing text with UTF8 and UTF16 encoding

#endif
