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
#include "DirectDrawClipper.h"

STDMETHODIMP CDirectDrawClipper::Initialize( LPDIRECTDRAW lpDD, DWORD dwFlags )
{
	CComQIPtr<IWrapperTable> pWrapperTable = lpDD;	
	Init(pWrapperTable);
	m_pDirect3D7 = (CDirect3D7*)pWrapperTable.p;
	m_dwFlags = dwFlags;
	m_pDirect3D7->AddWrapper(this);
	m_hWnd = NULL;
	return S_OK;
}

STDMETHODIMP CDirectDrawClipper::GetClipList( LPRECT lpRect, LPRGNDATA lpClipList, LPDWORD lpdwSize )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawClipper::GetHWnd( HWND FAR *lphWnd )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawClipper::IsClipListChanged( BOOL FAR *lpbChanged )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawClipper::SetClipList( LPRGNDATA lpClipList, DWORD dwFlags )
{
	DEBUG_MESSAGE("Not implemented: %s\n", __FUNCTION__);
	return E_NOTIMPL;
}

STDMETHODIMP CDirectDrawClipper::SetHWnd( DWORD dwFlags, HWND hWnd )
{
	DEBUG_TRACE3("%s\n", __FUNCTION__);
	m_hWnd = hWnd;
	return S_OK;
}