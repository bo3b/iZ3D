#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXLabel::CXLabel( const LOGFONT& Font, iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	/*D3DXCreateFont(Device, Font.lfWidth, Font.lfHeight, Font.lfWeight, 1, 
		Font.lfItalic, Font.lfCharSet, Font.lfOutPrecision, Font.lfQuality,
		Font.lfPitchAndFamily, Font.lfFaceName, &m_Font);*/
	GetDevice()->CreateFont( Font, &m_pFont );
	m_Caption = _T("");
	m_Colour = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_Format = DT_CENTER | DT_VCENTER;
}


CXLabel::~CXLabel()
{
	/*if(m_Font)
		m_Font->Release();*/
}


bool CXLabel::OnRender()
{
	if(m_pFont)
	{
		RECT Rectangle;
		POINT Vec2;
		Vec2.x = 0;
		Vec2.y = 0;
		GetAbsolutePosition(&Vec2);
		Rectangle.left = Vec2.x;
		Rectangle.top = Vec2.y;
		Rectangle.right = Vec2.x + GetWidth();
		Rectangle.bottom = Vec2.y + GetHeight();
		m_pFont->TextOut( m_Caption.c_str(), (INT)m_Caption.length(), 
			&Rectangle, m_Format, m_Colour);
	}

	return true;
}

void CXLabel::OnMouseDown(int , int , int )
{
}

void CXLabel::OnMouseMove(int , int )
{
}

void CXLabel::OnMouseUp(int , int , int )
{
}

void CXLabel::OnSetFocus()
{
}

void CXLabel::OnLostFocus()
{
}

void CXLabel::OnKeyDown(WPARAM , LPARAM )
{
}

void CXLabel::OnKeyUp(WPARAM , LPARAM )
{
}

void CXLabel::OnSysKey(WPARAM , LPARAM )
{
}