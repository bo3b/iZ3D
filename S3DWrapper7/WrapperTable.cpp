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
#include <algorithm>

STDMETHODIMP CWrapperTable::AddWrapper( IWrapper *pRes )
{
	HRESULT hResult = S_OK;
	m_Resources.push_back(pRes);
	return hResult;
}

STDMETHODIMP CWrapperTable::RemoveWrapper( IWrapper *pRes )
{
	HRESULT hResult = S_OK;
	std::deque<IWrapper*>::const_iterator it;
	it = std::find(m_Resources.begin(), m_Resources.end(), pRes);
	if (it != m_Resources.end())
	{
		m_Resources.erase(it);
	}
	return hResult;
}

STDMETHODIMP CWrapperTable::GetSize(ULONG *pnSize)
{
	*pnSize = (ULONG)m_Resources.size();
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
	DEBUG_TRACE2("Resources size = %d, objects:\n", m_Resources.size());
	/*IWrapper* pWrapper; 
	IUnknown* pReal;
	std::map<IUnknown *, IWrapper *>::const_iterator iter = m_WrapperMap.begin();
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
	}*/
	return S_OK;
}
