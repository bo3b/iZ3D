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
Copyright (c) 2007 Andreas J�nsson

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
#include "acgfx_dynrender_dx10.h"

namespace acGraphics
{

	//////////////////////////////////////////////////////////////////////////	

	static const D3D10_INPUT_ELEMENT_DESC	g_pDx10VertexElements[]	=
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R8G8B8A8_UINT,	0, 12,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};



	//////////////////////////////////////////////////////////////////////////


	CDynRender_dx10::CDynRender_dx10( ID3D10Device* pDevice_ )
		:	m_pDevice		( pDevice_	)
		,	m_pVertexBuffer	( NULL		)
		,	m_pInputLayout	( NULL		)
		,	m_pFontEffect	( NULL		)
		,	m_pFontTechnique( NULL		)
		,	m_pBlendState	( NULL		)
	{
		_ASSERT( m_pDevice );
		m_pDevice->AddRef();

		D3D10_BUFFER_DESC buffDesc;
		buffDesc.ByteWidth			= g_nMaxDynamicVertices * sizeof( SVertex );		
		buffDesc.Usage				= D3D10_USAGE_DYNAMIC;
		buffDesc.BindFlags			= D3D10_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags		= D3D10_CPU_ACCESS_WRITE ;
		buffDesc.MiscFlags			= 0;		

		HRESULT hr = m_pDevice->CreateBuffer( &buffDesc, NULL, &m_pVertexBuffer );
		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to create vertex buffer (%X)"), hr);
			return;
		}		

		//hr = D3DX10CreateEffectFromFile( GetShaderPath( L"font.fx" ), NULL, NULL, "fx_4_0", 0, 0, m_pDevice, NULL, NULL, &m_pFontEffect, NULL, NULL );
		hr = D3DX10CreateEffectFromFile( L"font.fx", NULL, NULL, "fx_4_0", 0, 0, m_pDevice, NULL, NULL, &m_pFontEffect, NULL, NULL );
		if ( FAILED( hr ))
		{			
			DEBUG_MESSAGE(_T("Failed to create effect \"font.fx\" (%X)"), hr );
			return;
		}

		m_pFontTechnique = m_pFontEffect->GetTechniqueByName( "Font" );
		if ( !m_pFontTechnique )
		{
			DEBUG_MESSAGE(_T("Failed to create technique \"Font\"") );
			return;
		}

		D3D10_PASS_DESC desc;	

		hr = m_pFontTechnique->GetPassByIndex( 0 )->GetDesc( &desc );		
		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed GetDesc  for effect (%X)"), hr);
			return;
		}

		UINT numElements = sizeof( g_pDx10VertexElements ) / sizeof( g_pDx10VertexElements[0] );
		hr = m_pDevice->CreateInputLayout( g_pDx10VertexElements, numElements, desc.pIAInputSignature, desc.IAInputSignatureSize, &m_pInputLayout );

		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to create vertex declaration (%X)"), hr);
			return;
		}

		// Create a blend state to disable alpha blending
		D3D10_BLEND_DESC BlendState;
		ZeroMemory( &BlendState, sizeof( D3D10_BLEND_DESC ) );
		BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		BlendState.BlendEnable[0] = TRUE;
		BlendState.BlendEnable[1] = TRUE;
		BlendState.BlendEnable[2] = TRUE;
		BlendState.BlendEnable[3] = TRUE;
		BlendState.BlendEnable[4] = TRUE;		
		BlendState.AlphaToCoverageEnable = TRUE;
		BlendState.DestBlend	= D3D10_BLEND_INV_SRC_ALPHA;
		BlendState.SrcBlend		= D3D10_BLEND_SRC_ALPHA;
		BlendState.BlendOp		= D3D10_BLEND_OP_ADD;

		hr = m_pDevice->CreateBlendState( &BlendState, &m_pBlendState );
		if( FAILED(hr) )
		{
			DEBUG_MESSAGE(_T("Failed to create blend state (%X)"), hr);
			return;
		}
	}

	CDynRender_dx10::~CDynRender_dx10()
	{
		if ( m_pBlendState )
		{
			m_pBlendState->Release();
			m_pBlendState = NULL;
		}

		if ( m_pFontEffect )
		{
			m_pFontEffect->Release();
			m_pFontEffect = NULL;
		}

		if ( m_pInputLayout )
		{
			m_pInputLayout->Release();
			m_pInputLayout = NULL;
		}

		if ( m_pVertexBuffer )
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = NULL;
		}

		if( m_pDevice )
		{
			m_pDevice->Release();
			m_pDevice = 0;
		}
	}

	HRESULT CDynRender_dx10::Draw()
	{
		HRESULT hr = S_OK;

		// Lock the vertex buffer to update it
		if( m_nVertexBase + m_nVertexCount > g_nMaxDynamicVertices )
			m_nVertexBase = 0;

		void* pData = NULL;
		hr = m_pVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, ( void** )&pData );
		if( FAILED( hr ) || !pData )
		{
			DEBUG_MESSAGE(_T("Failed to lock vertex buffer (%X)"), hr );
			return hr;
		}

		memcpy( pData, &m_pSysMemVertexBuffer[ 0 ], m_nVertexCount * sizeof( SVertex ) );			

		m_pVertexBuffer->Unmap();

		UINT nStride = sizeof( SVertex );
		UINT nOffset = 0;
		m_pDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &nStride, &nOffset );
		m_pDevice->IASetInputLayout( m_pInputLayout );			

		m_pFontTechnique->GetPassByIndex( 0 )->Apply( 0 );

		//			float BlendFactor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//			m_pDevice->OMSetBlendState( m_pBlendState, BlendFactor, 0xffffffff );

		// Draw the vertices
		if( m_eRenderType != RENDER_LINE_LIST )
			m_pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		else
			m_pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST );

		m_pDevice->Draw( m_nVertexCount, 0 );

		return hr;
	}

	HRESULT	CDynRender_dx10::SetVertexShaderConstantF( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ )
	{
		_ASSERT( IsValid() );

		ID3D10EffectMatrixVariable* pCB = m_pFontEffect->GetVariableByName( "tmView" )->AsMatrix();
		_ASSERT( pCB );
		HRESULT hr = pCB->SetMatrix( ( float* )pConstantData_ );		

		_ASSERT( hr == S_OK );
		return E_FAIL;		
	}

	HRESULT CDynRender_dx10::CreateTextureFromFile( LPCTSTR pSrcFile_, CDynTexture& texture_ )
	{
		_ASSERT( IsValid() );

		HRESULT hRes = D3DX10CreateShaderResourceViewFromFile( m_pDevice, pSrcFile_, NULL, NULL, &texture_.m_pDx10Tex, NULL );
		return hRes;
	}

	void CDynRender_dx10::DeleteTexture( CDynTexture& texture_ )
	{
		_ASSERT( texture_.m_pDx10Tex != NULL );
		texture_.m_pDx10Tex->Release();
		texture_.m_pDx10Tex = NULL;	
	}

	HRESULT CDynRender_dx10::SetTexture( const CDynTexture& texture_ )
	{
		_ASSERT( IsValid() );

		_ASSERT( texture_.m_pDx10Tex != NULL );
		//m_pDevice->PSSetShaderResources( 0, 0, &page_.m_pDx10Tex );
		m_pFontEffect->GetVariableByName( "txDiffuse" )->AsShaderResource()->SetResource( texture_.m_pDx10Tex );

		return S_OK;
	}

	bool CDynRender_dx10::IsValid() const
	{
		return ( m_pDevice && m_pVertexBuffer && m_pInputLayout && m_pFontEffect ) ? true : false;
	}

	//////////////////////////////////////////////////////////////////////////	

}
