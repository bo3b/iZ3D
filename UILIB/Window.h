#pragma once

class CXWindow : public CXControl
{
private:
    long m_X;
	long m_Y;
	bool m_Depressed;
public:
	CXWindow(iz3d::uilib::CommonDevice* Device);
	virtual bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
};
