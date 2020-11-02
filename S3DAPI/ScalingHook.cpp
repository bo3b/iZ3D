#include "stdafx.h"
#include "EnumToString.h"
#include "ScalingHook.h"
#include "HookAPI.h"
#include "HookDInput.h"
#include "ScalingAgent.h"
#include "Utils.h"
#include "Logging.h"
#include "../ZLOg/Detail/Utils.h"
#include <boost/bind.hpp>
#include <d3d9.h>

#undef ZLOG_MODULE
#define ZLOG_MODULE _T("ScalingHook")

namespace {

	const DWORD HookingFlags  = NO_SAFE_UNHOOKING | DONT_RENEW_OVERWRITTEN_HOOK | RESTORE_CODE_BEFORE_HOOK;

	// Hooked functions
	int  (WINAPI *Original_GetSystemMetrics)(int nIdex);
	BOOL (WINAPI *Original_EnumDisplaySettingsA)(LPCTSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode);
	BOOL (WINAPI *Original_EnumDisplaySettingsW)(LPCTSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode);
	BOOL (WINAPI *Original_GetClientRect)(HWND hWnd, LPRECT lpRect);
	BOOL (WINAPI *Original_GetWindowRect)(HWND hWnd, LPRECT lpRect);
	// some hooked functions
	BOOL (WINAPI *Original_GetCursorInfo)( PCURSORINFO pci );

#ifndef FINAL_RELEASE
	BOOL (WINAPI *Original_GetMonitorInfo)(HMONITOR hMonitor, LPMONITORINFO lpmi);
	LONG (WINAPI *Original_ChangeDisplaySettings)(DEVMODE *lpDevMode, DWORD dwflags);
	LONG (WINAPI *Original_ChangeDisplaySettingsEx)(LPCTSTR lpszDeviceName, DEVMODE *lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam);
#endif
	
	BOOL (WINAPI *Original_GetCursorPos)(LPPOINT lpPoint);
	BOOL (WINAPI *Original_SetCursorPos)(int X, int Y);
	BOOL (WINAPI *Original_GetClipCursor)(RECT *lpRect);
	BOOL (WINAPI *Original_ClipCursor)(const RECT* lpRect);

	CHAR currentDeviceNameA[MAX_PATH];
	CHAR currentDeviceNameW[MAX_PATH];

	template<typename T, typename Y>
	T round(Y x) 
	{ 
		return T(x + Y(0.5)); 
	}

	bool operator == (const RECT& a, const RECT& b)
	{
		return (a.bottom == b.bottom) && (a.top == b.top) && (a.right == b.right) && (a.left == b.left);
	}
		
	template<typename T>
	T TranslateX(T x, const RECT& src, const RECT& dst)
	{
		return round<T>(float(x - dst.left) * (src.right - src.left) / (dst.right - dst.left) + src.left);
	}
		
	template<typename T>
	T TranslateY(T y, const RECT& src, const RECT& dst)
	{
		return round<T>(float(y - dst.top) * (src.bottom - src.top) / (dst.bottom - dst.top) + src.top);
	}

	template<typename T>
	void Translate(T& x, T& y, const RECT& src, const RECT& dst)
	{
		x = TranslateX(x, src, dst);
		y = TranslateY(y, src, dst);
	}

