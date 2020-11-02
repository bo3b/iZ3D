#pragma once
#include <d3d9.h>

class CProxyQuery : public IDirect3DQuery9
{
public:	
	CProxyQuery(IDirect3DQuery9* pQuery, D3DQUERYTYPE qType) { m_pQuery = pQuery; m_Type = qType;}
	virtual ~CProxyQuery(void)	{}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG,AddRef)(THIS);
	STDMETHOD_(ULONG,Release)(THIS);

	/*** IDirect3DQuery9 methods ***/
	STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice);
	STDMETHOD_(D3DQUERYTYPE, GetType)(THIS);
	STDMETHOD_(DWORD, GetDataSize)(THIS);
	STDMETHOD(Issue)(THIS_ DWORD dwIssueFlags);
	STDMETHOD(GetData)(THIS_ void* pData,DWORD dwSize,DWORD dwGetDataFlags);

protected:
	IDirect3DQuery9* m_pQuery;
	D3DQUERYTYPE	 m_Type;
};
