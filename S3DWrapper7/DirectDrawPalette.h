/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <ddraw.h>
#include "Direct3D7.h"
#include "Wrapper.h"

class DECLSPEC_UUID("E8255A7C-E8CA-4FBE-BFEF-B8299047737D")
	CDirectDrawPalette : 
	public CWrapper,
	public IDirectDrawPalette,
	public CComCoClass<CDirectDrawPalette>
{
private:
	BEGIN_COM_MAP(CDirectDrawPalette)
		COM_INTERFACE_ENTRY(IWrapper)
		COM_INTERFACE_ENTRY(IDirectDrawPalette)
	END_COM_MAP()
public:
	/*** IDirectDrawPalette methods ***/
	STDMETHODIMP GetCaps(LPDWORD lpdwCaps);
	STDMETHODIMP GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries);
	STDMETHODIMP Initialize(LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable);
	STDMETHODIMP SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries);
};

