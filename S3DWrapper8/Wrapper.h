/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "WrapperTable.h"
#include "d3d9_8.h"

#define NSTRACE DEBUG_TRACE2
#define NSTRACEWRAPPER \
	DEBUG_TRACE3("%s Real(%p) Wrapper<%p> WrapperRef=%d, RealRef=%d\n", \
	__FUNCTION__ , (IUnknown *)m_pReal, this, GetWrapperRefCount(), GetRealRefCount());

struct IWrapper;
MIDL_INTERFACE("FCF7771E-D79E-4fa1-AFE6-8BC7F69E7148")
IWrapper : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Init(IWrapperTable *pWrapperTable, IUnknown *pReal) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRealVirt(IUnknown **ppReal) = 0;
	virtual VOID	STDMETHODCALLTYPE SetMcClaudFlag() = 0;
	virtual VOID	STDMETHODCALLTYPE ClearMcClaudFlag() = 0;
};

HRESULT ConstructWrapper(IUnknown *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3D9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DDevice9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DCubeTexture9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DIndexBuffer9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DPixelShader9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DQuery9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DStateBlock9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DSurface9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DSwapChain9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DTexture9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVertexBuffer9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVertexDeclaration9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVertexShader9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVolume9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVolumeTexture9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DBaseTexture9 *pReal, IWrapper **ppWrapper);
HRESULT ConstructWrapper(IDirect3DVertexDeclaration9 *pReal, IWrapper **ppWrapper);

char * GetUUID(IUnknown *pUnknown);
char * GetUUID(IDirect3D9 *pReal);
char * GetUUID(IDirect3DDevice9 *pReal);
char * GetUUID(IDirect3DCubeTexture9 *pReal);
char * GetUUID(IDirect3DIndexBuffer9 *pReal);
char * GetUUID(IDirect3DPixelShader9 *pReal);
char * GetUUID(IDirect3DQuery9 *pReal);
char * GetUUID(IDirect3DStateBlock9 *pReal);
char * GetUUID(IDirect3DSurface9 *pReal);
char * GetUUID(IDirect3DSwapChain9 *pReal);
char * GetUUID(IDirect3DTexture9 *pReal);
char * GetUUID(IDirect3DVertexBuffer9 *pReal);
char * GetUUID(IDirect3DVertexDeclaration9 *pReal);
char * GetUUID(IDirect3DVertexShader9 *pReal);
char * GetUUID(IDirect3DVolume9 *pReal);
char * GetUUID(IDirect3DVolumeTexture9 *pReal);
char * GetUUID(IDirect3DBaseTexture9 *pReal);
char * GetUUID(IDirect3DVertexDeclaration9 *pReal);

inline ULONG GetRealRefCount(IUnknown *pUnk)
{
	if (!pUnk)
		return 0;
	pUnk->AddRef();
	return pUnk->Release();
}

//Base is the user's class that derives from CWrapper and whatever
//interfaces the user wants to support on the object
template <class Base>
class CWrapperObject : public Base
{
public:
	typedef Base _BaseClass;
	CWrapperObject(void* = NULL) throw()
	{
		_pAtlModule->Lock();
	}
	// Set refcount to -(LONG_MAX/2) to protect destruction and 
	// also catch mismatched Release in debug builds
	virtual ~CWrapperObject() throw()
	{
		m_dwRef = -(LONG_MAX/2);
		FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
		_AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
		_pAtlModule->Unlock();
	}
	//If InternalAddRef or InternalRelease is undefined then your class
	//doesn't derive from CComObjectRoot
	STDMETHOD_(ULONG, AddRef)() {return InternalAddRef();}
	STDMETHOD_(ULONG, Release)()
	{
		if(m_dwRef == 1 && m_bMcClaudFlag)
		{
			DEBUG_TRACE3("\t%s McClaud Real(%p) Wrapper<%p>. Do not destroy now.\n", __FUNCTION__ , (IUnknown *)m_pReal, this);
			return 0;
		}

		ULONG l = InternalRelease();
		if (l == 0)
			delete this;
		return l;
	}
	//if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw()
	{return _InternalQueryInterface(iid, ppvObject);}
	template <class Q>
	HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) throw()
	{
		return QueryInterface(__uuidof(Q), (void**)pp);
	}

	static HRESULT WINAPI CreateInstance(CWrapperObject<Base>** pp) throw();
};

template <class T>
class CWrapperCoClass
{
public:
	template <class Q>
	static HRESULT CreateInstance(Q** pp)
	{
		ATLASSERT(pp != NULL);
		return ATL::CComCreator< CWrapperObject< T > >::CreateInstance(NULL, __uuidof(Q), (void**)pp);
	}
};

