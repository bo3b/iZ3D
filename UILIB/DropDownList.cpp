#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXDropDownList::CXDropDownList(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	LOGFONT lf;

	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);

	m_pListBox = new CXListBox(Device);
	m_pTextBox = new CXTextBox(lf, Device);
	m_pButton = new CXButton(Device);

	m_RolledUpHeight = 0;
}

CXDropDownList::~CXDropDownList()
{
	if(m_pListBox)
		delete m_pListBox;

	if(m_pTextBox)
		delete m_pTextBox;

	if(m_pButton)
		delete m_pButton;
}

bool CXDropDownList::OnRender()
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	m_pTextBox->SetXYPos(Abs.x, Abs.y);
	m_pTextBox->OnRender();

	m_pButton->SetXYPos(Abs.x + (GetWidth()-m_pButton->GetWidth()), Abs.y);
	m_pButton->OnRender();

	m_pListBox->SetXYPos(Abs.x, Abs.y + m_RolledUpHeight);
	m_pListBox->OnRender();

	return true;
}

void CXDropDownList::OnMouseDown(int Button, int X, int Y)
{
	if(TextBoxIntersect(X,Y))
	{
		m_pTextBox->OnMouseDown(Button, X, Y);
		ShowDropDownList(false);
		return;
	}

	if(ButtonIntersect(X,Y))
	{
		m_pButton->OnMouseDown(Button, X, Y);
		ShowDropDownList(true);
	}

	if(ListBoxIntersect(X,Y))
	{
		m_pListBox->OnMouseDown(Button, X, Y);


		if(ListItemIntersect(X,Y))
		{  
			if(m_pListBox->GetSelected())
			{
				TCHAR* Text = (TCHAR*) m_pListBox->GetSelected()->GetCaption();
				m_pTextBox->SetText(Text);
			}

			ShowDropDownList(false);
		}
	}

}

void CXDropDownList::OnMouseMove(int X, int Y)
{
	m_pListBox->OnMouseMove(X,Y);
	m_pButton->OnMouseMove(X,Y);
	m_pTextBox->OnMouseMove(X,Y);
}

void CXDropDownList::OnMouseUp(int Button, int X, int Y)
{
	if(ListBoxIntersect(X,Y))
		m_pListBox->OnMouseUp(Button, X, Y);

	if(ButtonIntersect(X,Y))
		m_pButton->OnMouseUp(Button, X, Y);
}

void CXDropDownList::OnSetFocus()
{
}

void CXDropDownList::OnLostFocus()
{
}

void CXDropDownList::OnKeyDown(WPARAM Key, LPARAM Extended)
{
	ShowDropDownList(false);
	m_pTextBox->OnKeyDown(Key, Extended);
}

void CXDropDownList::OnKeyUp(WPARAM Key, LPARAM Extended)
{
	ShowDropDownList(false);
	m_pTextBox->OnKeyUp(Key, Extended);
}

void CXDropDownList::OnSysKey(WPARAM Key, LPARAM Extended)
{
	ShowDropDownList(false);
	m_pTextBox->OnSysKey(Key, Extended);
}

void CXDropDownList::LoadButtonImage(TCHAR* )
{
}

void CXDropDownList::ShowDropDownList(bool State)
{
	m_pListBox->SetVisible(State);
	IsDroppedDown = State;

	if(State)
		SetWidthHeight(GetWidth(), m_RolledUpHeight + m_pListBox->GetHeight());
	else
		SetWidthHeight(GetWidth(), m_RolledUpHeight);
}

bool CXDropDownList::InitaliseDropDownList(LONG DropDownWidth,
										   LONG DropDownHeight,
										   LONG ListHeight,
										   LONG ButtonWidth,
										   TCHAR* ScrollThumb, 
										   TCHAR* ScrollTopArrow, 
										   TCHAR* ScrollBottomArrow, 
										   TCHAR* ScrollBackgroundDefault, 
										   TCHAR* ButtonBackgroundDefault,
										   TCHAR* ButtonBackgroundSelected,
										   TCHAR* ItemBackgroundDefault,
										   TCHAR* ItemBackgroundSelected)

{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	SetWidthHeight(DropDownWidth, DropDownHeight);
	m_RolledUpHeight = DropDownHeight;

	m_pListBox->SetXYPos(Abs.x, Abs.y + DropDownHeight);
	m_pListBox->SetWidthHeight(DropDownWidth, ListHeight);

	m_pListBox->LoadScrollBar(ScrollBackgroundDefault,
		ScrollThumb, ScrollTopArrow, ScrollBottomArrow);

	m_pListBox->LoadItemBackgrounds(ItemBackgroundDefault, ItemBackgroundSelected);

	m_pListBox->SetVisible(false);

	m_pButton->SetXYPos(Abs.x + (DropDownWidth-ButtonWidth), Abs.y);
	m_pButton->SetWidthHeight(ButtonWidth, ListHeight);

	m_pButton->SetDefaultImage(ButtonBackgroundDefault);
	m_pButton->SetPressedImage(ButtonBackgroundSelected);

	m_pTextBox->SetXYPos(Abs.x, Abs.y);
	m_pTextBox->SetWidthHeight(DropDownWidth-ButtonWidth, DropDownHeight);
	m_pTextBox->SetFocus(true);

	return true;
}

bool CXDropDownList::TextBoxIntersect(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if((X >= Abs.x) && (X <= Abs.x + (GetWidth() - m_pButton->GetWidth())))
		if((Y >= Abs.y) && (Y <= Abs.y + m_RolledUpHeight))
			return true;

	return false;
}

bool CXDropDownList::ButtonIntersect(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if((X >= Abs.x + (GetWidth() - m_pButton->GetWidth())) && (Abs.x + GetWidth()))
		if((Y >= Abs.y) && (Y <= Abs.y + m_RolledUpHeight))
			return true;

	return false;
}

bool CXDropDownList::ListBoxIntersect(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if((X >= Abs.x) && (X <= Abs.x + GetWidth()))
		if((Y >= Abs.y + m_RolledUpHeight) && (Y <= Abs.y + GetHeight()))
			return true;

	return false;
}

bool CXDropDownList::ListItemIntersect(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if((X >= Abs.x) && (X <= Abs.x + m_pListBox->GetListWidth()))
		if((Y >= Abs.y + m_RolledUpHeight) && (Y <= Abs.y + GetHeight()))
			return true;

	return false;
}