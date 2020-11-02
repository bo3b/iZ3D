#pragma once

class CXScrollBar : public CXControl
{
private:
protected:
	int m_Min;
	int m_Max;
	int m_ThumbPosition;
	int m_Change;
	LONG m_BackgroundHeight;
	bool m_DragMode;
	CXButton* m_pThumb;
	POINT m_ThumbPos;
	POINTF m_ThumbScale;
	LONG m_ThumbHeight;
	CXButton* m_pRightTopArrow;
	CXButton* m_pLeftBottomArrow;
	FLOAT AutoSizeThumb();
	FLOAT AutoSizeBackground();
	void UpdateScaling();
public:
	CXScrollBar(iz3d::uilib::CommonDevice* Device);
	virtual ~CXScrollBar();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	void LoadThumbImage(TCHAR* File);
	void LoadRightTopArrow(TCHAR* File);
	void LoadLeftBottomArrow(TCHAR* File);
	void SetValue(int Value);
	int GetValue() {return m_ThumbPosition;}
	int GetPosValue(LONG Pos);
	void SetMinMax(int Min, int Max);
	void LoadBackground(TCHAR* File);
	void SetChange (int Change) {m_Change = Change;}
};
