/* IZ3D_FILE: $Id$ 
*
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*
* $Author$
* $Revision$
* $Date$
* $LastChangedBy$
* $URL$
*/
#include "stdafx.h"
#include "DirectDrawPalette.h"

STDMETHODIMP CDirectDrawPalette::Initialize( LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawPalette::GetCaps( LPDWORD lpdwCaps )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawPalette::GetEntries( DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawPalette::SetEntries( DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}
