#pragma once
#include "CommonDevice.h"
class CXLabel : public CXControl
{
protected:
	std::basic_string<TCHAR> m_Caption;
	DWORD m_Colour;
	/*LPD3DXFONT m_Font;*/
	ICommonFont* m_pFont;
	DWORD m_Format;
public:
	CXLabel( const LOGFONT& Font, iz3d::uilib::CommonDevice* Device );	
	virtual ~CXLabel();
	bool OnRender();
	void OnMouseDown(int Button, int X, int Y);
	void OnMouseMove(int X, int Y);
	void OnMouseUp(int Button, int X, int Y);
	void OnSetFocus();
    void OnLostFocus();
	void OnKeyDown(WPARAM Key, LPARAM Extended);
	void OnKeyUp(WPARAM Key, LPARAM Extended);
	void OnSysKey(WPARAM Key, LPARAM Extended);
	void SetCaption(const TCHAR* Caption) {m_Caption = Caption;}
	const TCHAR* GetCaption() const {return m_Caption.c_str();}
	void SetColour(DWORD Colour) {m_Colour = Colour;}
	DWORD GetColour() const {return m_Colour;}
	void SetFormat(DWORD Format) {m_Format = Format;}
	DWORD GetFormat() const {return m_Format;}	
};