	int WINAPI Hooked_GetSystemMetrics(int nIndex)
	{
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && nIndex == SM_CXSCREEN) 
		{
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetSystemMetrics(%d) = %d\n"), nIndex, hook->originalSize.cx);
			return hook->originalSize.cx;
		}
		else if (hook && nIndex == SM_CYSCREEN) 
		{
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetSystemMetrics(%d) = %d\n"), nIndex, hook->originalSize.cy);
			return hook->originalSize.cy;
		}

		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetSystemMetrics(%d) = %d\n"), nIndex, Original_GetSystemMetrics(nIndex));
		return Original_GetSystemMetrics(nIndex);
	}

	BOOL WINAPI Hooked_EnumDisplaySettingsA(LPCTSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode)
	{
		BOOL res = Original_EnumDisplaySettingsA(lpszDeviceName, iModeNum, lpDevMode);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("EnumDisplaySettingsA(lpszDeviceName = %S, iModeNum = %d, lpDevMode = {%s}) = %d\n"), lpszDeviceName, iModeNum, GetDeviceModeString(lpDevMode), res);
		
		ScalingAgentPtrT agent = ScalingAgent::Instance();
		if (lpDevMode 
			// && (lpszDeviceName == NULL || StrCmpA(lpszDeviceName, currentDeviceNameA) Better to check this
			&& iModeNum == ENUM_CURRENT_SETTINGS)
		{
			if (ScalingHookPtrT hook = agent->Hook())
			{
				lpDevMode->dmPelsWidth = hook->originalSize.cx;
				lpDevMode->dmPelsHeight = hook->originalSize.cy;
			}
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpDevMode = %s\n"), GetDeviceModeString(lpDevMode));
		}

		return res;
	}
	
	BOOL WINAPI Hooked_EnumDisplaySettingsW(LPCTSTR lpszDeviceName, DWORD iModeNum, LPDEVMODE lpDevMode)
	{
		BOOL res = Original_EnumDisplaySettingsW(lpszDeviceName, iModeNum, lpDevMode);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("EnumDisplaySettingsW(lpszDeviceName = %s, iModeNum = %u, lpDevMode = {%s}) = %d\n"), lpszDeviceName, iModeNum, GetDeviceModeString(lpDevMode), res);

		ScalingAgentPtrT agent = ScalingAgent::Instance();
		if (lpDevMode 
			// && (lpszDeviceName == NULL || StrCmpW(lpszDeviceName, currentDeviceNameW) Better to check this 
			&& iModeNum == ENUM_CURRENT_SETTINGS)
		{
			if (ScalingHookPtrT hook = agent->Hook())
			{
				lpDevMode->dmPelsWidth = hook->originalSize.cx;
				lpDevMode->dmPelsHeight = hook->originalSize.cy;
			}
			ZLOG_MESSAGE(zlog::SV_FLOOD,_T("\tModified lpDevMode = %s\n"), GetDeviceModeString(lpDevMode));
		}

		return res;
	}

	BOOL WINAPI Hooked_GetClientRect(HWND hWnd, LPRECT lpRect)
	{
		BOOL res = Original_GetClientRect(hWnd, lpRect);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetClientRect(hWnd = %p, lpRect = %s) = %d\n"), hWnd, GetRectString(lpRect), res);
		
		ScalingAgentPtrT agent = ScalingAgent::Instance();
		if (lpRect && agent->AffectHWND(hWnd))
		{
			if (ScalingHookPtrT hook = agent->Hook())			
			{
				lpRect->bottom = lpRect->top + hook->originalSize.cy;
				lpRect->right = lpRect->left + hook->originalSize.cx;
			}
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpRect = %s\n"), GetRectString(lpRect));
		}

		return res;
	}

	BOOL WINAPI Hooked_GetWindowRect(HWND hWnd, LPRECT lpRect)
	{
		BOOL res = Original_GetWindowRect(hWnd, lpRect);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetWindowRect(hWnd = %p, lpRect = %s) = %d\n"), hWnd, GetRectString(lpRect), res);

		ScalingAgentPtrT agent = ScalingAgent::Instance();
		if (lpRect && agent->AffectHWND(hWnd))
		{
			RECT clientRect;
			Original_GetClientRect(hWnd, &clientRect);

			POINT diff;
			diff.x = lpRect->right - clientRect.right;
			diff.y = lpRect->bottom - clientRect.bottom;
			
			if (ScalingHookPtrT hook = agent->Hook())
			{
				lpRect->right = clientRect.left + hook->originalSize.cx + diff.x;
				lpRect->bottom = clientRect.top + hook->originalSize.cy + diff.y;
			}
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpRect = %s\n"), GetRectString(lpRect));
		}

		return res;
	}

	// These functions also can be source of bugs with scaling so log their calls
