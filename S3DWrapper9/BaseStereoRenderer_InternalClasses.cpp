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
#include "BaseStereoRenderer_InternalClasses.h"

SurfaceData::SurfaceData()
{
	m_pMainSurf = NULL;
	m_pMainTex = NULL;;
	type = Mono;
}

void SurfaceData::Clear()
{
	m_pMainSurf = NULL;
	m_pRightSurf.Release();
	m_pMainTex = NULL;
	type = Mono;
}

bool SurfaceData::SetType( TextureType textureType )
{
	if (m_pMainTex)	
	{
		DEBUG_TRACE3(_T("\tSetTextureType %p [%s]: TextureType = %s\n"), m_pMainTex, GetObjectName(m_pMainTex), GetTextureTypeString(textureType));

		if (type != textureType)	
		{
			type = textureType;
			return SetTextureType(m_pMainTex, type);
		}
		else
			return true;
	}
	else
		return false;	
}

bool SurfaceData::SetSurfaceType( TextureType textureType )
{
	if (m_pMainTex)
		return SetType(textureType);
	else if (m_pMainSurf)
	{
		DEBUG_TRACE3(_T("\tSetSurfaceType %p [%s]: TextureType = %s\n"), m_pMainSurf, GetObjectName(m_pMainSurf), GetTextureTypeString(textureType));

		if (type != textureType)	
		{
			type = textureType;
			return ::SetSurfaceType(m_pMainSurf, type);
		}
		else
			return true;
	}
	else
		return false;
}

void SurfaceData::Init( IDirect3DSurface9* pLeft, IDirect3DSurface9* pRight )
{
	m_pMainSurf = pLeft;
	m_pRightSurf = pRight;
	m_pMainTex = NULL;
	if(pLeft)
	{
		if (GetParentTexture(pLeft, &m_pMainTex))
		{
			m_pMainTex->Release(); // decrease reference count
			type = GetTextureType(m_pMainTex);
		}
		else type = Mono;
	}
	else type = Mono;
}

void SurfaceData::Init( IDirect3DSurface9* pLeft )
{
	m_pMainSurf = pLeft;
	m_pRightSurf.Release();
	m_pMainTex = NULL;
	if (pLeft)
	{
		GetStereoObject(pLeft, &m_pRightSurf); 
	}
	if(pLeft)
	{
		if (GetParentTexture(pLeft, &m_pMainTex)) {
			m_pMainTex->Release(); // decrease reference count
			type = GetTextureType(m_pMainTex);
		} else if (m_pMainSurf)
			type = GetSurfaceType(m_pMainSurf);
		else type = Mono;
	}
	else type = Mono;
}

TexData::TexData():
m_pLeft( NULL ), m_pWideTexture( NULL )
{
}

void TexData::Init(IDirect3DBaseTexture9* pTexture, IDirect3DBaseTexture9* pMainTexture)
{
	m_pLeft = NULL;
	m_pLeft = pTexture;
	m_pWideTexture = NULL;
	m_pRight.Release();
	if (pMainTexture && GetTextureType(pMainTexture) == Stereo)
	{
		GetStereoObject(pTexture, &m_pRight);
		DEBUG_TRACE2(_T("\tpRightTex = %p [%s])\n"), m_pRight, GetObjectName(m_pRight));
	}
}
