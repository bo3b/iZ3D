#pragma once

#include "Logging.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// forward
enum DeviceModes;
enum MultiWindowsModes;

template<typename Char> S3DAPI_API const Char* GetDeviceModeStringImpl(DeviceModes dm);
template<typename Char> S3DAPI_API const Char* GetMultiWindowsModeStringImpl(MultiWindowsModes dm);
template<typename Char> S3DAPI_API const Char* GetSecondWindowMessageStringImpl(WPARAM wParam);

template<typename Char> S3DAPI_API const Char* GetResultStringImpl(HRESULT hr);
template<typename Char> S3DAPI_API const Char* GetDeviceModeStringImpl(const DEVMODE* pMode);
template<typename Char> S3DAPI_API const Char* GetRectStringImpl(CONST RECT* pRect);
template<typename Char> S3DAPI_API const Char* GetSizeStringImpl(CONST SIZE* pSize);
template<typename Char> S3DAPI_API const Char* GetWindowRectStringImpl(HWND hWnd);
template<typename Char> S3DAPI_API const Char* GetMouseWindowMessageStringImpl(WPARAM wParam);
template<typename Char> S3DAPI_API const Char* GetMonitorInfoFlagsStringImpl(DWORD flags);
template<typename Char> S3DAPI_API const Char* GetMonitorInfoStringImpl(LPMONITORINFO lpmi);

inline const TCHAR* GetDeviceModeString(DeviceModes dm)				{ return GetDeviceModeStringImpl<TCHAR>(dm); }
inline const TCHAR* GetMultiWindowsModeString(MultiWindowsModes dm)	{ return GetMultiWindowsModeStringImpl<TCHAR>(dm); }
inline const TCHAR* GetSecondWindowMessageString(WPARAM wParam)		{ return GetSecondWindowMessageStringImpl<TCHAR>(wParam); }

inline const TCHAR* GetResultString(HRESULT hr)							{ return GetResultStringImpl<TCHAR>(hr); }
inline const TCHAR* GetDeviceModeString(const DEVMODE* pMode)			{ return GetDeviceModeStringImpl<TCHAR>(pMode); }
inline const TCHAR* GetRectString(CONST RECT* pRect)					{ return GetRectStringImpl<TCHAR>(pRect); }
inline const TCHAR* GetSizeString(CONST SIZE* pSize)					{ return GetSizeStringImpl<TCHAR>(pSize); }
inline const TCHAR* GetWindowRectString(HWND hWnd)						{ return GetWindowRectStringImpl<TCHAR>(hWnd); }
inline const TCHAR* GetMouseWindowMessageString(WPARAM wParam)			{ return GetMouseWindowMessageStringImpl<TCHAR>(wParam); }
inline const TCHAR* GetMonitorInfoFlagsString(DWORD flags)				{ return GetMonitorInfoFlagsStringImpl<TCHAR>(flags); }
inline const TCHAR* GetMonitorInfoString(LPMONITORINFO lpmi)			{ return GetMonitorInfoStringImpl<TCHAR>(lpmi); }
