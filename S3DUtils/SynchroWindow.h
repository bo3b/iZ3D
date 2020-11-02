/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <Multimon.h>
#include <atlbase.h>

class SynchroWindow
{
public:
	SynchroWindow(void);
	~SynchroWindow(void);

	void initialize(HMODULE hModule, HMONITOR hBack, HMONITOR hFront, HBITMAP backScreenBitmap, HBITMAP frontScreenBitmap, BOOL exitOnESC = TRUE);

	void SetMonitorData( BITMAP &info, RECT &mr, RECT &outImageRect );
	bool show();
	void clear();

protected:	
	void onPaint(HWND hWnd);
	
	HBITMAP				m_BackScreenBitmap;
	HBITMAP				m_FrontScreenBitmap;
	RECT				m_BackImageRect,  m_BackScreenRect;
	RECT				m_FrontImageRect, m_FrontScreenRect;
	HWND				m_FrontScreenWnd;
	HWND				m_BackScreenWnd;
	WNDCLASS			m_WndClass;
	BOOL				m_ExitOnESC;

private:
	HMODULE				m_ThisModule;

	friend LRESULT CALLBACK SynchroWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
};

HRESULT LoadBitmapFromResourcePack( LPCTSTR szResName, HBITMAP* hBitmap, IPicture** picture);
HRESULT LoadBitmapFromResource(LPCTSTR lpName, LPCTSTR lpType, HBITMAP* hBitmap, IPicture** picture);

std::wstring GetCurrentLanguage();
