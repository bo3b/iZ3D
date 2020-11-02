/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
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

#include "pcmp.h"
#include "acgfx_font.h"

using namespace std;

namespace acGraphics
{

	// Implement private helper classes for loading the bitmap font files

	class CFontLoader
	{
	public:
		CFontLoader(FILE *f, CFont *font, const TCHAR *fontFile);

		virtual int Load() = 0; // Must be implemented by derived class

	protected:
		void LoadPage(int id, const TCHAR *pageFile, const TCHAR *fontFile);
		void SetFontInfo(int outlineThickness);
		void SetCommonInfo(short fontHeight, short base, short scaleW, short scaleH, int pages, bool isPacked);
		void AddChar(int id, short x, short y, short w, short h, short xoffset, short yoffset, short xadvance, short page, int chnl);
		void AddKerningPair(SChar first, SChar second, int amount);

		FILE *f;
		CFont *font;
		const TCHAR *fontFile;

		int outlineThickness;
	};

	class CFontLoaderTextFormat : public CFontLoader
	{
	public:
		CFontLoaderTextFormat(FILE *f, CFont *font, const TCHAR *fontFile);

		int Load();

		int SkipWhiteSpace(std::wstring &str, int start);
		int FindEndOfToken(std::wstring &str, int start);

		void InterpretInfo(std::wstring &str, int start);
		void InterpretCommon(std::wstring &str, int start);
		void InterpretChar(std::wstring &str, int start);
		void InterpretSpacing(std::wstring &str, int start);
		void InterpretKerning(std::wstring &str, int start);
		void InterpretPage(std::wstring &str, int start, const TCHAR *fontFile);
	};

	class CFontLoaderBinaryFormat : public CFontLoader
	{
	public:
		CFontLoaderBinaryFormat(FILE *f, CFont *font, const TCHAR *fontFile);

		int Load();

		void ReadInfoBlock(int size);
		void ReadCommonBlock(int size);
		void ReadPagesBlock(int size);
		void ReadCharsBlock(int size);
		void ReadKerningPairsBlock(int size);
	};

	//=============================================================================
	// CFont
	//
	// This is the CFont class that is used to write text with bitmap fonts.
	//=============================================================================

	CFont::CFont()
	{
		m_FontHeight = 0;
		m_Base = 0;
		m_ScaleW = 0;
		m_ScaleH = 0;
		m_Scale = 1.0f;
		m_TexelOffset = 0.5f;
		m_pDynRender = 0;
	}

	CFont::~CFont()
	{
		for( UINT n = 0; n < m_FontPages.size(); n++ )
		{
			acGraphics::CDynTexture& texture = m_FontPages[n];
			if ( texture.m_pDxTex )			
				m_pDynRender->DeleteTexture( texture );
		}

		CharMap_t::iterator it = m_Chars.begin();
		while( it != m_Chars.end() )
		{
			delete it->second;
			it++;
		}		
	}

	int CFont::Init(const TCHAR *fontFile, CDynRender *render)
	{
		this->m_pDynRender = render;

		// Load the font
		FILE *f = NULL;
		_wfopen_s( &f, fontFile, L"rb");
		if(f)
		{
			// Determine format by reading the first bytes of the file
			char str[4] = {0};
			fread(str, 3, 1, f);
			fseek(f, 0, SEEK_SET);

			CFontLoader *loader = 0;
			if( strcmp(str, "BMF") == 0 )
				loader = DNew CFontLoaderBinaryFormat(f, this, fontFile);
			else
				loader = DNew CFontLoaderTextFormat(f, this, fontFile);

			int r = loader->Load();
			delete loader;
			return r;
		}
		return 0;
	}

	// Internal
	inline SCharDescr *CFont::GetChar(SChar id)
	{
		CharMap_t::iterator it = m_Chars.find(id);
		if( it == m_Chars.end() ) return 0;

		return it->second;
	}

	// Internal
	float CFont::AdjustForKerningPairs(SChar first, SChar second)
	{	
		SCharDescr *ch = GetChar(first);
		if( ch == 0 ) return 0;
		for( UINT n = 0; n < ch->kerningPairs.size(); n += 2 )
		{
			if( ch->kerningPairs[n] == second )
				return ch->kerningPairs[n+1] * m_Scale;
		}

		return 0;
	}

