#pragma once
#include <WTypes.h>

struct ICommonFont
{
	virtual HRESULT TextOut( LPCTSTR text,
		int length,
		RECT* pRect,
		DWORD Format,
		DWORD Colour ) = 0;
};