#ifndef FINAL_RELEASE
	LONG WINAPI Hooked_ChangeDisplaySettings(DEVMODE *lpDevMode, DWORD dwflags)
	{
		/*
		if ( ScalingHook::IsModeSwithIgnored() ) 
		{
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("ChangeDisplaySettings(lpDevMode = %s, dwflags = %h) ignored\n"), GetDeviceModeString(lpDevMode), dwflags);
			return DISP_CHANGE_SUCCESSFUL;
		}
		*/
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("ChangeDisplaySettings(lpDevMode = %s, dwflags = %h)\n"), GetDeviceModeString(lpDevMode), dwflags);
		return Original_ChangeDisplaySettings(lpDevMode, dwflags);
	}

	LONG WINAPI Hooked_ChangeDisplaySettingsEx(LPCTSTR lpszDeviceName, DEVMODE *lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam)
	{
		/*
		if ( ScalingHook::IsModeSwithIgnored() ) 
		{
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("ChangeDisplaySettingsEx(lpszDeviceName = %s, lpDevMode = %s, hwnd = %h, dwFlags = %h, lParam = %p) ignored\n"),
										 lpszDeviceName, GetDeviceModeString(lpDevMode), hwnd, dwflags, lParam);
			return DISP_CHANGE_SUCCESSFUL;
		}
		*/
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("ChangeDisplaySettingsEx(lpszDeviceName = %s, lpDevMode = %s, hwnd = %h, dwFlags = %h, lParam = %p)\n"),
									 lpszDeviceName, GetDeviceModeString(lpDevMode), hwnd, dwflags, lParam);
		return Original_ChangeDisplaySettingsEx(lpszDeviceName, lpDevMode, hwnd, dwflags, lParam);
	}

	BOOL WINAPI Hooked_GetMonitorInfo(HMONITOR hMonitor, LPMONITORINFO lpmi)
	{
		BOOL result = Original_GetMonitorInfo(hMonitor, lpmi);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetMonitorInfo(hMonitor = %h, lpmi = %s) = %i\n"), hMonitor, GetMonitorInfoString(lpmi), result);
		/*
		if ( result && ScalingHook::IsMonitorAffected(hMonitor) ) 
		{
			ScalingHookPtrT hook = ScalingHook::Instance();		
			
			// resize working monitor area, hope this heuristic works
			POINT diff;
			diff.x = lpmi->rcMonitor.right  - lpmi->rcWork.right;
			diff.y = lpmi->rcMonitor.bottom - lpmi->rcWork.bottom;
			lpmi->rcWork.right	= lpmi->rcWork.left + hook->originalSize.cx + diff.x;
			lpmi->rcWork.bottom = lpmi->rcWork.bottom + hook->originalSize.cy + diff.y;
			 
			// resize client monitor area
			lpmi->rcMonitor.bottom = lpmi->rcMonitor.top  + hook->originalSize.cy;
			lpmi->rcMonitor.right  = lpmi->rcMonitor.left + hook->originalSize.cx;

			DEBUG_TRACE3(_T("\tModified lpmi = %s\n"), GetMonitorInfoString(lpmi));
		}
		*/
		return result;
	}
#endif // FINAL_RELEASE

	/** Translate cursor position into application coordinate system */
	BOOL WINAPI Hooked_GetCursorPos(LPPOINT lpPoint)
	{
		BOOL result = Original_GetCursorPos(lpPoint);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetCursorPos(lpPoint = (%d, %d)) = %i"), lpPoint->x, lpPoint->y, result);

		if (result) 
		{
			ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
			if (hook) Translate(lpPoint->x, lpPoint->y, hook->srcRect, hook->dstRect);
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpPoint = (%d, %d)"), lpPoint->x, lpPoint->y);
		}
		return result;
	}

	BOOL WINAPI Hooked_GetCursorInfo( PCURSORINFO pci )
	{
		ZLOG_MESSAGE(zlog::SV_NOTICE, _T("Hooked_GetCursorInfo: %x, Original_GetCursorInfo: %x"), Hooked_GetCursorInfo, Original_GetCursorInfo );
		BOOL result = Original_GetCursorInfo(pci);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetCursorInfo(pci = (%d, %d)) = %i"), pci->ptScreenPos.x, pci->ptScreenPos.y, result);

		if (result) 
		{
			ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
			if (hook) Translate(pci->ptScreenPos.x, pci->ptScreenPos.y, hook->srcRect, hook->dstRect);
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified pci = (%d, %d)"), pci->ptScreenPos.x, pci->ptScreenPos.y);
		}
		return result;
	}
	/** Translate cursor position into scaled coordinate system */
	BOOL WINAPI Hooked_SetCursorPos(int X, int Y)
	{
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("SetCursorPos(X = %d, Y = %d)\n"), X, Y);
		if (hook) Translate(X, Y, hook->dstRect, hook->srcRect);
		BOOL result = Original_SetCursorPos(X, Y);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified X = %d, Y = %d"), X, Y);
		return result;
	}

	/** Translate cursor clip area into application coordinate system */
	BOOL WINAPI Hooked_GetClipCursor(RECT *lpRect)
	{
		BOOL result = Original_GetClipCursor(lpRect);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetClipCursor(lpRect = %p) = %i\n"), GetRectString(lpRect), result);
		if (result)
		{
			if (ScalingHookPtrT hook = ScalingAgent::Instance()->Hook())
			{
				Translate(lpRect->left, lpRect->top, hook->srcRect, hook->dstRect);
				Translate(lpRect->right, lpRect->bottom, hook->srcRect, hook->dstRect);
			}
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpRect = %s\n"), GetRectString(lpRect));
		}

		return result;
	}

	/** Translate cursor clip area into scaled coordinate system */
	BOOL WINAPI Hooked_ClipCursor(const RECT *lpRect)
	{
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("ClipCursor(lpRect = %s)"), GetRectString(lpRect));
		BOOL result = FALSE;
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && lpRect)
		{
			RECT tmp = *lpRect;
			hook->lastClipCursor = *lpRect;
			Translate(tmp.left, tmp.top, hook->dstRect, hook->srcRect);
			Translate(tmp.right, tmp.bottom, hook->dstRect, hook->srcRect);
			result = Original_ClipCursor(&tmp);
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tModified lpRect = %s\n"), GetRectString(&tmp));
		}
		else if (hook) 
		{
			hook->lastClipCursor = hook->nativeRect;
			result = Original_ClipCursor(lpRect);
		}

		return result;
	}

	/** Transform mouse messages to keep translated mouse coordinates. */
	LRESULT CALLBACK Hooked_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		assert(hook);

		switch (uMsg)
		{
			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEHOVER:
			case WM_MOUSEHWHEEL:
			case WM_MOUSEMOVE:
			case WM_MOUSEWHEEL:
			case WM_NCHITTEST:
			case WM_NCLBUTTONDBLCLK:
			case WM_NCLBUTTONDOWN:
			case WM_NCLBUTTONUP:
			case WM_NCMBUTTONDBLCLK:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONUP:
			case WM_NCMOUSEHOVER:
			case WM_NCMOUSEMOVE:
			case WM_NCRBUTTONDBLCLK:
			case WM_NCRBUTTONDOWN:
			case WM_NCRBUTTONUP:
			case WM_NCXBUTTONDBLCLK:
			case WM_NCXBUTTONDOWN:
			case WM_NCXBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			{			
				SHORT xPos = LOWORD(lParam);
				SHORT yPos = HIWORD(lParam);
				ZLOG_MESSAGE(zlog::SV_FLOOD, _T("WindowProc(%p): %s(%d, %d)\n"), hwnd, GetMouseWindowMessageString(uMsg), xPos, yPos);
				Translate(xPos, yPos, hook->srcRect, hook->dstRect);
				lParam = MAKELPARAM(xPos, yPos);
				ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tTransformed WindowProc: %s(%d, %d)\n"), GetMouseWindowMessageString(uMsg), xPos, yPos);
				break;
			}

			case WM_INPUT:
			{
				ZLOG_MESSAGE(zlog::SV_FLOOD, _T("WindowProc(%p): WM_INPUT(%p)\n"), hwnd, lParam);
				break;
			}

			case WM_SIZE:
			{
				ZLOG_MESSAGE(zlog::SV_FLOOD, _T("WindowProc(%p): WM_SIZE(%d, %d)\n"), hwnd, LOWORD(lParam), HIWORD(lParam) );
				lParam = MAKELPARAM(hook->originalSize.cx, hook->originalSize.cy);
				ZLOG_MESSAGE(zlog::SV_FLOOD, _T("\tTransformed WindowProc: WM_SIZE(%d, %d)"), LOWORD(lParam), HIWORD(lParam) );
				break;
			}

		default:
			ZLOG_MESSAGE(zlog::SV_FLOOD, _T("WindowProc(%p): %d\n"), hwnd, uMsg);
			break;
		}

		WNDPROC wndProc = hook->GetWndProc(hwnd);
