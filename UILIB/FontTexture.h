#pragma once
#include "../CommonUtils/StringResourceManager.h"
#include <windows.h>
#include <map>
namespace iz3d {
	namespace graphicUI {
		class FontTexture
		{
			typedef std::map< wchar_t, RECT > TCharRectMap;
			void* m_pData;
			size_t m_size;
			TCharRectMap m_CharMap;
			void CreateBinaryTextureFromFont( const LOGFONT& lf );
		public:
			FontTexture( const LOGFONT& lf );
			~FontTexture();

			const void* GetBinaryFontData() const;
			RECT GetCharRect( const wchar_t ch ) const;
			size_t GetBinaryFontSize() const;
			void CalculateTextSize( LPCTSTR text, size_t len, SIZE& textSize );
			size_t CalculateLineSize( LPCTSTR text, size_t len, SIZE& lineSize );
		};
	}
}