	float CFont::GetTextWidth(const char *text, int count)
	{
		if( count <= 0 )
			count = GetTextLength(text);
		const char *end = text + count;
		return InternalGetTextWidth(text, end);
	}

	float CFont::GetTextWidth(const wchar_t *text, int count)
	{
		if( count <= 0 )
			count = GetTextLength(text);
		const wchar_t *end = text + count;
		return InternalGetTextWidth(text, end);
	}

	template< class T >  
	inline float CFont::InternalGetTextWidth(const T *text, const T *end)
	{
		float x = 0;

		const T *pos = text;
		while ( pos < end )
		{
			SChar charId = GetTextChar(pos);

			SCharDescr *ch = GetChar(charId);
			if( ch == 0 ) ch = &m_DefChar;

			x += m_Scale * (ch->xAdv);

			pos++;
			if( pos < end )
				x += AdjustForKerningPairs(charId, GetTextChar(pos));
		}

		return x;
	}

	void CFont::SetHeight(float h)
	{
		m_Scale = h / float(m_FontHeight);
	}

	float CFont::GetHeight()
	{
		return m_Scale * float(m_FontHeight);
	}

	float CFont::GetBottomOffset()
	{
		return m_Scale * (m_Base - m_FontHeight);
	}

	float CFont::GetTopOffset()
	{
		return m_Scale * (m_Base - 0);
	}

	void CFont::SetTexelOffset( float offset )
	{
		m_TexelOffset = offset;
	}

	// Internal
	// Returns the number of bytes in the string until the null char
	inline int CFont::GetTextLength(const char *text)
	{
		return (int)strlen(text);
	}

	inline int CFont::GetTextLength(const wchar_t *text)
	{
		return (int)wcslen(text);
	}

	// Internal
	template< class T >  
	inline SChar CFont::GetTextChar(const T *text)
	{
		SChar ch = text[0];
		return ch;
	}

	// Internal
	template< class T >
	const T * CFont::FindTextChar(const T *text, const T *end, T ch)
	{
		const T *pos = text;
		while( pos < end )
		{
			if( pos[0] == ch )
				return pos;
			pos++;
		} 

		return NULL;
	}

	template< class T >
	inline float CFont::GetTextOffset( unsigned int mode, const T * text, const T * end )
	{
		switch(mode)
		{
		case FONT_ALIGN_CENTER:
			return -InternalGetTextWidth(text, end)/2;
		case FONT_ALIGN_RIGHT:
			return -InternalGetTextWidth(text, end);
		default:
			return 0;
		}
	}

	template< class T >  
	void CFont::InternalDrawLine(float x, float y, float z, const T *text, const T *end, float spacing)
	{
		int page = -1;
		m_pDynRender->Begin(RENDER_QUAD_LIST);

		const T *pos = text;
		y += m_Scale * float(m_Base);

		float rScaleW = 1.0f / m_ScaleW;
		float rScaleH = 1.0f / m_ScaleH;

		while (pos < end)
		{
			SChar charId = GetTextChar(pos);
			SCharDescr *ch = GetChar(charId);
			if( ch == 0 ) ch = &m_DefChar;

			// Map the center of the texel to the corners
			// in order to get pixel perfect mapping
			float u = (float(ch->srcX)+m_TexelOffset) * rScaleW;
			float v = (float(ch->srcY)+m_TexelOffset) * rScaleH;
			float u2 = u + float(ch->srcW) * rScaleW;
			float v2 = v + float(ch->srcH) * rScaleH;

			float a = m_Scale * float(ch->xAdv + 1);
			float w = m_Scale * float(ch->srcW);
			float h = m_Scale * float(ch->srcH);
			float ox = m_Scale * float(ch->xOff);
			float oy = m_Scale * float(ch->yOff);

			if( ch->page != page )
			{
				m_pDynRender->End();
				if (size_t(ch->page) >= m_FontPages.size())
					return;
				page = ch->page;			
				m_pDynRender->SetTexture( m_FontPages[page] );
				m_pDynRender->Begin( RENDER_QUAD_LIST );
			}

			m_pDynRender->VtxData(ch->chnl);
			m_pDynRender->VtxTexCoord(u, v);
			m_pDynRender->VtxPos(x+ox, y-oy, z);
			m_pDynRender->VtxTexCoord(u2, v);
			m_pDynRender->VtxPos(x+w+ox, y-oy, z);
			m_pDynRender->VtxTexCoord(u2, v2);
			m_pDynRender->VtxPos(x+w+ox, y-h-oy, z);
			m_pDynRender->VtxTexCoord(u, v2);
			m_pDynRender->VtxPos(x+ox, y-h-oy, z);

			x += a;
			if( charId == (T)' ' )
				x += spacing;
			else if( charId == (T)'\t' )
				x += 4 * spacing;

			pos++;
			if( pos < end )
				x += AdjustForKerningPairs(charId, GetTextChar(pos));
		}

		m_pDynRender->End();
	}

