/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "WrapperTable.h"
#pragma warning (disable: 4189)
#pragma warning (disable: 4100)

#define NSTRACE DEBUG_TRACE2

struct IWrapper;
MIDL_INTERFACE("FCF7771E-D79E-4fa1-AFE6-8BC7F69E7148")
IWrapper : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init(IWrapperTable *pWrapperTable, IUnknown *pReal) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetReal(IUnknown **ppReal) = 0;
};

HRESULT ConstructWrapper(IUnknown *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3D9 *pReal, IWrapper **ppWrapper);

inline ULONG GetRealRefCount(IUnknown *pUnk)
{
	if (!pUnk)
		return 0;
	pUnk->AddRef();
	return pUnk->Release();
}

template <class Q>
class CWrapper: 
	public Q,
	public IWrapper,
	public CComObjectRoot
{
protected:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;
	CComPtr<Q>              m_pReal;
	CComPtr<IWrapperTable>	m_pWrapperTable;

	inline ULONG GetRealRefCount()
	{
		IUnknown *pUnknown = m_pReal;
		if (pUnknown)
		{
			pUnknown->AddRef();
			return pUnknown->Release();
		}
		else
		{
			return 0;
		}
	}
	inline ULONG GetWrapperRefCount()
	{
		CComObjectRoot::InternalAddRef();
		return CComObjectRoot::InternalRelease();
	}

	inline ULONG InternalAddRef() 
	{ 
		ULONG nRefCount = CComObjectRoot::InternalAddRef();
		NSTRACE(_T("%s Real(%p) Wrapper<%p> returns %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, nRefCount);
		return nRefCount;
	}
	inline ULONG InternalRelease()
	{
		ULONG nRefCount = CComObjectRoot::InternalRelease();
		NSTRACE(_T("%s Real(%p) Wrapper<%p> returns %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, nRefCount);
		return nRefCount;
	}

public:
	CWrapper() { m_pReal = 0; }

	inline void FinalRelease()
	{
		HRESULT hResult = S_OK;
		NSTRACE(_T("%s Real(%p) Wrapper<%p> RealRefCount = %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, GetRealRefCount());
		NSCALL(m_pWrapperTable->RemoveWrapper(m_pReal));
	}

	STDMETHODIMP Init(IWrapperTable *pWrapperTable, IUnknown *pReal)
	{
		HRESULT hResult = S_OK;
		m_pWrapperTable = pWrapperTable;
		NSPTR(m_pWrapperTable);
		NSCALL(pReal->QueryInterface(&m_pReal));
		NSCALL(m_pWrapperTable->AddWrapper(m_pReal, this));
		return hResult;
	}

	STDMETHODIMP GetReal(IUnknown **ppReal)
	{
		HRESULT hResult = S_OK;
		*ppReal = m_pReal;
		return hResult;
	}
};
