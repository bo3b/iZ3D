#pragma once

class CXDropDownList : public CXControl
{
private:
protected:
	CXListBox* m_pListBox;
	CXTextBox* m_pTextBox;
	CXButton* m_pButton;
	bool IsDroppedDown;
	bool TextBoxIntersect(int X, int Y);
	bool ButtonIntersect(int X, int Y);
	bool ListBoxIntersect(int X, int Y);
	bool ListItemIntersect(int X, int Y);
	LONG m_RolledUpHeight;
public:
	CXDropDownList(iz3d::uilib::CommonDevice* Device);
	virtual ~CXDropDownList();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	void LoadButtonImage(TCHAR* File);
	CXListItem* AddNewItem(TCHAR* Caption) {return m_pListBox->AddNewItem(Caption);}
	void ShowDropDownList(bool State);
	bool InitaliseDropDownList(LONG DropDownWidth,
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
							   TCHAR* ItemBackgroundSelected);
};

//---------------------------------------------------------------------------------
