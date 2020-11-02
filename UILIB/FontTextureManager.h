#pragma once
#include "../CommonUtils/Singleton.h"
#include <map>

struct FontTexture;
namespace iz3d{
	namespace graphicUI{
		class FontTextureManager : public iz3d::Singleton<FontTextureManager>
		{
		public:
			bool AddFont( const LOGFONT* pLogFont );
			const void* getFontTextureBinaryData() const;
			size_t getFontTextureSize() const;
			void SetCurFont( const LOGFONT* pLogFont );
			RECT GetCharRect( const wchar_t ch ) const;
			void CalculateTextSize( LPCTSTR text, size_t len, SIZE& textSize );
			size_t CalculateLineSize( LPCTSTR text, size_t len, SIZE& lineSize );
		private:
			FontTextureManager();
			virtual ~FontTextureManager();
			typedef std::map< const LOGFONT*, FontTexture* > TFonts;
			FontTexture* m_pCurTexture;
			TFonts m_Manager;
			friend class iz3d::Singleton<FontTextureManager>;
		};
	}
}