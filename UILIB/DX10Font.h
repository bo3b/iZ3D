#pragma once
#include "..\CommonUtils\SelfDeleter.h"
#include "CommonFont.h"
#include "FontTextureManager.h"
#include "DX10Texture.h"
class DX10Font : public Deletable< DX10Font, ICommonFont >
{
	const LOGFONT* m_pFont;
	typedef iz3d::graphicUI::FontTextureManager TFontManager; 
	typedef iz3d::utils::MemFunction2<ICommonTexture::TDrawFunction::TReturnType,
		DX10Texture,
		ICommonTexture::TDrawFunction::TParam1,
		ICommonTexture::TDrawFunction::TParam2>
		TFuncImpl;
	void DrawSymbol( const RECT& fromRect, const RECT& toRect );
	DX10Texture* m_pFontTexture;
public:
	DX10Font( const LOGFONT& Font, DX10Texture* pTexture );
	virtual ~DX10Font(void);
	virtual HRESULT TextOut( LPCTSTR text,
		int length,
		RECT* pRect,
		DWORD Format,
		DWORD Colour );
};

