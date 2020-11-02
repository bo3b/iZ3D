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
/*
AngelCode Tool Box Library
Copyright (c) 2007 Andreas Jonsson

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you 
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product 
documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.

Andreas Jonsson
andreas@angelcode.com
*/

#include "pcmp.h"
#include "acgfx_dynrender_dx9.h"

namespace acGraphics
{

//////////////////////////////////////////////////////////////////////////	

	static const D3DVERTEXELEMENT9			g_pDx9VertexElements[]	=
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0 },
		{ 0, 12, D3DDECLTYPE_UBYTE4  , 0, D3DDECLUSAGE_BLENDINDICES, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0 },
		D3DDECL_END()
	};

//////////////////////////////////////////////////////////////////////////	

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }
#endif

	CDynRender_dx9::CDynRender_dx9( IDirect3DDevice9* pDevice_ )
		:	m_pDevice		( pDevice_ )
		,	m_pVertexBuffer ( NULL )
		,	m_pDeclaration	( NULL )
	{
		_ASSERT( m_pDevice );
		m_pDevice->AddRef();

		HRESULT hr = m_pDevice->CreateVertexBuffer( g_nMaxDynamicVertices * sizeof( SVertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL );
		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to create vertex buffer (%X)"), hr);
			return;
		}

		hr = m_pDevice->CreateVertexDeclaration( g_pDx9VertexElements, &m_pDeclaration );
		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to create vertex declaration (%X)"), hr);
			return;
		}	
	}

	CDynRender_dx9::~CDynRender_dx9()
	{
		SAFE_RELEASE( m_pVertexBuffer );
		SAFE_RELEASE( m_pDeclaration );
		SAFE_RELEASE( m_pDevice );
	}

	HRESULT	CDynRender_dx9::Draw()
	{
		HRESULT hr = S_OK;

		// Lock the vertex buffer to update it
		if( m_nVertexBase + m_nVertexCount > g_nMaxDynamicVertices )
			m_nVertexBase = 0;
		hr = m_pVertexBuffer->Lock(m_nVertexBase * sizeof(SVertex), m_nVertexCount * sizeof(SVertex), (void**)&m_pSysMemVertices, m_nVertexBase != 0 ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
		if( FAILED(hr) ) 
		{
			DEBUG_MESSAGE(_T("Failed to lock vertex buffer (%X)"), hr);
			return hr;
		}

		// Copy data
		memcpy(m_pSysMemVertices, &m_pSysMemVertexBuffer[0], m_nVertexCount * sizeof(SVertex));

		// Unlock
		hr = m_pVertexBuffer->Unlock();

		// Set streams
		hr = m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SVertex));

		// Set the vertex declaration that we're using
		hr = m_pDevice->SetVertexDeclaration(m_pDeclaration);

		// Draw the vertices
		if( m_eRenderType != RENDER_LINE_LIST )
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, m_nVertexBase, m_nVertexCount/3);
		else
			m_pDevice->DrawPrimitive(D3DPT_LINELIST, m_nVertexBase, m_nVertexCount/2);

		return hr;
	}

	bool CDynRender_dx9::IsValid() const
	{
		return ( m_pDevice && m_pVertexBuffer && m_pDeclaration ) ? true : false;
	}

	HRESULT CDynRender_dx9::SetVertexShaderConstantF ( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ )
	{
		_ASSERT( IsValid() );

		return m_pDevice->SetVertexShaderConstantF( nStartRegister_, pConstantData_ , nVector4fCount_ );
	}

	HRESULT CDynRender_dx9::CreateTextureFromFile( LPCTSTR pSrcFile_, CDynTexture& texture_ )
	{
		_ASSERT( IsValid() );

		HRESULT hRes = D3DXCreateTextureFromFileEx( m_pDevice, pSrcFile_,
			D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
			D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture_.m_pDx9Tex );

		return hRes;
	}

	HRESULT CDynRender_dx9::CreateTextureFromMemory( const void* pSrcFile_, UINT nDataSize_, CDynTexture& texture_ )
	{
		_ASSERT( IsValid() );

		HRESULT hRes = D3DXCreateTextureFromFileInMemoryEx( m_pDevice, pSrcFile_, nDataSize_, 
			D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
			D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture_.m_pDx9Tex );

		return hRes;
	}

	void CDynRender_dx9::DeleteTexture( CDynTexture& texture_ )
	{
		_ASSERT( texture_.m_pDx9Tex != NULL );
		SAFE_RELEASE( texture_.m_pDx9Tex );
	}

	HRESULT CDynRender_dx9::SetTexture( const CDynTexture& texture_ )
	{
		_ASSERT( IsValid() );

		_ASSERT( texture_.m_pDx9Tex != NULL );
		HRESULT hRes = m_pDevice->SetTexture( 0, texture_.m_pDx9Tex );
		return hRes;
	}

}
