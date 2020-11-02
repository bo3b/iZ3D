#include <d3d9.h>
#include <d3dx9.h>
#include "uilib.h"

CXListBox::CXListBox(iz3d::uilib::CommonDevice* Device) : CXControl(Device)
{
	m_AllowMultiSelect = false;
	m_pSelected = NULL;
	m_pBackground = NULL;
	m_pFont = NULL;
	m_pBar = new CXScrollBar(Device);
	m_ListFrame = 0;
	m_ItemCount = 0;
	m_pListItems = NULL;
	m_pSelectedItem = NULL;

	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
	SetFont(lf);
}

CXListBox::~CXListBox()
{
	ClearItems();

	if(m_pSelected)
		delete m_pSelected;

	if(m_pBackground)
		delete m_pBackground;

	/*if(m_pFont)
		m_pFont->Release();*/

	if(m_pBar)
		delete m_pBar;
}

bool CXListBox::OnRender()
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	if(GetVisible())
	{
		if(m_pBar)
		{
			m_pBar->SetXYPos(Abs.x + (GetWidth() - m_pBar->GetWidth()), Abs.y);
			m_pBar->OnRender();
		}

		if(m_pListItems) 
			PostToAllReverse((CXControl*)m_pListItems, WM_PAINT, 0, 0, NULL);
	}
	return true;
}

void CXListBox::OnMouseDown(int Button, int X, int Y)
{
	CXListItem* Selected = NULL;
	Selected = GetItemAtPos(X, Y);
	if(Selected)
		OnItemSelect(Selected, Button, X, Y);

	if(ScrollBarIntersect(X, Y))
		m_pBar->OnMouseDown(Button, X, Y);
}

void CXListBox::OnMouseMove(int X, int Y)
{
	m_pBar->OnMouseMove(X, Y);
}

void CXListBox::OnMouseUp(int Button, int X, int Y)
{
	if(ScrollBarIntersect(X, Y))
	{
		m_pBar->OnMouseUp(Button, X, Y);
		ScrollTo(m_pBar->GetValue());
	}
}

void CXListBox::OnSetFocus()
{
}

void CXListBox::OnLostFocus()
{
}

void CXListBox::OnKeyDown(WPARAM , LPARAM )
{
}

void CXListBox::OnKeyUp(WPARAM , LPARAM )
{

}

void CXListBox::OnSysKey(WPARAM Key, LPARAM )
{
	if (Key != VK_UP && Key != VK_DOWN)
		return;

	CXListItem* pBuf = GetSelected();
	if (!pBuf)
		return;
	if (Key == VK_UP)
		pBuf = (CXListItem*)pBuf->GetPreviousSibling();
	else
		pBuf = (CXListItem*)pBuf->GetNextSibling();
	if (!pBuf)
		return;
	OnItemSelect(pBuf, 0, 0, 0);
}

CXListItem* CXListBox::AddNewItem(TCHAR* Caption)
{
	if((m_pBackground) && (m_pSelected))
	{

		D3DXIMAGE_INFO Info;
		CXListItem* Item; //Item To Add

		Item = new CXListItem(GetDevice()); //Create New Item
		ZeroMemory(&Info, sizeof(D3DXIMAGE_INFO));
		D3DXGetImageInfoFromFile(m_pBackground->GetPath(), &Info);

		if(Item)
		{
			Item->SetParentControl(this);
			CXPen* Pen = Item->GetPen();

			if(Pen)
				delete Pen;

			Item->SetPen(GetPen()); //Share Parent CXPen

			if(!m_pListItems)
			{
				m_pListItems = Item;
				Item->SetXYPos(0,0);
			}
			else 
			{
				CXListItem* Temp = m_pListItems;
				LONG Height = 0;

				while(Temp->GetNextSibling())
				{
					Height += Temp->GetHeight();
					Temp = (CXListItem*) Temp->GetNextSibling();
				}

				Temp->SetNextSibling((CXControl*) Item);
				Item->SetPreviousSibling((CXControl*) Temp);


				Item->SetXYPos(0,Height + Temp->GetHeight());
			}


			Item->SetItemSize(GetWidth() - m_pBar->GetWidth(), Info.Height);
			Item->SetCaption(Caption);
			m_ItemCount++;
			m_ListFrame = ComputeListFrame(); //How many items can be displayed?
			ScrollTo(0);

			return Item;
		}
	}
	return NULL;
}

void CXListBox::OnItemSelect(CXListItem* Item, int , int , int )
{
	if(!m_AllowMultiSelect)
	{
		CXListItem* CurrentItem = m_pListItems;

		while(CurrentItem)
		{
			CurrentItem->SetSelect(false);
			CurrentItem = (CXListItem*) CurrentItem->GetNextSibling();  
		}
		Item->SetSelect(true);
		m_pSelectedItem = Item;
	}
}

