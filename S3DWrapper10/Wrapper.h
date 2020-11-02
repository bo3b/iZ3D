#pragma once

#include <atlbase.h>
#include <atlcom.h>
#pragma warning (disable: 4189)
#pragma warning (disable: 4100)

#define NSTRACEWRAPPER \
	DEBUG_TRACE3(_T("%s Real(%p) Wrapper<%p> WrapperRef=%d, RealRef=%d\n"), \
	_T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, GetWrapperRefCount(), GetRealRefCount());

inline CONST TCHAR * GetUUID(IDXGISwapChain *pReal)
{	return _T("IID_IDXGISwapChain");	}
inline CONST TCHAR * GetUUID(IDXGIFactory *pReal)
{	return _T("IID_IDXGIFactory");	}

struct IWrapper;
MIDL_INTERFACE("FCF7771E-D79E-4fa1-AFE6-8BC7F69E7148")
IWrapper : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init(IUnknown *pReal) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetReal(IUnknown **ppReal) = 0;
};

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
		DEBUG_TRACE2(_T("%s Real(%p) Wrapper<%p> returns %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, nRefCount);
		return nRefCount;
	}
	inline ULONG InternalRelease()
	{
		ULONG nRefCount = CComObjectRoot::InternalRelease();
		DEBUG_TRACE2(_T("%s Real(%p) Wrapper<%p> returns %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, nRefCount);
		return nRefCount;
	}

public:
	CWrapper() { m_pReal = 0; }

	inline void FinalRelease()
	{
		HRESULT hResult = S_OK;
		DEBUG_TRACE2(_T("%s Real(%p) Wrapper<%p> RealRefCount = %d\n"), _T( __FUNCTION__ ) , (IUnknown *)m_pReal, this, GetRealRefCount());
	}

	STDMETHODIMP Init(IUnknown *pReal)
	{
		HRESULT hResult = S_OK;
		NSCALL(pReal->QueryInterface(&m_pReal));
		return hResult;
	}

	STDMETHODIMP GetReal(IUnknown **ppReal)
	{
		HRESULT hResult = S_OK;
		*ppReal = m_pReal;
		return hResult;
	}

	template <class Q, class T> 
	inline HRESULT CreateWrapper(T *pD3D10Object, Q **ppD3D9Object)
	{
		return Wrap(pD3D10Object, ppD3D9Object, TRUE);
	}

	template <class Q, class T> 
	inline HRESULT Wrap(T *pD3D10Object, Q **ppD3D9Object, BOOL bCreateAlways = FALSE)
	{
		HRESULT hResult = S_OK;
		_ASSERT(ppD3D9Object != NULL);
		_ASSERT(pD3D10Object != NULL);
		*ppD3D9Object = 0;

		CComPtr<IWrapper> pWrapper;
		if (!bCreateAlways)
		{
			NSCALL(GetWrapper<T>(pD3D10Object, &pWrapper));
		}
		BOOL bCreatedNew = FALSE;
		if (pWrapper == 0)
		{
			NSCALL(ConstructWrapper(pD3D10Object, &pWrapper));
			NSPTR(pWrapper);
			NSCALL(pWrapper->Init(pD3D10Object));
			bCreatedNew = TRUE;
		}
		NSCALL(pWrapper.QueryInterface(ppD3D9Object));
#ifndef FINAL_RELEASE
		NSPTR(*ppD3D9Object);
		if (bCreatedNew) {
			DEBUG_TRACE2(_T("Create Wrapper %s Real(%p) Wrapper<%p>\n"), 
				GetUUID(pD3D10Object), pD3D10Object, *ppD3D9Object);
		} else {
			DEBUG_TRACE3(_T("Wrap %s Real(%p) Wrapper<%p>\n"), 
				GetUUID(pD3D10Object), pD3D10Object, *ppD3D9Object);
		}
		pWrapper.Release();
		NSTRACEWRAPPER;
#endif
		return hResult;
	}

	template <class T, class C, class Q>
	inline T* Unwrap(Q *pD3D9Object)
	{
		HRESULT hResult = S_OK;
		if (pD3D9Object == NULL)
		{
			return NULL;
		}
		C* pWrapper = (C*)pD3D9Object;
		T* pReal;
		NSCALL(pWrapper->GetReal((IUnknown**)&pReal));
		if(!pReal)
			return NULL;
		DEBUG_TRACE3(_T("Unwrap %s Real(%p), WrapperD3D8<%p> %d (%d)\n"), 
			GetUUID(pReal), (T *)pReal, pD3D9Object, GetWrapperRefCount(), GetRealRefCount());
		return pReal;
	}
};