#ifndef FINAL_RELEASE
		if (wndProc == Hooked_WindowProc) __debugbreak();
#endif
		return wndProc(hwnd, uMsg, wParam, lParam);
	}
	
	/** Transform mouse history to keep translated mouse coordinates. */
	int WINAPI Hooked_GetMouseMovePointsEx(UINT cbSize, LPMOUSEMOVEPOINT lppt, LPMOUSEMOVEPOINT lpptBuf, int nBufPoints, DWORD resolution)
	{
		STATIC_FUNCTION_HOOK(GetMouseMovePointsEx, Hooked_GetMouseMovePointsEx);
		
		int result = CALL_ORIGINAL(cbSize, lppt, lpptBuf, nBufPoints, resolution);
		ZLOG_MESSAGE(zlog::SV_FLOOD, 
					 _T("GetMouseMovePointsEx(cbSize = %d, lppt = %p, lpptBuf = %p, nBufPoints = %d, resolution = %d) %d\n"), 
					 cbSize, lppt, lpptBuf, nBufPoints, resolution, result);
		return result;
	}


	template<typename IDevice>
	STDMETHODIMP Hooked_GetDeviceState(IDevice* this_, DWORD cbData, LPVOID lpvData)
	{
		UNIQUE_MEMBER_FUNCTION_HOOK(this_, &IDevice::GetDeviceState, Hooked_GetDeviceState<IDevice>);
		
		HRESULT	hResult = CALL_ORIGINAL_MEMBER(this_, cbData, lpvData);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("IDirectInputDevice8::GetDeviceState(this = %p, cbData = %d, lpvData = %p) %s\n"), this_, cbData, lpvData, GetResultString(hResult) );

		if ( INTERFACE_HOOKED && SUCCEEDED(hResult) )
		{
			ScalingAgentPtrT agent	= ScalingAgent::Instance();
			ScalingHookPtrT	 hook   = agent->Hook();

			BYTE* data = (BYTE*)lpvData;
			ScalingAgent::device_info& info = agent->GetDeviceInfo(this_);
			/*
			for (size_t i = 0; i<info.xOffsets.size(); ++i) {
				*(LONG*)(data + info.xOffsets[i]) = hook->cursorPos.x - info.prevPos.x + 1;
			}
						
			for (size_t i = 0; i<info.yOffsets.size(); ++i) {
				*(LONG*)(data + info.yOffsets[i]) = hook->cursorPos.y - info.prevPos.y;
			}

			info.prevPos = hook->cursorPos;
			*/
			if ( !info.xOffsets.empty() && !info.yOffsets.empty() ) 
			{
				hook->cursorPos.x += *(LONG*)(data + info.xOffsets[0]);
				hook->cursorPos.y += *(LONG*)(data + info.yOffsets[0]);
			}
			//std::ostringstream ss;
			//ss << "x = " << *(LONG*)(data + fmt.xOffsets[0])
			//   << "; y = " << *(LONG*)(data + fmt.yOffsets[0]);
			//MessageBoxA(0, ss.str().c_str(), "GetDeviceState", MB_OK);
		}

		return hResult;
	}

	template<typename IDevice>
	STDMETHODIMP Hooked_GetDeviceData(IDevice* this_, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
	{
		UNIQUE_MEMBER_FUNCTION_HOOK(this_, &IDevice::GetDeviceData, Hooked_GetDeviceData<IDevice>);

		HRESULT	hResult = CALL_ORIGINAL_MEMBER(this_, cbObjectData, rgdod, pdwInOut, dwFlags);
		ZLOG_MESSAGE( zlog::SV_FLOOD, 
					  _T("IDirectInputDevice8::GetDeviceData(this = %p, cbObjectData = %d, rgdod = %p, pdwInOut = %d, dwFlags = %d) %s\n"), 
					  this_, cbObjectData, rgdod, dwFlags, GetResultString(hResult) );
		
		if ( INTERFACE_HOOKED && SUCCEEDED(hResult) )
		{
			ScalingAgentPtrT agent = ScalingAgent::Instance();
			ScalingHookPtrT	 hook  = agent->Hook();

			const ScalingAgent::device_info& info = agent->GetDeviceInfo(this_);
			for (DWORD i = 0; i<*pdwInOut; ++i, ++rgdod)
			{
				BYTE* data = (BYTE*)rgdod;
				for (size_t j = 0; j<info.xOffsets.size(); ++j) 
				{
					LONG* x = (LONG*)(data + info.xOffsets[j]);
					LONG oldx = *x;
					TranslateX(*x, hook->srcRect, hook->dstRect);
				}
						
				for (size_t j = 0; j<info.yOffsets.size(); ++j) {
					TranslateY(*(LONG*)(data + info.yOffsets[j]), hook->srcRect, hook->dstRect);
				}
			}
		}

		return hResult;
	}
	
	UINT WINAPI Hooked_GetRawInputBuffer(PRAWINPUT pData, PUINT pcbSize, UINT cbSizeHeader)
	{
		STATIC_FUNCTION_HOOK(GetRawInputBuffer, Hooked_GetRawInputBuffer);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetRawInputBuffer"));
		return CALL_ORIGINAL(pData, pcbSize, cbSizeHeader);
	}

	UINT WINAPI Hooked_GetRawInputData(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader)
	{
		STATIC_FUNCTION_HOOK(GetRawInputData, Hooked_GetRawInputData);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetRawInputData"));
		return CALL_ORIGINAL(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
	}

	/*
	STDMETHODIMP Hooked_GetAdapterDisplayModeEx(IDirect3D9* this_, UINT Adapter, D3DDISPLAYMODEEX* pMode)
	{
		HRESULT	hResult = CALL_ORIGINAL_MEMBER(this_, Adapter, pMode);
		ZLOG_MESSAGE( zlog::SV_FLOOD, 
					  _T("IDirect3D9::GetAdapterDisplayMode(this = %p, Adapter = %d, pMode = %s) %s\n"), 
					  this_, Adapter, GetDisplayModeString(pMode), GetResultString(hResult) );
		
	}
	*/

	int WINAPI Hooked_GetWindowRgn(HWND hWnd, HRGN hRgn)
	{
		STATIC_FUNCTION_HOOK(GetWindowRgn, Hooked_GetWindowRgn);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetWindowRgn(hWnd =, hRgn = )"));
		return CALL_ORIGINAL(hWnd, hRgn);
	}

	int WINAPI Hooked_SetWindowRgn(HWND hWnd, HRGN hRgn, BOOL redraw)
	{
		STATIC_FUNCTION_HOOK(SetWindowRgn, Hooked_SetWindowRgn);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("SetWindowRgn(hWnd =, hRgn = , redraw =)"));
		return CALL_ORIGINAL(hWnd, hRgn, redraw);
	}

	BOOL WINAPI Hooked_RectVisible(HDC hdc, CONST RECT* lprc)
	{
		STATIC_FUNCTION_HOOK(RectVisible, Hooked_RectVisible);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("RectVisible(hdc =, lprc = )"));
		return CALL_ORIGINAL(hdc, lprc);
	}

	void WINAPI Hooked_GetViewportExtEx(HDC hdc, LPSIZE lpSize)
	{
		STATIC_FUNCTION_HOOK(GetViewportExtEx, Hooked_GetViewportExtEx);
		CALL_ORIGINAL(hdc, lpSize);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetViewportExtEx(hdc = %p, lpSize = %s)"), hdc, GetSizeString(lpSize));
	}

	void WINAPI Hooked_GetWindowExtEx(HDC hdc, LPSIZE lpSize)
	{
		STATIC_FUNCTION_HOOK(GetWindowExtEx, Hooked_GetWindowExtEx);
		CALL_ORIGINAL(hdc, lpSize);
		ZLOG_MESSAGE(zlog::SV_FLOOD, _T("GetWindowExtEx(hdc = %p, lpSize = %s)"), hdc, GetSizeString(lpSize));
	}
	
	LONG WINAPI Hooked_GetWindowLongA(HWND hWnd, int nIndex)
	{
		STATIC_FUNCTION_HOOK(GetWindowLongA, Hooked_GetWindowLongA);
		
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && nIndex == GWLP_WNDPROC) 
		{
			if (WNDPROC wndProc = hook->GetWndProc(hWnd)) {
				return (LONG)wndProc;
			}
		}

		return CALL_ORIGINAL(hWnd, nIndex);
	}

	LONG WINAPI Hooked_GetWindowLongW(HWND hWnd, int nIndex)
	{
		STATIC_FUNCTION_HOOK(GetWindowLongW, Hooked_GetWindowLongW);
		
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && nIndex == GWLP_WNDPROC) 
		{
			if (WNDPROC wndProc = hook->GetWndProc(hWnd)) {
				return (LONG)wndProc;
			}
		}

		return CALL_ORIGINAL(hWnd, nIndex);
	}

	LONG WINAPI Hooked_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
	{
		STATIC_FUNCTION_HOOK(SetWindowLongA, Hooked_SetWindowLongA);
		
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && nIndex == GWLP_WNDPROC) 
		{
			if (WNDPROC wndProc = hook->ChangeWndProc(hWnd, reinterpret_cast<WNDPROC>(dwNewLong))) {
				return (LONG)wndProc;
			}
		}

		return CALL_ORIGINAL(hWnd, nIndex, dwNewLong);
	}
	
	LONG WINAPI Hooked_SetWindowLongW(HWND hWnd, int nIndex, LONG dwNewLong)
	{
		STATIC_FUNCTION_HOOK(SetWindowLongW, Hooked_SetWindowLongW);
		
		ScalingHookPtrT hook = ScalingAgent::Instance()->Hook();
		if (hook && nIndex == GWLP_WNDPROC) 
		{
			if (WNDPROC wndProc = hook->ChangeWndProc(hWnd, reinterpret_cast<WNDPROC>(dwNewLong))) {
				return (LONG)wndProc;
			}
		}

		return CALL_ORIGINAL(hWnd, nIndex, dwNewLong);
	}

} // anonymous namespace 

