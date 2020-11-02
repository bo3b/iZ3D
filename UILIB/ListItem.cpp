#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXListItem::CXListItem(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	m_StretchWidthFactor = 1.0f;
	m_StretchHeightFactor = 1.0f;
	IsSelected = false;
	m_Caption = _T("");
}

//---------------------------------------------------------------------------------

CXListItem::~CXListItem()
{
}

bool CXListItem::OnRender()
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	POINTF Scaling;
	ZeroMemory(&Scaling, sizeof(POINTF));

	Scaling.x = m_StretchWidthFactor;
	Scaling.y = m_StretchHeightFactor;

	CXListBox* List = (CXListBox*) GetParentControl();

	if(IsSelected)
	{

		if(List->GetSelectedImage())
		{
			List->GetSelectedImage()->SetTranslation(Abs);
			List->GetSelectedImage()->SetScaling(Scaling);
			GetPen()->DrawTexture(List->GetSelectedImage());
		}
	}
	else
	{
		if(List->GetBackgroundImage())
		{
			List->GetBackgroundImage()->SetTranslation(Abs);
			List->GetBackgroundImage()->SetScaling(Scaling);
			GetPen()->DrawTexture(List->GetBackgroundImage());
		}
	}

	if(List->GetFont())
	{
		RECT Rct;
		ZeroMemory(&Rct, sizeof(RECT));
		Rct.left = Abs.x;
		Rct.top = Abs.y;
		Rct.right = Abs.x + GetWidth();
		Rct.bottom = Abs.y + GetHeight();
		List->GetFont()->TextOut(m_Caption.c_str(), (INT)m_Caption.length(), 
			&Rct, DT_LEFT, D3DCOLOR_XRGB(255,255,255));
	}

	return true;
}

void CXListItem::OnMouseDown(int Button, int X, int Y)
{
	IsSelected = true;
	CXListBox* List = (CXListBox*) GetParentControl();
	List->OnItemSelect(this, Button, X, Y);
}

void CXListItem::OnMouseMove(int , int )
{
}

void CXListItem::OnMouseUp(int , int , int )
{
}

void CXListItem::OnSetFocus()
{
}

void CXListItem::OnLostFocus()
{
}

void CXListItem::OnKeyDown(WPARAM , LPARAM )
{
}

void CXListItem::OnKeyUp(WPARAM , LPARAM )
{
}

void CXListItem::OnSysKey(WPARAM , LPARAM )
{
}

void CXListItem::LoadItemIcon(TCHAR* File)
{
	GetCanvas()->LoadFromFile(File);
}

void CXListItem::SetItemSize(LONG Width, LONG Height)
{
	if(GetParentControl())
	{
		D3DXIMAGE_INFO Info;
		CXListBox* List = NULL;

		ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
		List = (CXListBox*) GetParentControl();


		D3DXGetImageInfoFromFile(List->GetBackgroundImage()->GetPath(), &Info);

		m_StretchWidthFactor = (FLOAT)Width / Info.Width;
		m_StretchHeightFactor = (FLOAT)Height / Info.Height;

		SetWidthHeight(Width, Height);
	}
}
