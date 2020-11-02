#pragma once

class CXListItem : public CXControl
{
private:
protected:
	FLOAT m_StretchWidthFactor;
	FLOAT m_StretchHeightFactor;
	bool IsSelected;
	std::basic_string<TCHAR> m_Caption;
public:
	CXListItem(iz3d::uilib::CommonDevice* Device);
	virtual ~CXListItem();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	void LoadItemIcon(TCHAR* File);
	void SetSelect(bool State) {IsSelected = State;}
	void SetItemSize(LONG Width, LONG Height);
	void SetCaption(TCHAR* Caption) {m_Caption = Caption;}
	const TCHAR* GetCaption() {return m_Caption.c_str();}
};