	void CFont::Draw(float x, float y, float z, const char *text, int count, unsigned int mode)
	{
		InternalDraw(x, y, z, text, count, mode);
	}

	void CFont::Draw( float x, float y, float z, const wchar_t *text, int count, unsigned int mode )
	{
		InternalDraw(x, y, z, text, count, mode);
	}

	// Internal
	template< class T >  
	void CFont::InternalDraw( float x, float y, float z, const T *text, int count, unsigned int mode )
	{
		if( count <= 0 )
			count = GetTextLength(text);
		const T *end = text + count;

		y = -y - m_Scale * float(m_FontHeight);
		x += GetTextOffset(mode, text, end);

		InternalDrawLine(x, y, z, text, end);
	}

	int CFont::DrawML(float x, float y, float z, const char *text, int count, unsigned int mode)
	{
		return InternalDrawML(x, y, z, text, count, mode );
	}

	int CFont::DrawML( float x, float y, float z, const wchar_t *text, int count, unsigned int mode )
	{
		return InternalDrawML(x, y, z, text, count, mode );
	}

	// Internal
	template< class T >  
	int CFont::InternalDrawML( float x, float y, float z, const T *text, int count, unsigned int mode )
	{
		if( count <= 0 )
			count = GetTextLength(text);
		const T *end = text + count;

		// Get first line
		const T *pos = text;
		const T *nextLine = FindTextChar(pos, end, (T)'\n'); 
		if( nextLine == NULL ) 
			nextLine = end;
		y = -y;
		int lineCount = 0;
		while( pos < end )
		{
			float cx = x + GetTextOffset(mode, pos, nextLine);

			y -= m_Scale * float(m_FontHeight);

			InternalDrawLine(cx, y, z, pos, nextLine);
			lineCount++;

			// Get next line
			pos = nextLine;
			T ch = pos[0];
			pos++;
			if( ch == (T)'\n' )
			{
				nextLine = FindTextChar(pos, end, (T)'\n'); 
				if( nextLine == NULL ) 
					nextLine = end;
			}
		}
		return lineCount;
	}

	void CFont::PreparePixelPerfectOutput(CONST D3DVIEWPORT9 &vp)
	{
		D3DXMATRIX mtx;
		// Setup orthogonal view
		// Origin is in lower-left corner
		D3DXMatrixOrthoOffCenterLH(&mtx, 0, (float)vp.Width, -(float)vp.Height, 0, 1.0f, 1e+7f);
		D3DXMATRIX mtx2;
		D3DXMatrixTranspose(&mtx2, &mtx);
		//D3DXMatrixOrthoLH(&mtx, (float)vp.Width, (float)vp.Height, vp.MinZ, vp.MaxZ);
		m_pDynRender->SetVertexShaderConstantF(0, &mtx2._11, 4 );

		// Adjust the scale of the font so that the 
		// resolution of texture is the same as the screen
		m_Scale = 1.0f;
	}

	void CFont::PreparePixelPerfectOutput( CONST D3D10_VIEWPORT& vp )
	{
		D3DXMATRIX mtx;		
		D3DXMatrixOrthoOffCenterLH(&mtx, 0, (float)vp.Width, -(float)vp.Height, 0, 1.0f, 1e+7f);
		//m_pDynRender->SetVertexShaderConstantF(0, &mtx._11, 4 );
		D3DXMATRIX mtx2;
		D3DXMatrixTranspose(&mtx2, &mtx);
		m_pDynRender->SetVertexShaderConstantF(0, &mtx2._11, 4 );		
		m_Scale = 1.0f;
	}

	//=============================================================================
	// CFontLoader
	//
	// This is the base class for all loader classes. This is the only class
	// that has access to and knows how to set the CFont members.
	//=============================================================================

