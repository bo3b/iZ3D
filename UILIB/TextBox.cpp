#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXTextBox::CXTextBox(LOGFONT Font, iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{	
	Device->CreateLine( & m_pCaretLine );
	Device->CreateFont( Font, &m_pFont );
	/*D3DXCreateLine(Device, &m_pCaretLine);

	D3DXCreateFont(Device, Font.lfWidth, Font.lfHeight, Font.lfWeight, 1, 
		Font.lfItalic, Font.lfCharSet, Font.lfOutPrecision, Font.lfQuality,
		Font.lfPitchAndFamily, Font.lfFaceName, &m_pFont);*/

	ZeroMemory(m_CaretVector, sizeof(D3DXVECTOR2) * 2);

	m_TextWidth = 0.0f;
	m_Text = _T("");
	m_CaretVisible = false;
	CaretCharPos = 0;
}

CXTextBox::~CXTextBox()
{
	/*if(m_pCaretLine)
		m_pCaretLine->Release();

	if(m_pFont)
		m_pFont->Release();*/
}

bool CXTextBox::OnRender()
{
	POINT Abs;
	RECT Rct;

	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));

	GetAbsolutePosition(&Abs);

	Rct.left = Abs.x;
	Rct.top = Abs.y;
	Rct.right = Abs.x + GetWidth();
	Rct.bottom = Abs.y + GetHeight();

	m_pFont->TextOut( m_Text.c_str(), (INT)m_Text.length(), 
		&Rct, DT_LEFT, D3DCOLOR_XRGB(255,255,255));

	if( GetFocus() )
		if(m_CaretVisible)
		{

			POINTF Absolute[2];
			ZeroMemory(Absolute, sizeof(POINTF) * 2);

			Absolute[0].x = (Abs.x + m_CaretVector[0].x);
			Absolute[0].y = (Abs.y + m_CaretVector[0].y);
			Absolute[1].x = (Abs.x + m_CaretVector[0].x);
			Absolute[1].y = (Abs.y + m_CaretVector[0].y + GetHeight());

			m_pCaretLine->Draw(Absolute, 2, D3DCOLOR_XRGB(255,255,255));

			m_CaretVisible = false;
		}
		else
			m_CaretVisible = true;


	return true;
}

void CXTextBox::OnMouseDown(int , int X, int Y)
{
	if( GetFocus() )
		if(CursorIntersectChar(X, Y))
			SetCaretPos(GetCharAtPos(X,Y));
}

void CXTextBox::OnMouseMove(int , int )
{
}

void CXTextBox::OnMouseUp(int , int , int )
{
}

void CXTextBox::OnSetFocus()
{
}

void CXTextBox::OnLostFocus()
{
}

void CXTextBox::OnKeyDown(WPARAM Key, LPARAM )
{
	if(Key != VK_BACK)
		InsertText((TCHAR*) &Key);
}

void CXTextBox::OnKeyUp(WPARAM , LPARAM )
{
}

void CXTextBox::OnSysKey(WPARAM Key, LPARAM )
{
	switch (Key)
	{
	case VK_BACK:	
	case VK_DELETE:
		{
			RemoveText(1);
		}
		break;

	case VK_LEFT:
		{
			SetCaretPos(GetCaretPos()-1);
		}
		break;

	case VK_RIGHT:
		{
			SetCaretPos(GetCaretPos()+1);
		}
		break;

	}
}

long CXTextBox::GetCharAtPos(int X, int )
{
	long Pos = 0;
	long Left = 0;
	long Right= 0;

	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	Left = (LONG)Abs.x;

	while(Pos < (long)m_Text.length())
	{

		std::basic_string<TCHAR> TmpStr = m_Text.substr(Pos, 1);

		Right = (LONG)GetStringWidth(TmpStr);

		if((X >= Left) && (X <= Left + Right))
			break;

		Pos++;
		Left += Right;
	}

	return Pos;
}

FLOAT CXTextBox::GetStringWidth(std::basic_string<TCHAR> String)
{
	RECT String_Info;
	ZeroMemory(&String_Info, sizeof(RECT)); 

	if(m_pFont)
		m_pFont->TextOut(String.c_str(), (INT)String.length(), &String_Info, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));

	return (FLOAT)String_Info.right;
}

FLOAT CXTextBox::GetStringHeight(std::basic_string<TCHAR> String)
{
	RECT String_Info;
	ZeroMemory(&String_Info, sizeof(RECT));

	if(m_pFont)
		m_pFont->TextOut(String.c_str(), (INT)String.length(), &String_Info, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));

	return (FLOAT)String_Info.bottom;
}

bool CXTextBox::CursorIntersectChar(int X, int Y)
{
	POINT Pos;

	this->GetAbsolutePosition(&Pos);

	if((X >= Pos.x) && (X <= Pos.x + m_TextWidth))
		if((Y >= Pos.y) && (Y <= Pos.y + GetHeight()))
			return true;

	return false;
} 

void CXTextBox::SetText(TCHAR* Text)
{
	m_Text.empty();
	m_Text = Text;
	m_TextWidth = GetStringWidth(m_Text);
	SetCaretPos(m_Text.length());
}

void CXTextBox::SetCaretPos(size_t Pos)
{
	if((Pos >= 0) && (Pos <= m_Text.length()))
	{
		std::basic_string<TCHAR> tmpstr = m_Text.substr(0, Pos);

		m_CaretVector[0].x = GetStringWidth(tmpstr);
		m_CaretVector[1].x = m_CaretVector[0].x;

		CaretCharPos = (long)Pos;
	}
}

bool CXTextBox::InsertText(TCHAR* Text)
{
	std::basic_string<TCHAR>	TmpStr = Text;
	if((m_TextWidth + GetStringWidth(TmpStr)) <= GetWidth())
	{
		m_Text.insert(CaretCharPos, Text);
		m_TextWidth = GetStringWidth(m_Text);
		SetCaretPos(GetCaretPos()+_tcslen(Text));
		return true;
	}

	return false;
}

long CXTextBox::RemoveText(long Quantity)
{
	SetCaretPos(GetCaretPos()-Quantity);
	m_Text.erase(CaretCharPos, Quantity);
	m_TextWidth = GetStringWidth(m_Text);
	return (long)m_TextWidth;
}