ScalingHook::ScalingHook(ScalingAgentPtrT agent_, TDisplayScalingMode mode_, SIZE originalSize_, SIZE scaledSize_) :
	agent(agent_)
{
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook initializing\n"));
		
	HookCode(GetSystemMetrics, Hooked_GetSystemMetrics, (PVOID*)&Original_GetSystemMetrics, HookingFlags);
	//HookCode(EnumDisplaySettingsA, Hooked_EnumDisplaySettingsA, (PVOID*)&Original_EnumDisplaySettingsA, HookingFlags);
	//HookCode(EnumDisplaySettingsW, Hooked_EnumDisplaySettingsW, (PVOID*)&Original_EnumDisplaySettingsW, HookingFlags);
	HookCode(GetClientRect, Hooked_GetClientRect, (PVOID*)&Original_GetClientRect, HookingFlags);
	HookCode(GetWindowRect, Hooked_GetWindowRect, (PVOID*)&Original_GetWindowRect, HookingFlags);
#ifndef FINAL_RELEASE
	HookCode(GetMonitorInfo, Hooked_GetMonitorInfo, (PVOID*)&Original_GetMonitorInfo, HookingFlags);
	HookCode(ChangeDisplaySettings, Hooked_ChangeDisplaySettings, (PVOID*)&Original_ChangeDisplaySettings, HookingFlags);
	HookCode(ChangeDisplaySettingsEx, Hooked_ChangeDisplaySettingsEx, (PVOID*)&Original_ChangeDisplaySettingsEx, HookingFlags);
#endif
	HookCode(GetCursorPos, Hooked_GetCursorPos, (PVOID*)&Original_GetCursorPos, HookingFlags);
	HookCode(SetCursorPos, Hooked_SetCursorPos, (PVOID*)&Original_SetCursorPos, HookingFlags);
	HookCode(GetClipCursor, Hooked_GetClipCursor, (PVOID*)&Original_GetClipCursor, HookingFlags);
	HookCode(ClipCursor, Hooked_ClipCursor, (PVOID*)&Original_ClipCursor, HookingFlags);
	HookCode(GetCursorInfo, Hooked_GetCursorInfo, (PVOID*)&Original_GetCursorInfo, HookingFlags);
	//hooks.add_hook( MAKE_STATIC_HOOK(GetMouseMovePointsEx, Hooked_GetMouseMovePointsEx, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(GetRawInputBuffer, Hooked_GetRawInputBuffer, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(GetRawInputData, Hooked_GetRawInputData, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(GetWindowRgn, Hooked_GetWindowRgn, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(SetWindowRgn, Hooked_SetWindowRgn, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(RectVisible, Hooked_RectVisible, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(GetViewportExtEx, Hooked_GetViewportExtEx, HookingFlags) );
	//hooks.add_hook( MAKE_STATIC_HOOK(GetWindowExtEx, Hooked_GetWindowExtEx, HookingFlags) );
	hooks.add_hook( MAKE_STATIC_HOOK(GetWindowLongA, Hooked_GetWindowLongA, HookingFlags) );
	hooks.add_hook( MAKE_STATIC_HOOK(GetWindowLongW, Hooked_GetWindowLongW, HookingFlags) );
	hooks.add_hook( MAKE_STATIC_HOOK(SetWindowLongA, Hooked_SetWindowLongA, HookingFlags) );
	hooks.add_hook( MAKE_STATIC_HOOK(SetWindowLongW, Hooked_SetWindowLongW, HookingFlags) );

	// remember original clip cursor area
	if ( !Original_GetClipCursor(&lastClipCursor) ) 
	{
		// This function may not FAIL, I suppose
		ZLOG_MESSAGE(zlog::SV_ERROR, _T("ScalingHook GetClipCursor failed: error#%d\n"), GetLastError());
	}
	
	Setup(mode_, originalSize_, scaledSize_);
}