void CXListBox::LoadItemBackgrounds(TCHAR* Default, TCHAR* Selected)
{
	if(!m_pBackground)
	{
		m_pBackground = new CXCanvas(GetDevice());
		m_pBackground->LoadFromFile(Default);
	}

	if(!m_pSelected)
	{
		m_pSelected = new CXCanvas(GetDevice());
		m_pSelected->LoadFromFile(Selected);

	}
}

void CXListBox::SetFont(LOGFONT Font)
{
	/*if(m_pFont)
		m_pFont->Release();*/

	/*D3DXCreateFont(GetDevice(), Font.lfWidth, Font.lfHeight, Font.lfWeight, 1, 
		Font.lfItalic, Font.lfCharSet, Font.lfOutPrecision, Font.lfQuality,
		Font.lfPitchAndFamily, Font.lfFaceName, &m_pFont);*/
	GetDevice()->CreateFont( Font, &m_pFont );
}

void CXListBox::RemoveListItem(CXListItem* Item)
{
	if(Item)
		RemoveChildControl((CXControl*) Item);

	m_ItemCount--;
}

void CXListBox::LoadScrollBar(TCHAR* Background, TCHAR* Thumb, TCHAR* TopArrow, TCHAR* BottomArrow)
{
	if(m_pBar)
	{
		m_pBar->LoadThumbImage(Thumb);	
		m_pBar->LoadBackground(Background);
		m_pBar->LoadLeftBottomArrow(BottomArrow);
		m_pBar->LoadRightTopArrow(TopArrow);
		m_pBar->SetWidthHeight(32 , GetHeight());
		m_pBar->SetXYPos(GetWidth() - m_pBar->GetWidth(), 0);
		m_pBar->SetValue(0);
	}
}

CXListItem* CXListBox::GetItemByIndex(long Index)
{
	long Counter = 0;

	CXListItem* CurrentItem = m_pListItems;

	while(CurrentItem)
	{
		if(Counter == Index)
			return CurrentItem;

		Counter++;
		CurrentItem = (CXListItem*) CurrentItem->GetNextSibling();  
	}

	return NULL;
}

void CXListBox::ScrollTo(long Value)
{
	if(m_ItemCount > m_ListFrame) //Can it scroll?
		if((Value + m_ListFrame) <= m_ItemCount)
		{
			//Get First Item
			CXListItem* CurrentItem = m_pListItems;

			long Counter = 0;
			LONG AccumilativeYPos = 0;

			while(CurrentItem)
			{

				CurrentItem->SetSelect(false);

				if((Counter >= Value) && (Counter < Value + m_ListFrame)) //Is it visible?
				{
					CurrentItem->SetVisible(true);
					CurrentItem->SetXYPos(0, AccumilativeYPos);
					AccumilativeYPos+= CurrentItem->GetHeight();

				}
				else
					CurrentItem->SetVisible(false);


				Counter++;


				CurrentItem = (CXListItem*) CurrentItem->GetNextSibling();  
			}
		}
}

long CXListBox::ComputeListFrame()
{
	if(m_pListItems)
	{
		long Frame = (long) GetHeight() / m_pListItems->GetHeight();
		long Max = (m_ItemCount+1) - Frame;

		if(Max > 0)
			m_pBar->SetMinMax(0, Max);
		else
			m_pBar->SetMinMax(0, 1);

		return Frame;
	}
	else
	{
		m_pBar->SetMinMax(0, 1);
		return 0;
	}
}

void CXListBox::ClearItems()
{

	CXListItem* Temp = m_pListItems;

	while(Temp)
	{
		CXListItem* Next = (CXListItem*) Temp->GetNextSibling();

		if(Temp)
			delete Temp;

		Temp = Next;
	}

	m_pListItems = NULL;
	m_ItemCount = 0;
}

CXListItem* CXListBox::GetItemAtPos(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);


	CXListItem* CurrentItem = m_pListItems;

	while(CurrentItem)
	{
		if(CurrentItem->GetVisible())
			if((X >=  Abs.x) && (X <= Abs.x + (GetWidth() - m_pBar->GetWidth())))
				if((Y >= Abs.y + CurrentItem->GetYPos()) && (Y <= Abs.y + CurrentItem->GetYPos() + CurrentItem->GetHeight()))
					return CurrentItem;

		CurrentItem = (CXListItem*) CurrentItem->GetNextSibling();  
	}

	return NULL;
}

bool CXListBox::ScrollBarIntersect(int X, int Y)
{
	POINT Abs;
	ZeroMemory(&Abs, sizeof(D3DXVECTOR2));
	GetAbsolutePosition(&Abs);

	int LeftRangePos = Abs.x + (GetWidth() - m_pBar->GetWidth());

	if((X >= LeftRangePos) && (X <= Abs.x + GetWidth()))
		if((Y >= Abs.y) && (Y <= Abs.y + m_pBar->GetHeight()))
			return true;

	return false;
}