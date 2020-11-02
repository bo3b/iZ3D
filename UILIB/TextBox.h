#pragma once

class CXTextBox : public CXControl
{
protected:
	std::basic_string<TCHAR> m_Text;
	ICommonFont* m_pFont;

//	LPD3DXLINE m_pCaretLine;
	ICommonLine* m_pCaretLine;
	POINTF m_CaretVector[2];
	bool m_CaretVisible;
	long CaretCharPos;

	FLOAT m_TextWidth;
public:
	CXTextBox(LOGFONT Font, iz3d::uilib::CommonDevice* Device);
	virtual ~CXTextBox();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	const TCHAR* GetText() const {return m_Text.c_str();}
	void SetText(TCHAR* Text);
	long GetCharAtPos(int X, int Y);
	bool CursorIntersectChar(int X, int Y);
	FLOAT GetStringWidth(std::basic_string<TCHAR> String);
	FLOAT GetStringHeight(std::basic_string<TCHAR> String);
	long GetCaretPos() const {return CaretCharPos;}
	void SetCaretPos(size_t Pos);
	bool InsertText(TCHAR* Text);
	long RemoveText(long Quantity);
};