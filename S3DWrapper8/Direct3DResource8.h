/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DDevice8.h"
#include "Direct3D8.h"
#include "Wrapper.h"

template <class Q, class T>
class CDirect3DResource8: public CWrapper<Q, T>
{
protected:
	using CWrapper<Q, T>::Lock;
	using CWrapper<Q, T>::Unlock;
  
public:
	IMPLEMENT_GET_DEVICE(m_pReal);
	IMPLEMENT_INTERNAL_RELEASE_WEAK();

    STDMETHODIMP SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
    {
		DEBUG_TRACE3("CDirect3DResource8::SetPrivateData\n");
		return (m_pReal->SetPrivateData(refguid, pData, SizeOfData, Flags));
    }

    STDMETHODIMP GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
	{
		DEBUG_TRACE3("CDirect3DResource8::GetPrivateData\n");
		return (m_pReal->GetPrivateData(refguid, pData, pSizeOfData));
    }

    STDMETHODIMP FreePrivateData(REFGUID refguid)
	{
		DEBUG_TRACE3("CDirect3DResource8::FreePrivateData\n");
		return (m_pReal->FreePrivateData(refguid));
    }

    STDMETHODIMP_(DWORD) SetPriority(DWORD PriorityNew)
	{
		DEBUG_TRACE3("CDirect3DResource8::SetPriority\n");
		return m_pReal->SetPriority(PriorityNew);
    }

    STDMETHODIMP_(DWORD) GetPriority()
	{
		DEBUG_TRACE3("CDirect3DResource8::GetPriority\n");
		return m_pReal->GetPriority();
    }

    STDMETHODIMP_(void) PreLoad()
	{
		DEBUG_TRACE3("CDirect3DResource8::PreLoad\n");
		m_pReal->PreLoad();
    }

    STDMETHODIMP_(D3DRESOURCETYPE) GetType()
	{
		DEBUG_TRACE3("CDirect3DResource8::GetType\n");
		return m_pReal->GetType();
    }
};