template <class Q, class T>
class CWrapper: 
	public Q,
	public IWrapper,
	public CComObjectRoot
{
protected:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;
	CComPtr<T>              m_pStrongReal;
	T*						m_pReal;
	CComPtr<IWrapperTable>	m_pWrapperTable;
	bool					m_bMcClaudFlag;

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
		DEBUG_TRACE3("\t%s Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, GetRealRefCount());
		return nRefCount;
	}

	inline STDMETHODIMP_(ULONG) InternalReleaseStrong()
	{
		ULONG nRefCount = CComObjectRoot::InternalRelease();
		DEBUG_TRACE3("\t%s Strong Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, GetRealRefCount());
		return nRefCount;
	}

#ifdef FINAL_RELEASE
	__forceinline
#endif
	STDMETHODIMP_(ULONG) InternalReleaseWeak(LONG_PTR CallingModule)
	{
		ULONG nRefCount = GetWrapperRefCount();
		if (nRefCount > 1)
		{
			nRefCount = CComObjectRoot::InternalRelease();
			DEBUG_TRACE3("\t%s Strong Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, GetRealRefCount());
		}
		else
		{
			DWORD nRealRefCount;
			if (CallingModule >= D3D9StartAddress && CallingModule <= D3D9EndAddress)
			{
				DEBUG_TRACE3("\tRelease() called from D3D9\n");
				nRealRefCount = 1;
			}
			else
				nRealRefCount = GetRealRefCount();
			if (nRealRefCount > 1)
			{
				if (m_pStrongReal)
				{
					m_pStrongReal.Release();
					nRealRefCount--;
					IUnknown* pWrap = NULL;
					DWORD size = sizeof(IUnknown *);
					if (FAILED(m_pReal->GetPrivateData(__uuidof(Q), (void *)pWrap, &size)))
					{
						m_pReal->SetPrivateData(__uuidof(Q), this, sizeof(IUnknown *), D3DSPD_IUNKNOWN);
						nRefCount = CComObjectRoot::InternalRelease();
						DEBUG_TRACE2("\t%s Set Weak Reference Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, nRealRefCount);
					}
				}
				DEBUG_TRACE3("\t%s Weak Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, nRealRefCount);
			}
			else
			{
				nRefCount = CComObjectRoot::InternalRelease();
				DEBUG_TRACE3("\t%s Strong (Final) Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, nRealRefCount);
			}
		}
		return nRefCount;
	}

	inline HRESULT AddWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->AddWrapper(m_pReal, this));
		return hResult;
	}

	inline HRESULT RemoveWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->RemoveWrapper(m_pReal));
		return hResult;
	}

	inline HRESULT GetWrapper(IUnknown *pReal, IWrapper **ppWrapper)
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->GetWrapper(pReal, ppWrapper));
		DEBUG_TRACE3("GetWrapper %s Real(%p) Wrapper<%p>\n", 
			GetUUID(pReal), (T *)pReal, *ppWrapper);
		return hResult;
	}

	template <class T>
	inline HRESULT GetWrapper(T *pReal, IWrapper **ppWrapper)
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->GetWrapper(pReal, (IWrapper **)ppWrapper));
		DEBUG_TRACE3("GetWrapper %s Real(%p) Wrapper<%p>\n", 
			GetUUID((T *)pReal), (T *)pReal, *ppWrapper);
		return hResult;
	}

