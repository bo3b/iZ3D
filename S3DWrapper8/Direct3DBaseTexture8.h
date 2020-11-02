/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include "resource.h"
#include "Direct3DResource8.h"

template <class Q, class T>
class CDirect3DBaseTexture8: public CDirect3DResource8<Q, T>
{
public:
    STDMETHODIMP_(DWORD) SetLOD(DWORD LODNew)
	{
		DEBUG_TRACE3("CDirect3DBaseTexture8::SetLOD\n");
        return m_pReal->SetLOD(LODNew);
    }
    STDMETHODIMP_(DWORD) GetLOD()
	{
		DEBUG_TRACE3("CDirect3DBaseTexture8::GetLOD\n");
        return m_pReal->GetLOD();
    }
    STDMETHODIMP_(DWORD) GetLevelCount()
	{
		DEBUG_TRACE3("CDirect3DBaseTexture8::GetLevelCount\n");
        return m_pReal->GetLevelCount();
    }
};
