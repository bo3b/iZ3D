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
			NSTRACE(_T("%s: Real(*p) is already in the map iter->second(*p) pWrapper(*p)\n"), 
				_T( __FUNCTION__ ) , pReal, iter->second, pWrapper);
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

