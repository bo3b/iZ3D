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
#include "StdAfx.h"
#include "WrapperTable.h"
#include "Wrapper.h"


STDMETHODIMP CWrapperTable::GetWrapper(IUnknown *pReal, IWrapper **ppWrapper)
{
	HRESULT hResult = S_OK;
	CComPtr<IWrapper> pWrapper;
	if (pReal)
	{
		WrappersMap_t::iterator iter = m_WrapperMap.find(pReal);
		if (iter != m_WrapperMap.end())
		{
			pWrapper = iter->second;
		}
	}
	pWrapper.CopyTo(ppWrapper);
	return hResult;
}

STDMETHODIMP CWrapperTable::AddWrapper(IUnknown *pReal, IWrapper *pWrapper)
{
	HRESULT hResult = S_OK;
	if (pReal)
	{
		WrappersMap_t::iterator iter = m_WrapperMap.find(pReal);
		if (iter != m_WrapperMap.end())
		{
			NSTRACE("%s: Real(*p) is already in the map iter->second(*p) pWrapper(*p)\n", 
				__FUNCTION__ , pReal, iter->second, pWrapper);
			NSFAILIF(iter->second != pWrapper);
		}
		m_WrapperMap[pReal] = pWrapper;
	}
	return hResult;
}

STDMETHODIMP CWrapperTable::RemoveWrapper(IUnknown *pReal)
{
	HRESULT hResult = S_OK;
	if (pReal)
	{
		m_WrapperMap.erase(pReal);
	}
	return hResult;
}

STDMETHODIMP CWrapperTable::GetSize(ULONG *pnSize)
{
	*pnSize = (ULONG)m_WrapperMap.size();
	return S_OK;
}

#define DUMP_WRAPPER(Text) \
	pReal = 0; \
	pWrapper->GetRealVirt(&pReal); \
	CComQIPtr<I##Text> p##Text(pReal); \
	if (p##Text) \
{ \
	DEBUG_TRACE2("\tI%s Real(%p) Wrapper<%p>\n", \
	#Text, (IUnknown *)pReal, (IUnknown *)pWrapper);\
	break; \
} \

char* D3DPOOL_NAME[] = { "D3DPOOL_DEFAULT", "D3DPOOL_MANAGED",	"D3DPOOL_SYSTEMMEM", "D3DPOOL_SCRATCH" };

#define DUMP_WRAPPER_POOL(Text, DESC) \
	pReal = 0; \
	pWrapper->GetRealVirt(&pReal); \
	CComQIPtr<I##Text> p##Text(pReal); \
	if (p##Text) \
{ \
	DESC desc; \
	p##Text->GetDesc(&desc);\
	DEBUG_TRACE2("\tI%s Real(%p) Wrapper<%p>  POOL = %s\n", \
#Text, (IUnknown *)pReal, (IUnknown *)pWrapper, D3DPOOL_NAME[desc.Pool]);\
	break; \
} \

#define DUMP_WRAPPER_POOL_LEVEL(Text, DESC) \
	pReal = 0; \
	pWrapper->GetRealVirt(&pReal); \
	CComQIPtr<I##Text> p##Text(pReal); \
	if (p##Text) \
{ \
	DESC desc; \
	p##Text->GetLevelDesc(0, &desc);\
	DEBUG_TRACE2("\tI%s Real(%p) Wrapper<%p>  POOL = %s\n", \
#Text, (IUnknown *)pReal, (IUnknown *)pWrapper, D3DPOOL_NAME[desc.Pool]);\
	break; \
} \

STDMETHODIMP CWrapperTable::DumpState()
{
	NSTRACE("WrapperTable size = %d, objects:\n", m_WrapperMap.size());
	IWrapper* pWrapper; 
	IUnknown* pReal;
	WrappersMap_t::const_iterator iter = m_WrapperMap.begin();
	for (; iter != m_WrapperMap.end(); ++iter)
	{
		pWrapper = iter->second;
		while (TRUE)
		{
			DUMP_WRAPPER(Direct3D9);
			DUMP_WRAPPER(Direct3DDevice9);
			DUMP_WRAPPER_POOL_LEVEL(Direct3DCubeTexture9, D3DSURFACE_DESC);
			DUMP_WRAPPER_POOL(Direct3DIndexBuffer9, D3DINDEXBUFFER_DESC);
			DUMP_WRAPPER(Direct3DStateBlock9);
			DUMP_WRAPPER_POOL(Direct3DSurface9, D3DSURFACE_DESC);
			DUMP_WRAPPER(Direct3DSwapChain9);
			DUMP_WRAPPER_POOL_LEVEL(Direct3DTexture9, D3DSURFACE_DESC);
			DUMP_WRAPPER_POOL(Direct3DVertexBuffer9, D3DVERTEXBUFFER_DESC);
			DUMP_WRAPPER(Direct3DVertexDeclaration9);
			DUMP_WRAPPER(Direct3DVolume9);
			DUMP_WRAPPER_POOL_LEVEL(Direct3DVolumeTexture9, D3DVOLUME_DESC);
			break;
		}
	
	}
	return S_OK;
}