	CFontLoader::CFontLoader(FILE *f, CFont *font, const TCHAR *fontFile)
	{
		this->f = f;
		this->font = font;
		this->fontFile = fontFile;

		outlineThickness = 0;
	}

	void CFontLoader::LoadPage(int id, const TCHAR *pageFile, const TCHAR *fontFile)
	{
		wstring str;

		// Load the texture from the same directory as the font descriptor file

		// Find the directory
		str = fontFile;
		for( size_t n = 0; (n = str.find('/', n)) != string::npos; ) str.replace(n, 1, L"\\");
		size_t i = str.rfind('\\');
		if( i != string::npos )
			str = str.substr(0, i+1);
		else
			str = L"";

		// Load the font textures
		str += pageFile;
		acGraphics::CDynTexture page;
		UINT mipLevels = 0; // 0 = all

		//HRESULT hr = D3DXCreateTextureFromFileEx( font->m_pDynRender->GetDevice(), str.c_str(), 
		//	D3DX_DEFAULT, D3DX_DEFAULT, mipLevels, 0, D3DFMT_UNKNOWN, 
		//	D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);

		HRESULT hr = font->m_pDynRender->CreateTextureFromFile( str.c_str(), page );

		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to load font page '%s' (%X)\n"), str.c_str(), hr);
		}
		font->m_FontPages[id] = page;
	}

	void CFontLoader::SetFontInfo(int outlineThickness)
	{
		this->outlineThickness = outlineThickness;
	}

	void CFontLoader::SetCommonInfo(short fontHeight, short base, short scaleW, short scaleH, int pages, bool isPacked)
	{
		font->m_FontHeight = fontHeight;
		font->m_Base = base;
		font->m_ScaleW = scaleW;
		font->m_ScaleH = scaleH;
		font->m_FontPages.resize(pages);
		for( int n = 0; n < pages; n++ )
			font->m_FontPages[n].m_pDxTex = 0;

		if( isPacked && outlineThickness )
			font->m_HasOutline = true;
	}

	void CFontLoader::AddChar(int id, short x, short y, short w, short h, short xoffset, short yoffset, short xadvance, short page, int chnl)
	{
		// Convert to a 4 element vector
		// TODO: Does this depend on hardware? It probably does
		if     ( chnl == 1 ) chnl = 0x00010000;  // Blue channel
		else if( chnl == 2 ) chnl = 0x00000100;  // Green channel
		else if( chnl == 4 ) chnl = 0x00000001;  // Red channel
		else if( chnl == 8 ) chnl = 0x01000000;  // Alpha channel
		else chnl = 0;

		if( id >= 0 )
		{
			SCharDescr *ch = DNew SCharDescr;
			ch->srcX = x;
			ch->srcY = y;
			ch->srcW = w;
			ch->srcH = h;
			ch->xOff = xoffset;
			ch->yOff = yoffset;
			ch->xAdv = xadvance;
			ch->page = page;
			ch->chnl = chnl;

			font->m_Chars.insert(CharMap_t::value_type(id, ch));
		}

		if( id == -1 )
		{
			SCharDescr& defChar = font->m_DefChar;
			defChar.srcX = x;
			defChar.srcY = y;
			defChar.srcW = w;
			defChar.srcH = h;
			defChar.xOff = xoffset;
			defChar.yOff = yoffset;
			defChar.xAdv = xadvance;
			defChar.page = page;
			defChar.chnl = chnl;
		}
	}

	void CFontLoader::AddKerningPair(SChar first, SChar second, int amount)
	{
		if( first >= 0 && first < 256 && font->m_Chars[first] )
		{
			font->m_Chars[first]->kerningPairs.push_back(second);
			font->m_Chars[first]->kerningPairs.push_back(amount);
		}
	}

	//=============================================================================
	// CFontLoaderTextFormat
	//
	// This class implements the logic for loading a BMFont file in text format
	//=============================================================================

	CFontLoaderTextFormat::CFontLoaderTextFormat(FILE *f, CFont *font, const TCHAR *fontFile) : CFontLoader(f, font, fontFile)
	{
	}

	int CFontLoaderTextFormat::Load()
	{
		wstring line;

		while( !feof(f) )
		{
			// Read until line feed (or EOF)
			line = L"";
			line.reserve(256);
			while( !feof(f) )
			{
				char ch;
				if( fread(&ch, 1, 1, f) )
				{
					if( ch != '\n' ) 
						line += ch; 
					else
						break;
				}
			}

			// Skip white spaces
			int pos = SkipWhiteSpace(line, 0);

			// Read token
			int pos2 = FindEndOfToken(line, pos);
			wstring token = line.substr(pos, pos2-pos);

			// Interpret line
			if( token == L"info" )
				InterpretInfo(line, pos2);
			else if( token == L"common" )
				InterpretCommon(line, pos2);
			else if( token == L"char" )
				InterpretChar(line, pos2);
			else if( token == L"kerning" )
				InterpretKerning(line, pos2);
			else if( token == L"page" )
				InterpretPage(line, pos2, fontFile);
		}

		fclose(f);

		// Success
		return 0;
	}

	int CFontLoaderTextFormat::SkipWhiteSpace(wstring &str, int start)
	{
		UINT n = start;
		while( n < str.size() )
		{
			TCHAR ch = str[n];
			if( ch != ' ' && 
				ch != '\t' && 
				ch != '\r' && 
				ch != '\n' )
				break;

			++n;
		}

		return n;
	}

	int CFontLoaderTextFormat::FindEndOfToken(wstring &str, int start)
	{
		UINT n = start;
		if( n < str.size() && str[n] == '"' )
		{
			n++;
			while( n < str.size() )
			{
				TCHAR ch = str[n];
				if( ch == '"' )
				{
					// Include the last quote char in the token
					++n;
					break;
				}
				++n;
			}
		}
		else
		{
			while( n < str.size() )
			{
				TCHAR ch = str[n];
				if( ch == ' ' ||
					ch == '\t' ||
					ch == '\r' ||
					ch == '\n' ||
					ch == '=' )
					break;

				++n;
			}
		}

		return n;
	}

	void CFontLoaderTextFormat::InterpretKerning(wstring &str, int start)
	{
		// Read the attributes
		acGraphics::SChar first = 0;
		acGraphics::SChar second = 0;
		int amount = 0;

		int pos, pos2 = start;
		pos = SkipWhiteSpace(str, pos2);
		while( true )
		{
			pos2 = FindEndOfToken(str, pos);

			wstring token = str.substr(pos, pos2-pos);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() || str[pos] != '=' ) break;

			pos = SkipWhiteSpace(str, pos+1);
			pos2 = FindEndOfToken(str, pos);

			wstring value = str.substr(pos, pos2-pos);

			if( token == L"first" )
				first = (acGraphics::SChar)_tcstol(value.c_str(), 0, 10);
			else if( token == L"second" )
				second = (acGraphics::SChar)_tcstol(value.c_str(), 0, 10);
			else if( token == L"amount" )
				amount = _tcstol(value.c_str(), 0, 10);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() ) break;
		}

		// Store the attributes
		AddKerningPair(first, second, amount);
	}

	void CFontLoaderTextFormat::InterpretChar(wstring &str, int start)
	{
		// Read all attributes
		int id = 0;
		short x = 0;
		short y = 0;
		short width = 0;
		short height = 0;
		short xoffset = 0;
		short yoffset = 0;
		short xadvance = 0;
		short page = 0;
		int chnl = 0;

		int pos, pos2 = start;
		pos = SkipWhiteSpace(str, pos2);
		while( true )
		{
			pos2 = FindEndOfToken(str, pos);

			wstring token = str.substr(pos, pos2-pos);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() || str[pos] != '=' ) break;

			pos = SkipWhiteSpace(str, pos+1);
			pos2 = FindEndOfToken(str, pos);

			wstring value = str.substr(pos, pos2-pos);

			if( token == L"id" )
				id = _tcstol(value.c_str(), 0, 10);
			else if( token == L"x" )
				x = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"y" )
				y = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"width" )
				width = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"height" )
				height = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"xoffset" )
				xoffset = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"yoffset" )
				yoffset = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"xadvance" )
				xadvance = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"page" )
				page = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"chnl" )
				chnl = _tcstol(value.c_str(), 0, 10);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() ) break;
		}

		// Store the attributes
		AddChar(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl);
	}

	void CFontLoaderTextFormat::InterpretCommon(wstring &str, int start)
	{
		short fontHeight = 0x00;
		short base = 0x00;
		short scaleW = 0x00;
		short scaleH = 0x00;
		int pages = 0x00;
		int packed = 0x00;

		// Read all attributes
		int pos, pos2 = start;
		pos = SkipWhiteSpace(str, pos2);
		while( true )
		{
			pos2 = FindEndOfToken(str, pos);

			wstring token = str.substr(pos, pos2-pos);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() || str[pos] != '=' ) break;

			pos = SkipWhiteSpace(str, pos+1);
			pos2 = FindEndOfToken(str, pos);

			wstring value = str.substr(pos, pos2-pos);

			if( token == L"lineHeight" )
				fontHeight = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"base" )
				base = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"scaleW" )
				scaleW = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"scaleH" )
				scaleH = (short)_tcstol(value.c_str(), 0, 10);
			else if( token == L"pages" )
				pages = _tcstol(value.c_str(), 0, 10);
			else if( token == L"packed" )
				packed = _tcstol(value.c_str(), 0, 10);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() ) break;
		}

		SetCommonInfo(fontHeight, base, scaleW, scaleH, pages, packed ? true : false);
	}

	void CFontLoaderTextFormat::InterpretInfo(wstring &str, int start)
	{
		int outlineThickness = 0x00;

		// Read all attributes
		int pos, pos2 = start;
		pos = SkipWhiteSpace(str, pos2);
		while( true )
		{
			pos2 = FindEndOfToken(str, pos);

			wstring token = str.substr(pos, pos2-pos);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() || str[pos] != '=' ) break;

			pos = SkipWhiteSpace(str, pos+1);
			pos2 = FindEndOfToken(str, pos);

			wstring value = str.substr(pos, pos2-pos);

			if( token == L"outline" )
				outlineThickness = (short)_tcstol(value.c_str(), 0, 10);

			pos = SkipWhiteSpace(str, pos);
			if( pos >= (int)str.size() )
				break;
		}

		SetFontInfo(outlineThickness);
	}

	void CFontLoaderTextFormat::InterpretPage(wstring &str, int start, const TCHAR *fontFile)
	{
		int id = 0;
		wstring file;

		// Read all attributes
		int pos, pos2 = start;
		pos = SkipWhiteSpace(str, pos2);
		while( true )
		{
			pos2 = FindEndOfToken(str, pos);

			wstring token = str.substr(pos, pos2-pos);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() || str[pos] != '=' ) break;

			pos = SkipWhiteSpace(str, pos+1);
			pos2 = FindEndOfToken(str, pos);

			wstring value = str.substr(pos, pos2-pos);

			if( token == L"id" )
				id = _tcstol(value.c_str(), 0, 10);
			else if( token == L"file" )
				file = value.substr(1, value.length()-2);

			pos = SkipWhiteSpace(str, pos2);
			if( pos == str.size() ) break;
		}

		LoadPage(id, file.c_str(), fontFile);
	}

	//=============================================================================
	// CFontLoaderBinaryFormat
	//
	// This class implements the logic for loading a BMFont file in binary format
	//=============================================================================

	CFontLoaderBinaryFormat::CFontLoaderBinaryFormat(FILE *f, CFont *font, const TCHAR *fontFile) : CFontLoader(f, font, fontFile)
	{
	}

	int CFontLoaderBinaryFormat::Load()
	{
		// Read and validate the tag. It should be 66, 77, 70, 2, 
		// or 'BMF' and 2 where the number is the file version.
		char magicString[4];
		fread(magicString, 4, 1, f);
		if( strncmp(magicString, "BMF\003", 4) != 0 )
		{
			DEBUG_MESSAGE(_T("Unrecognized format for '%s'"), fontFile);
			fclose(f);
			return -1;
		}

		// Read each block
		char blockType;
		int blockSize;
		while( fread(&blockType, 1, 1, f) )
		{
			// Read the blockSize
			fread(&blockSize, 4, 1, f);

			switch( blockType )
			{
			case 1: // info
				ReadInfoBlock(blockSize);
				break;
			case 2: // common
				ReadCommonBlock(blockSize);
				break;
			case 3: // pages
				ReadPagesBlock(blockSize);
				break;
			case 4: // chars
				ReadCharsBlock(blockSize);
				break;
			case 5: // kerning pairs
				ReadKerningPairsBlock(blockSize);
				break;
			default:
				DEBUG_MESSAGE(_T("Unexpected block type (%d)"), blockType);
				fclose(f);
				return -1;
			}
		}

		fclose(f);

		// Success
		return 0;
	}

	void CFontLoaderBinaryFormat::ReadInfoBlock(int size)
	{
#pragma pack(push, 1)
		struct infoBlock
		{
			WORD fontSize;
			BYTE reserved:4;
			BYTE bold    :1;
			BYTE italic  :1;
			BYTE unicode :1;
			BYTE smooth  :1;
			BYTE charSet;
			WORD stretchH;
			BYTE aa;
			BYTE paddingUp;
			BYTE paddingRight;
			BYTE paddingDown;
			BYTE paddingLeft;
			BYTE spacingHoriz;
			BYTE spacingVert;
			BYTE outline;         // Added with version 2
			char fontName[1];
		};
#pragma pack(pop)

		char *buffer = DNew char[size];
		fread(buffer, size, 1, f);

		// We're only interested in the outline thickness
		infoBlock *blk = (infoBlock*)buffer;
		SetFontInfo(blk->outline);

		delete[] buffer;
	}

	void CFontLoaderBinaryFormat::ReadCommonBlock(int size)
	{
#pragma pack(push, 1)
		struct commonBlock
		{
			WORD lineHeight;
			WORD base;
			WORD scaleW;
			WORD scaleH;
			WORD pages;
			BYTE packed  :1;
			BYTE reserved:7;
			BYTE alphaChnl;
			BYTE redChnl;
			BYTE greenChnl;
			BYTE blueChnl;
		}; 
#pragma pack(pop)

		char *buffer = DNew char[size];
		fread(buffer, size, 1, f);

		commonBlock *blk = (commonBlock*)buffer;

		SetCommonInfo(blk->lineHeight, blk->base, blk->scaleW, blk->scaleH, blk->pages, blk->packed ? true : false);

		delete[] buffer;
	}

	void CFontLoaderBinaryFormat::ReadPagesBlock(int size)
	{
#pragma pack(push, 1)
		struct pagesBlock
		{
			TCHAR pageNames[1];
		};
#pragma pack(pop)

		char *buffer = DNew char[size];
		fread(buffer, size, 1, f);

		pagesBlock *blk = (pagesBlock*)buffer;

		for( int id = 0, pos = 0; pos < size; id++ )
		{
			LoadPage(id, &blk->pageNames[pos], fontFile);
			pos += 1 + (int)_tcslen(&blk->pageNames[pos]);
		}

		delete[] buffer;
	}

	void CFontLoaderBinaryFormat::ReadCharsBlock(int size)
	{
#pragma pack(push, 1)
		struct charsBlock
		{
			struct charInfo
			{
				DWORD id;
				WORD  x;
				WORD  y;
				WORD  width;
				WORD  height;
				short xoffset;
				short yoffset;
				short xadvance;
				BYTE  page;
				BYTE  chnl;
			} chars[1];
		};
#pragma pack(pop)

		char *buffer = DNew char[size];
		fread(buffer, size, 1, f);

		charsBlock *blk = (charsBlock*)buffer;

		for( int n = 0; int(n*sizeof(charsBlock::charInfo)) < size; n++ )
		{
			AddChar(blk->chars[n].id,
				blk->chars[n].x,
				blk->chars[n].y,
				blk->chars[n].width,
				blk->chars[n].height,
				blk->chars[n].xoffset,
				blk->chars[n].yoffset,
				blk->chars[n].xadvance,
				blk->chars[n].page,
				blk->chars[n].chnl);
		}

		delete[] buffer;
	}

	void CFontLoaderBinaryFormat::ReadKerningPairsBlock(int size)
	{
#pragma pack(push, 1)
		struct kerningPairsBlock
		{
			struct kerningPair
			{
				SChar first;
				SChar second;
				short amount;
			} kerningPairs[1];
		};
#pragma pack(pop)

		char *buffer = DNew char[size];
		fread(buffer, size, 1, f);

		kerningPairsBlock *blk = (kerningPairsBlock*)buffer;

		for( int n = 0; int(n*sizeof(kerningPairsBlock::kerningPair)) < size; n++ )
		{
			AddKerningPair(blk->kerningPairs[n].first,
				blk->kerningPairs[n].second,
				blk->kerningPairs[n].amount);
		}

		delete[] buffer;
	}

} // end namespace

// 2008-05-11 Storing the characters in a map instead of an array
// 2008-05-11 Loading the new binary format for BMFont v1.10
// 2008-05-17 Added support for writing text with UTF8 and UTF16 encoding
