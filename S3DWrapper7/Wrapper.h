/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "WrapperTable.h"

#include <InitGuid.h>

DEFINE_GUID(IID_IWrapper, 0xFCF7771E, 0xD79E, 0x4fa1, 0xAF, 0xE6, 0x8B, 0xC7, 0xF6, 0x9E, 0x71, 0x48);

interface DECLSPEC_UUID("FCF7771E-D79E-4fa1-AFE6-8BC7F69E7148")
IWrapper : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init(IWrapperTable *pWrapperTable) = 0;
};

inline ULONG GetRealRefCount(IUnknown *pUnk)
{
	if (!pUnk)
		return 0;
	pUnk->AddRef();
	return pUnk->Release();
}

class CWrapper: 
	public IWrapper,
	public CComObjectRoot
{
protected:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;
	CComPtr<IWrapperTable>	m_pWrapperTable;

	inline ULONG GetWrapperRefCount()
	{
		CComObjectRoot::InternalAddRef();
		return CComObjectRoot::InternalRelease();
	}

	inline ULONG InternalAddRef() 
	{ 
		ULONG nRefCount = CComObjectRoot::InternalAddRef();
		DEBUG_TRACE3("\t%s Wrapper<%p> returns %d\n", __FUNCTION__ , this, nRefCount);
		return nRefCount;
	}

	inline STDMETHODIMP_(ULONG) InternalRelease()
	{
		ULONG nRefCount = CComObjectRoot::InternalRelease();
		DEBUG_TRACE3("\t%s Strong Wrapper<%p> returns %d\n", __FUNCTION__ , this, nRefCount);
		return nRefCount;
	}

	inline HRESULT AddWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->AddWrapper(this));
		return hResult;
	}

	inline HRESULT RemoveWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->RemoveWrapper(this));
		return hResult;
	}

public:
	CWrapper() { }

	inline void FinalRelease()
    {
		HRESULT hResult = S_OK;
		DEBUG_TRACE2("Final Release Wrapper<%p>\n\t%s\n", this, __FUNCTION__ );
		NSCALL(RemoveWrapper());
	}

	STDMETHODIMP Init(IWrapperTable *pWrapperTable)
	{
		HRESULT hResult = S_OK;
		m_pWrapperTable = pWrapperTable;
		NSPTR(m_pWrapperTable);
		NSCALL(AddWrapper());
		return hResult;
	}
};
