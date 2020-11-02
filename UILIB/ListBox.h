#pragma once

class CXListBox : public CXControl
{
protected:
	CXListItem* m_pListItems;
	CXListItem* m_pSelectedItem;
	CXCanvas* m_pSelected;
	CXCanvas* m_pBackground;
	ICommonFont* m_pFont;
	CXScrollBar* m_pBar;
	bool m_AllowMultiSelect;
	long m_ListFrame;
	long m_ItemCount;
	long ComputeListFrame();
public:
	CXListBox(iz3d::uilib::CommonDevice* Device);
	virtual ~CXListBox();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	void OnItemSelect(CXListItem* Item, int Button, int X, int Y);
	void LoadItemBackgrounds(TCHAR* Default, TCHAR* Selected);
	CXListItem* AddNewItem(TCHAR* Caption);
	void RemoveListItem(CXListItem* Item);
	CXCanvas* GetBackgroundImage() {return m_pBackground;}
	CXCanvas* GetSelectedImage() {return m_pSelected;}
	void SetFont(LOGFONT Font);
	ICommonFont* GetFont() {return m_pFont;}
	void LoadScrollBar(TCHAR* Background, TCHAR* Thumb, TCHAR* TopArrow, TCHAR* BottomArrow);
	CXListItem* GetItemAtPos(int X, int Y);
	CXListItem* GetItemByIndex(long Index);
	void ScrollTo(long Value);
	void ClearItems();
	bool ScrollBarIntersect(int X, int Y);
	CXListItem* GetSelected() {return m_pSelectedItem;}
	LONG GetListWidth() {return GetWidth() - m_pBar->GetWidth();}
};