ScalingHook::~ScalingHook()
{
	while (wndProcs.size() > 0)
	{
		// this call modifies the collection!
		UnhookWndProc(wndProcs.begin()->first);
	}

	// restore application clip cursor area
	if ( !Original_ClipCursor(lastClipCursor == nativeRect ? NULL : &lastClipCursor) ) 
	{
		// This function may not FAIL, I suppose
		ZLOG_MESSAGE(zlog::SV_ERROR, _T("ScalingHook ClipCursor failed: error#%d\n"), GetLastError());
	}

	UnhookCode((PVOID*)&Original_GetSystemMetrics);   
	//UnhookCode((PVOID*)&Original_EnumDisplaySettingsA);  
	//UnhookCode((PVOID*)&Original_EnumDisplaySettingsW);  
	UnhookCode((PVOID*)&Original_GetClientRect);   
	UnhookCode((PVOID*)&Original_GetWindowRect);  
#ifndef FINAL_RELEASE
	UnhookCode((PVOID*)&Original_GetMonitorInfo);  
	UnhookCode((PVOID*)&Original_ChangeDisplaySettings);   
	UnhookCode((PVOID*)&Original_ChangeDisplaySettingsEx);  
#endif
	UnhookCode((PVOID*)&Original_GetCursorPos);
	UnhookCode((PVOID*)&Original_SetCursorPos);
	UnhookCode((PVOID*)&Original_GetClipCursor);
	UnhookCode((PVOID*)&Original_ClipCursor);
	UnhookCode((PVOID*)&Original_GetCursorInfo);

	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("Scaling hook removed\n"));
}

