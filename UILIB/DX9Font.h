#pragma once
#include "CommonFont.h"
#include <d3dx9core.h>
#include "..\CommonUtils\SelfDeleter.h"
#include <atlcomcli.h>
class DX9Font : public Deletable< DX9Font, ICommonFont >
{
	ID3DXFont* m_pFont;
public:
	// c-tor
	DX9Font( ID3DXFont* pFont );
	virtual ~DX9Font();

	// Draws text
	virtual HRESULT TextOut( LPCTSTR text,
		int length,
		RECT* pRect,
		DWORD Format,
		DWORD Colour );
};