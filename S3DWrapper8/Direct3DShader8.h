/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <vector>
#include "d3d8shader.h"
#include "D3DX9Mesh.h"

class DECLSPEC_UUID("3C8228E1-347E-4a5c-9531-FDF43F6EFA20") CDirect3DVertexShader8: 
	public IWrapper,
	public CComCoClass<CDirect3DVertexShader8>,
	public CComObjectRoot
{
protected:
	using CComObjectRoot::Lock;
	using CComObjectRoot::Unlock;
	CComPtr<IDirect3DVertexDeclaration9> m_pReal;
	CComPtr<IWrapperTable>	m_pWrapperTable;
	CComPtr<IDirect3DVertexShader9> m_pShader;
	DWORD* m_pOldDecl;
	DWORD m_pOldDeclSize;

	BEGIN_COM_MAP(CDirect3DVertexShader8)
		COM_INTERFACE_ENTRY(IWrapper)
	END_COM_MAP()

public:

	inline IDirect3DVertexShader9* GetVS()
	{
		return m_pShader;
	}
	inline void SetVS(IDirect3DVertexShader9* pVS)
	{
		m_pShader = pVS;
	}
	inline IDirect3DVertexDeclaration9* GetVD()
	{
		return m_pReal;
	}
	inline void GetVD8(void* pData, DWORD* pSizeOfData)
	{
		memcpy(pData, m_pOldDecl, m_pOldDeclSize);
		*pSizeOfData = m_pOldDeclSize;
	}
	inline void SetVD8(CONST DWORD* pDeclaration, DWORD declSize)
	{
		m_pOldDeclSize = declSize;
		if (m_pOldDeclSize != 0)
		{
			m_pOldDecl = DNew DWORD[declSize];
			memcpy(m_pOldDecl, pDeclaration, m_pOldDeclSize);
		}
	}

	STDMETHODIMP_(VOID) SetMcClaudFlag()   {}
	STDMETHODIMP_(VOID) ClearMcClaudFlag() {}

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
	inline ULONG InternalRelease()
	{
		ULONG nRefCount = CComObjectRoot::InternalRelease();
		DEBUG_TRACE3("\t%s Real(%p) Wrapper<%p> returns %d (%d)\n", __FUNCTION__ , (IUnknown *)m_pReal, this, nRefCount, GetRealRefCount());
		return nRefCount;
	}

	HRESULT AddWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->AddWrapper(m_pReal, this));
		return hResult;
	}

	HRESULT RemoveWrapper()
	{
		HRESULT hResult = S_OK;
		NSCALL(m_pWrapperTable->RemoveWrapper(m_pReal));
		return hResult;
	}

	HRESULT GetWrapper(IUnknown *pReal, IWrapper **ppWrapper)
	{
		HRESULT hResult = S_OK;;
		NSCALL(m_pWrapperTable->GetWrapper(pReal, ppWrapper));
		DEBUG_TRACE3("GetWrapper %s Real(%p) Wrapper<%p>\n", 
			GetUUID(pReal), (IDirect3DVertexDeclaration9 *)pReal, *ppWrapper);
		return hResult;
	}

public:
	CDirect3DVertexShader8() { m_pReal = 0; m_pShader = 0; m_pOldDecl = 0; m_pOldDeclSize = 0; }

	inline void FinalRelease()
	{
		HRESULT hResult = S_OK;
		NSTRACE("%s Real(%p) Wrapper<%p> RealRefCount = %d\n", __FUNCTION__ , (IUnknown *)m_pReal, this, GetRealRefCount());
		NSCALL(RemoveWrapper());
#ifndef FINAL_RELEASE
		ULONG nRealRefCount = 0;
		if (m_pReal)
		{
			nRealRefCount = GetRealRefCount();
		}
#endif
		if (m_pOldDecl)
		{
			delete[] m_pOldDecl;
		}
	}

	STDMETHODIMP Init(IWrapperTable *pWrapperTable, IUnknown *pReal)
	{
		HRESULT hResult = S_OK;
		m_pWrapperTable = pWrapperTable;
		NSPTR(m_pWrapperTable);
		NSCALL(pReal->QueryInterface(&m_pReal));
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
};


#if 0
#define nl "   "
#define id ""
#else
#define nl "\n"
#define id "    "
#endif

#define MAXSHADERSIZE 65536

struct CONSTANT_REGISTER 
{
	int index;
	float x, y, z, w;
};

struct VSDeclData
{
	D3DVERTEXELEMENT9* pVertexElements;
	std::vector<DWORD> reg_vec;
	std::vector<CONSTANT_REGISTER> const_vec;
};

extern int GetVSDecl( VSDeclData &declData, char* vsDecl );
extern HRESULT ConvertVSDecl( CONST DWORD* pDeclaration, DWORD &declSize, VSDeclData &declData, char* buffer = NULL );
extern HRESULT CompileVertexShader(IDirect3DVertexShader9** ppVS, LPSTR sstream, IDirect3DDevice9 * pDevice, char *buffer = NULL );
extern void WriteShaderTextToFile(TCHAR* fileName, char* shaderText, char* buffer = NULL);
extern void WriteErrorTextToFile(TCHAR* fileName, char* errorText);

extern int ParseVertexFunction(DWORD* command);
