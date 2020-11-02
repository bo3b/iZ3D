#include "StdAfx.h"
#include "ProxyQuery.h"

/*** IUnknown methods ***/
STDMETHODIMP CProxyQuery::QueryInterface(REFIID riid, void** ppvObj)
{
	if( ppvObj == NULL )
		return E_POINTER;

	if(InlineIsEqualGUID(riid, IID_IDirect3DQuery9)) 
	{
		* ppvObj = (IDirect3DQuery9*)this;
		return S_OK;
	}
	
	return m_pQuery->QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) CProxyQuery::AddRef()
{
	return m_pQuery->AddRef();
}

STDMETHODIMP_(ULONG) CProxyQuery::Release()
{
	DWORD refCount = m_pQuery->Release();
	if(refCount == 0)
		delete this;
	return refCount;
}

/*** IDirect3DQuery9 methods ***/
STDMETHODIMP CProxyQuery::GetDevice(IDirect3DDevice9** ppDevice)
{
	return m_pQuery->GetDevice(ppDevice);
}

STDMETHODIMP_(D3DQUERYTYPE) CProxyQuery::GetType()
{
	return m_pQuery->GetType();
}

STDMETHODIMP_(DWORD) CProxyQuery::GetDataSize()
{
	return m_pQuery->GetDataSize();
}

STDMETHODIMP CProxyQuery::Issue(DWORD dwIssueFlags)
{
	//if(m_Type == D3DQUERYTYPE_OCCLUSION)
	//	return S_OK;
	HRESULT hr = m_pQuery->Issue(dwIssueFlags);
	DEBUG_TRACE3(_T("CProxyQuery::Issue(0x%X)  Flags = %u, HResult = %u\n"), this,  dwIssueFlags, hr);	
	return hr;
}

STDMETHODIMP CProxyQuery::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags)
{
	/*
	if(m_Type == D3DQUERYTYPE_EVENT)
	{
		if(pData)
		{
			*(DWORD*)pData = 1;
			return S_OK;
		}
	}
	*/
	//if(m_Type == D3DQUERYTYPE_OCCLUSION)
	//{
	//	if(pData)
	//	{
	//		*(DWORD*)pData = 255;
	//		return S_OK;
	//	}
	//	else
	//		return S_OK;
	//}
	//{
//		if(pData)
		{
//			*(DWORD*)pData = 0;
			HRESULT hr = m_pQuery->GetData(pData, dwSize, dwGetDataFlags);
			DEBUG_TRACE3(_T("CProxyQuery::GetData(0x%X)  Type = %s, *pData = 0x%X, size = %u, flag = %u, HResult = %u\n"), this, GetQueryTypeString(m_Type), Indirect((DWORD*)pData), dwSize, dwGetDataFlags, hr);	
			//*(DWORD*)pData = 255;
			//if(SUCCEEDED(hr) && m_Type == D3DQUERYTYPE_OCCLUSION && gInfo.EnableStereo)
			//	*(DWORD*)pData *= 2;
			return hr;
		}
//	HRESULT hr = m_pQuery->GetData(pData, dwSize, dwGetDataFlags);
//	return hr;
}