bool ScalingHook::HookWndProc(HWND hWnd)
{
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook::HookWndProc(HWND hWnd = %p)\n"), hWnd);
	
	if (wndProcs.count(hWnd) == 0)
	{
		if ( IsWindowUnicode(hWnd) ) 
		{
			STATIC_FUNCTION_HOOK(SetWindowLongW, Hooked_SetWindowLongW);
			WNDPROC wndProc = reinterpret_cast<WNDPROC>( CALL_ORIGINAL(hWnd, GWLP_WNDPROC, (LONG)Hooked_WindowProc) );
			wndProcs.insert( wndproc_map::value_type(hWnd, wndProc) );
		}
		else 
		{
			STATIC_FUNCTION_HOOK(SetWindowLongA, Hooked_SetWindowLongA);
			WNDPROC wndProc = reinterpret_cast<WNDPROC>( CALL_ORIGINAL(hWnd, GWLP_WNDPROC, (LONG)Hooked_WindowProc) );
			wndProcs.insert( wndproc_map::value_type(hWnd, wndProc) );
		}

		return true;
	}

	return false;
}

WNDPROC	ScalingHook::GetWndProc(HWND hWnd)
{
	auto wndProcIter = wndProcs.find(hWnd);
	if ( wndProcIter == wndProcs.end() ) {
		return 0;
	}

	return wndProcIter->second;
}

