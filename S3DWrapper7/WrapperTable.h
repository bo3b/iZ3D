/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <deque>

interface IWrapper;

interface DECLSPEC_UUID("666DCF5D-73DC-494c-BF7C-CFF4718E087B")
IWrapperTable: 
	public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE AddWrapper(IWrapper *pWrapper) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveWrapper(IWrapper *pWrapper) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSize(ULONG *pnSize) = 0;
	virtual HRESULT STDMETHODCALLTYPE DumpState() = 0;
};

class CWrapperTable: 
	public IWrapperTable
{
private:
	std::deque<IWrapper*>	m_Resources;

public:
	STDMETHODIMP AddWrapper(IWrapper *pWrapper);
	STDMETHODIMP RemoveWrapper(IWrapper *pWrapper);
	STDMETHODIMP GetSize(ULONG *pnSize);
	STDMETHODIMP DumpState();
};
