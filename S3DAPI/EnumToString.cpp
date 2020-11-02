#include "stdafx.h"
#include "EnumToString.h"
#include "GlobalData.h"
#include "../ZLOg/Detail/Utils.h"

template<typename Char>
const Char* GetDeviceModeStringImpl(DeviceModes dm)
{
	switch(dm)
	{
		UNPACK_VALUE(DEVICE_MODE_FORCEFULLSCREEN);
		UNPACK_VALUE(DEVICE_MODE_FORCEWINDOWED);
		UNPACK_VALUE(DEVICE_MODE_RESERVED);
		UNPACK_VALUE(DEVICE_MODE_AUTO);
	};
	
	return AUTO_LITERAL("UNKNOWN");
}

template<typename Char>
const Char* GetMultiWindowsModeStringImpl(MultiWindowsModes dm)
{
	switch(dm)
	{
		UNPACK_VALUE(MULTI_WINDOWS_MODE_AUTO);
		UNPACK_VALUE(MULTI_WINDOWS_MODE_MULTIHEAD);
		UNPACK_VALUE(MULTI_WINDOWS_MODE_SWAPCHAINS);
		UNPACK_VALUE(MULTI_WINDOWS_MODE_APPLICATION);
		UNPACK_VALUE(MULTI_WINDOWS_MODE_SIDEBYSIDEWINDOWED);
	};

	return AUTO_LITERAL("UNKNOWN");
}

template<typename Char>
const Char* GetSecondWindowMessageStringImpl(WPARAM wParam)
{
	switch (wParam)
	{
		UNPACK_VALUE(SWM_MOVE_SECOND_WINDOW);
		UNPACK_VALUE(SWM_SHOW_SECOND_WINDOW);
		UNPACK_VALUE(SWM_HIDE_SECOND_WINDOW);
		UNPACK_VALUE(SWM_MOVE_APP_WINDOW);
		UNPACK_VALUE(SWM_RESTORE_APP_WINDOW);
	};
	return AUTO_LITERAL("UNKNOWN");
}

template<typename Char>
const Char* GetResultStringImpl(HRESULT hr) 
{
	switch(hr)
	{
		UNPACK_VALUE(S_OK)
		UNPACK_VALUE(E_INVALIDARG)
		UNPACK_VALUE(E_OUTOFMEMORY)
		UNPACK_VALUE(E_FAIL)
	}
	return AUTO_LITERAL("UNKNOWN");
}

template<typename Char> 
const Char* GetDeviceModeStringImpl(const DEVMODE* pMode)
{
	if (!pMode) {
		return AUTO_LITERAL("NULL");
	}

	return ZLOG_DBG_SPRINTF(AUTO_LITERAL("dmBitsPerPel = %d, dmPelsWidth = %d, dmPelsHeight = %d, dmDisplayFrequency = %d"), 
									 pMode->dmBitsPerPel, pMode->dmPelsWidth, pMode->dmPelsHeight, pMode->dmDisplayFrequency);
}

template<typename Char>
const Char* GetRectStringImpl(CONST RECT* pRect)
{
	if (pRect == NULL) {
		return AUTO_LITERAL("NULL");
	}

	return ZLOG_DBG_SPRINTF(AUTO_LITERAL("(%ld,%ld)-(%ld,%ld)"), pRect->left, pRect->top, pRect->right, pRect->bottom);
}

template<typename Char>
const Char* GetSizeStringImpl(CONST SIZE* pSize)
{
	if (pSize == NULL) {
		return AUTO_LITERAL("NULL");
	}

	return ZLOG_DBG_SPRINTF(AUTO_LITERAL("(%ld,%ld)"), pSize->cx, pSize->cy);
}

template<typename Char>
const Char* GetWindowRectStringImpl(HWND hWnd)
{
	if (hWnd == 0) {
		return AUTO_LITERAL("0");
	}

	RECT rcWindow;
	SetRect(&rcWindow, 0, 0, 0, 0);
	if (GetWindowRect(hWnd, &rcWindow))
	{
		return ZLOG_DBG_SPRINTF( AUTO_LITERAL("%p [%d, %d], [%d, %d]"), 
										  hWnd, 
										  rcWindow.left, 
										  rcWindow.top, 
										  rcWindow.right - rcWindow.left, 
										  rcWindow.bottom - rcWindow.top );
	}
	else {
		return ZLOG_DBG_SPRINTF( AUTO_LITERAL("%p, GetWindowRect() failed"), hWnd);
	}
}