WNDPROC ScalingHook::ChangeWndProc(HWND hWnd, WNDPROC wndProc)
{
	auto wndProcIter = wndProcs.find(hWnd);
	if ( wndProcIter != wndProcs.end() ) 
	{
		WNDPROC orig = wndProcs[hWnd];
		wndProcs[hWnd] = wndProc;
		return orig;
	}

	return 0;
}

void ScalingHook::UnhookWndProc(HWND hWnd)
{
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook::UnhookWndProc(HWND hWnd = %p)\n"), hWnd);
	if ( IsWindowUnicode(hWnd) ) 
	{
		// call original!
		STATIC_FUNCTION_HOOK(SetWindowLongW, Hooked_SetWindowLongW);
		CALL_ORIGINAL(hWnd, GWLP_WNDPROC, (LONG)wndProcs[hWnd]);
	}
	else 
	{
		// call original!
		STATIC_FUNCTION_HOOK(SetWindowLongA, Hooked_SetWindowLongA);
		CALL_ORIGINAL(hWnd, GWLP_WNDPROC, (LONG)wndProcs[hWnd]);
	}
	wndProcs.erase(hWnd);
}

template<typename IDevice>
bool ScalingHook::HookDevice(IDevice* device)
{
	if (deviceHooks.count(device) == 0)
	{
		ScalingAgent::Instance()->GetDeviceInfo(device).prevPos = cursorPos;

		DInputDeviceHook hook;
		hook.getDeviceStateHook = MAKE_UNIQUE_MEMBER_HOOK( (IDevice*)device,
														   &IDevice::GetDeviceState, 
														   Hooked_GetDeviceState<IDevice>, 
														   HookingFlags );
		hook.getDeviceDataHook = MAKE_UNIQUE_MEMBER_HOOK( (IDevice*)device,
														  &IDevice::GetDeviceData, 
														  Hooked_GetDeviceData<IDevice>, 
														  HookingFlags );
		deviceHooks.insert( device_hook_map::value_type(device, hook) );
		ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook::HookDevice(IDirectInputDevice8* device = %p)\n"), device);
		return true;
	}

	return false;
}

template<typename IDevice>
void ScalingHook::UnhookDevice(IDevice* device)
{
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook::UnhookDevice(IDirectInputDevice8* device = %p)\n"), device);
	deviceHooks.erase(device);
}

void ScalingHook::Setup(TDisplayScalingMode mode, SIZE originalSize, SIZE scaledSize)
{
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook::Setup(mode = %d, originalSize = (%d, %d), scaledSize = (%d, %d)\n"), 
		mode, 
		originalSize.cx, 
		originalSize.cy,
		scaledSize.cx,
		scaledSize.cy);

	this->originalSize = originalSize;

	// calculate source and destination rect to translate cursor position
	RECT src;
	{
		src.left   = 0;
		src.right  = originalSize.cx;
		src.top    = 0;
		src.bottom = originalSize.cy;

		nativeRect.left   = 0;
		nativeRect.right  = scaledSize.cx;
		nativeRect.top    = 0;
		nativeRect.bottom = scaledSize.cy;
	}
	ScaleRects(mode, &src, &nativeRect, &srcRect, &dstRect);

	// clip cursor into scaled area
	Original_ClipCursor(&dstRect);
	ZLOG_MESSAGE(zlog::SV_NOTICE, _T("ScalingHook srcRect = {%s}, dstRect = {%s}\n"), GetRectString(&srcRect), GetRectString(&dstRect));
}

template bool ScalingHook::HookDevice<IDirectInputDevice8A>(IDirectInputDevice8A* device);
template void ScalingHook::UnhookDevice<IDirectInputDevice8A>(IDirectInputDevice8A* device);

template bool ScalingHook::HookDevice<IDirectInputDevice8W>(IDirectInputDevice8W* device);
template void ScalingHook::UnhookDevice<IDirectInputDevice8W>(IDirectInputDevice8W* device);