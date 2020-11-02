/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "Direct3D7.h"
#include "Wrapper.h"

class DECLSPEC_UUID("E77A61A3-7730-429C-8C01-DBA7BB9FA331")
	CDirectDrawClipper : 
	public CWrapper,
	public IDirectDrawClipper,
	public CComCoClass<CDirectDrawClipper>
{
private:
	BEGIN_COM_MAP(CDirectDrawClipper)
		COM_INTERFACE_ENTRY(IWrapper)
		COM_INTERFACE_ENTRY(IDirectDrawClipper)
	END_COM_MAP()

public:
	/*** IDirectDrawClipper methods ***/
	STDMETHODIMP GetClipList(LPRECT lpRect, LPRGNDATA lpClipList, LPDWORD lpdwSize);
	STDMETHODIMP GetHWnd(HWND FAR *lphWnd);
	STDMETHODIMP Initialize(LPDIRECTDRAW lpDD, DWORD dwFlags);
	STDMETHODIMP IsClipListChanged(BOOL FAR *lpbChanged);
	STDMETHODIMP SetClipList(LPRGNDATA lpClipList, DWORD dwFlags);
	STDMETHODIMP SetHWnd(DWORD dwFlags, HWND hWnd);
private:
	CDirect3D7*		m_pDirect3D7;
	DWORD			m_dwFlags;
	HWND			m_hWnd;
};

