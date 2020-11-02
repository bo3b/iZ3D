/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <hash_map>
#include "boost/unordered_map.hpp"

interface IWrapper;

MIDL_INTERFACE("666DCF5D-73DC-494c-BF7C-CFF4718E087B")
IWrapperTable : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetWrapper(IUnknown *pReal, IWrapper **ppWrapper) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddWrapper(IUnknown *pReal, IWrapper *pWrapper) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveWrapper(IUnknown *pReal) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSize(ULONG *pnSize) = 0;
	virtual HRESULT STDMETHODCALLTYPE DumpState() = 0;
};

#ifndef _DEBUG
typedef boost::unordered_map<IUnknown *, IWrapper *> WrappersMap_t;
#else
typedef stdext::hash_map<IUnknown *, IWrapper *> WrappersMap_t;
#endif

class CWrapperTable: 
	public IWrapperTable,
	public CComObjectRoot,
	public CComCoClass<CWrapperTable>
{
	WrappersMap_t	m_WrapperMap;
public:
	BEGIN_COM_MAP(CWrapperTable)
		COM_INTERFACE_ENTRY(IWrapperTable)
	END_COM_MAP()
	STDMETHODIMP GetWrapper(IUnknown *pReal, IWrapper **ppWrapper);
	STDMETHODIMP AddWrapper(IUnknown *pReal, IWrapper *pWrapper);
	STDMETHODIMP RemoveWrapper(IUnknown *pReal);
	STDMETHODIMP GetSize(ULONG *pnSize);
	STDMETHODIMP DumpState();
};