public:
	CWrapper() { m_pReal = 0; m_pStrongReal = 0; m_bMcClaudFlag = false; }

	STDMETHODIMP_(VOID) SetMcClaudFlag()	{ m_bMcClaudFlag = TRUE;   }
	STDMETHODIMP_(VOID) ClearMcClaudFlag()	{ m_bMcClaudFlag = FALSE;  }
		
	inline void FinalRelease()
    {
		HRESULT hResult = S_OK;
		NSTRACE("Final Release Wrapper Real(%p) Wrapper<%p>\n\t%s\n", (IUnknown *)m_pReal, this, __FUNCTION__ );
		NSCALL(RemoveWrapper());
	}

	STDMETHODIMP Init(IWrapperTable *pWrapperTable, IUnknown *pReal)
	{
		HRESULT hResult = S_OK;
		m_pWrapperTable = pWrapperTable;
		NSPTR(m_pWrapperTable);
		NSCALL(pReal->QueryInterface(&m_pStrongReal));
		m_pReal = m_pStrongReal;
		NSCALL(AddWrapper());
		return hResult;
	}

	STDMETHODIMP GetReal(IUnknown **ppReal)
	{
		HRESULT hResult = S_OK;
		*ppReal = m_pReal;
		return hResult;
	}

	STDMETHODIMP GetRealVirt(IUnknown **ppReal)
	{
		return GetReal(ppReal);
	}

	template <class Q, class T> 
	inline HRESULT CreateWrapper(T *pD3D9Object, Q **ppD3D8Object)
	{
		return Wrap(pD3D9Object, ppD3D8Object, TRUE);
	}

	template <class Q, class T> 
	inline HRESULT Wrap(T *pD3D9Object, Q **ppD3D8Object, BOOL bCreateAlways = FALSE)
	{
		HRESULT hResult = S_OK;
		_ASSERT(ppD3D8Object != NULL);
		_ASSERT(pD3D9Object != NULL);
		*ppD3D8Object = 0;

		CComPtr<IWrapper> pWrapper;
		if (!bCreateAlways)
		{
			NSCALL(GetWrapper<T>(pD3D9Object, &pWrapper));
		}
		BOOL bCreatedNew = FALSE;
		if (pWrapper == 0)
		{
			NSCALL(ConstructWrapper(pD3D9Object, &pWrapper));
			NSPTR(pWrapper);
			NSCALL(pWrapper->Init(m_pWrapperTable, pD3D9Object));
			bCreatedNew = TRUE;
		}
		NSCALL(pWrapper.QueryInterface(ppD3D8Object));
#ifndef FINAL_RELEASE
		NSPTR(*ppD3D8Object);
		if (bCreatedNew) {
			NSTRACE("Create Wrapper %s Real(%p) Wrapper<%p>\n", 
				GetUUID(pD3D9Object), pD3D9Object, *ppD3D8Object);
		} else {
			DEBUG_TRACE3("Wrap %s Real(%p) Wrapper<%p>\n", 
				GetUUID(pD3D9Object), pD3D9Object, *ppD3D8Object);
		}
		pWrapper.Release();
		NSTRACEWRAPPER;
#endif
		return hResult;
	}
	template <class T, class C, class Q>
	inline T* Unwrap(Q *pD3D8Object)
	{
		HRESULT hResult = S_OK;
		if (pD3D8Object == NULL)
		{
			return NULL;
		}
		C* pWrapper = (C*)pD3D8Object;
		T* pReal;
		NSCALL(pWrapper->GetReal((IUnknown**)&pReal));
		if(!pReal)
			return NULL;
		DEBUG_TRACE3("Unwrap %s Real(%p), WrapperD3D8<%p> %d (%d)\n", GetUUID(pReal), (T *)pReal, pD3D8Object, GetWrapperRefCount(), GetRealRefCount());
		return pReal;
	}
};

static LONG_PTR D3D9StartAddress;
static LONG_PTR D3D9EndAddress;
void InitD3DInfo();

#define IMPLEMENT_GET_DEVICE(m_pReal) \
	STDMETHODIMP GetDevice(IDirect3DDevice8** ppDevice)\
{\
	NSTRACEWRAPPER;\
	CComPtr<IDirect3DDevice9> pD3D9Dev;\
    HRESULT hr = m_pReal->GetDevice(&pD3D9Dev);\
    if (SUCCEEDED(hr))\
	    Wrap(pD3D9Dev.p, ppDevice);\
	return hr;\
}

#define GET_CONTAINER(D3D9, D3D8)										\
	if (InlineIsEqualGUID(riid, IID_I##D3D8))							\
	{																	\
		CComPtr<I##D3D9> p##D3D9;										\
		hr = m_pReal->GetContainer(IID_I##D3D9, (void **)&p##D3D9);		\
		if (SUCCEEDED(hr))												\
		{																\
			CComPtr<I##D3D8> p##D3D8;									\
			Wrap(p##D3D9.p, &p##D3D8);									\
			hr = p##D3D8->QueryInterface(riid, ppContainer);			\
			return hr;													\
		}																\
	}

#define IMPLEMENT_GET_CONTAINER(m_pReal) \
	STDMETHODIMP GetContainer(REFIID riid,void** ppContainer)\
{\
	NSTRACEWRAPPER;\
	HRESULT hr = E_NOINTERFACE;\
	GET_CONTAINER(Direct3DBaseTexture9, Direct3DBaseTexture8);\
	GET_CONTAINER(Direct3DTexture9, Direct3DTexture8);\
	GET_CONTAINER(Direct3DCubeTexture9, Direct3DCubeTexture8);\
	GET_CONTAINER(Direct3DVolumeTexture9, Direct3DVolumeTexture8);\
	GET_CONTAINER(Direct3DResource9, Direct3DResource8);\
	GET_CONTAINER(Direct3DDevice9, Direct3DDevice8);\
	GET_CONTAINER(Unknown, Unknown);\
	return hr;\
}

#define IMPLEMENT_INTERNAL_RELEASE_STRONG() \
	__forceinline STDMETHODIMP_(ULONG) InternalRelease()\
{\
	return InternalReleaseStrong();\
}

extern "C" void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

#define g_CallingModule reinterpret_cast<LONG_PTR>(_ReturnAddress())

#define IMPLEMENT_INTERNAL_RELEASE_WEAK() \
	__forceinline STDMETHODIMP_(ULONG) InternalRelease()\
{\
	return InternalReleaseWeak(g_CallingModule);\
}