template<typename Char>
const Char* GetMouseWindowMessageStringImpl(WPARAM wParam)
{
	switch (wParam)
	{
		UNPACK_VALUE(WM_CAPTURECHANGED);
		UNPACK_VALUE(WM_LBUTTONDBLCLK);
		UNPACK_VALUE(WM_LBUTTONDOWN);
		UNPACK_VALUE(WM_LBUTTONUP);
		UNPACK_VALUE(WM_MBUTTONDBLCLK);
		UNPACK_VALUE(WM_MBUTTONDOWN);
		UNPACK_VALUE(WM_MBUTTONUP);
		UNPACK_VALUE(WM_MOUSEACTIVATE);
		UNPACK_VALUE(WM_MOUSEHOVER);
		UNPACK_VALUE(WM_MOUSEHWHEEL);
		UNPACK_VALUE(WM_MOUSELEAVE);
		UNPACK_VALUE(WM_MOUSEMOVE);
		UNPACK_VALUE(WM_MOUSEWHEEL);
		UNPACK_VALUE(WM_NCHITTEST);
		UNPACK_VALUE(WM_NCLBUTTONDBLCLK);
		UNPACK_VALUE(WM_NCLBUTTONDOWN);
		UNPACK_VALUE(WM_NCLBUTTONUP);
		UNPACK_VALUE(WM_NCMBUTTONDBLCLK);
		UNPACK_VALUE(WM_NCMBUTTONDOWN);
		UNPACK_VALUE(WM_NCMBUTTONUP);
		UNPACK_VALUE(WM_NCMOUSEHOVER);
		UNPACK_VALUE(WM_NCMOUSELEAVE);
		UNPACK_VALUE(WM_NCMOUSEMOVE);
		UNPACK_VALUE(WM_NCRBUTTONDBLCLK);
		UNPACK_VALUE(WM_NCRBUTTONDOWN);
		UNPACK_VALUE(WM_NCRBUTTONUP);
		UNPACK_VALUE(WM_NCXBUTTONDBLCLK);
		UNPACK_VALUE(WM_NCXBUTTONDOWN);
		UNPACK_VALUE(WM_NCXBUTTONUP);
		UNPACK_VALUE(WM_RBUTTONDBLCLK);
		UNPACK_VALUE(WM_RBUTTONDOWN);
		UNPACK_VALUE(WM_RBUTTONUP);
		UNPACK_VALUE(WM_XBUTTONDBLCLK);
		UNPACK_VALUE(WM_XBUTTONDOWN);
		UNPACK_VALUE(WM_XBUTTONUP);
	};

	return AUTO_LITERAL("UNKNOWN");
}

template<typename Char>
const Char* GetMonitorInfoFlagsStringImpl(DWORD Flags)
{
	if (Flags == MONITORINFOF_PRIMARY) return AUTO_LITERAL("MONITORINFOF_PRIMARY");
	else return AUTO_LITERAL("0");
}

template<typename Char>
const Char* GetMonitorInfoStringImpl(LPMONITORINFO lpmi)
{
	if (!lpmi) {
		return AUTO_LITERAL("NULL");
	}

	if (lpmi->cbSize == sizeof(MONITORINFOEX))
	{
		LPMONITORINFOEX lpmiex = (LPMONITORINFOEX)lpmi;
		return ZLOG_DBG_SPRINTF( AUTO_LITERAL("rcMonitor = %s, rcWork = %s, dwFlags = %s, szDevice = %s"),
										  GetRectStringImpl<Char>(&lpmiex->rcMonitor), 
										  GetRectStringImpl<Char>(&lpmiex->rcWork), 
										  GetMonitorInfoFlagsStringImpl<Char>(lpmiex->dwFlags),
										  lpmiex->szDevice );
	}
	else 
	{
		return ZLOG_DBG_SPRINTF( AUTO_LITERAL("rcMonitor = %s, rcWork = %s, dwFlags = %s"),
										  GetRectStringImpl<Char>(&lpmi->rcMonitor), 
										  GetRectStringImpl<Char>(&lpmi->rcWork), 
										  GetMonitorInfoFlagsStringImpl<Char>(lpmi->dwFlags) );
	}
}

template const char* GetDeviceModeStringImpl<char>(DeviceModes dm);
template const char* GetMultiWindowsModeStringImpl<char>(MultiWindowsModes dm);
template const char* GetSecondWindowMessageStringImpl<char>(WPARAM wParam);

template const char* GetResultStringImpl<char>(HRESULT hr);
template const char* GetDeviceModeStringImpl<char>(const DEVMODE* pMode);
template const char* GetRectStringImpl<char>(CONST RECT* pRect);
template const char* GetSizeStringImpl<char>(CONST SIZE* pSize);
template const char* GetWindowRectStringImpl<char>(HWND hWnd);
template const char* GetMouseWindowMessageStringImpl<char>(WPARAM wParam);
template const char* GetMonitorInfoFlagsStringImpl<char>(DWORD Flags);
template const char* GetMonitorInfoStringImpl<char>(LPMONITORINFO lpmi);

template const wchar_t* GetDeviceModeStringImpl<wchar_t>(DeviceModes dm);
template const wchar_t* GetMultiWindowsModeStringImpl<wchar_t>(MultiWindowsModes dm);
template const wchar_t* GetSecondWindowMessageStringImpl<wchar_t>(WPARAM wParam);

template const wchar_t* GetResultStringImpl<wchar_t>(HRESULT hr);
template const wchar_t* GetDeviceModeStringImpl<wchar_t>(const DEVMODE* pMode);
template const wchar_t* GetRectStringImpl<wchar_t>(CONST RECT* pRect);
template const wchar_t* GetSizeStringImpl<wchar_t>(CONST SIZE* pSize);
template const wchar_t* GetWindowRectStringImpl<wchar_t>(HWND hWnd);
template const wchar_t* GetMouseWindowMessageStringImpl<wchar_t>(WPARAM wParam);
template const wchar_t* GetMonitorInfoFlagsStringImpl<wchar_t>(DWORD Flags);
template const wchar_t* GetMonitorInfoStringImpl<wchar_t>(LPMONITORINFO lpmi);
