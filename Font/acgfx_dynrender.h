/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
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

Andreas J�nsson
andreas@angelcode.com
*/

#pragma once

#ifndef ACGFX_DYNRENDER_H
#define ACGFX_DYNRENDER_H

interface IDirect3DTexture9;
interface ID3D10ShaderResourceView;

namespace acGraphics
{
	//////////////////////////////////////////////////////////////////////////

	enum eRenderTypes
	{
		RENDER_TRIANGLE_LIST,
		RENDER_QUAD_LIST,
		RENDER_LINE_LIST,
		RENDER_QUAD_CLIPPING,
	};	

	struct TD3D10UMShaderResourceView
	{
		void*	m_hResView;
		void*	m_hRes;
	};

	union CDynTexture
	{
		interface IDirect3DTexture9*		m_pDx9Tex;
		interface ID3D10ShaderResourceView* m_pDx10Tex;
		TD3D10UMShaderResourceView*			m_pDxTex;

		CDynTexture()
			: m_pDxTex(NULL)
		{	}
		CDynTexture(IDirect3DTexture9*	pDx9Tex)
			: m_pDx9Tex(pDx9Tex)
		{	}
		CDynTexture(ID3D10ShaderResourceView*	pDx10Tex)
			: m_pDx10Tex(pDx10Tex)
		{	}
	};

	const int								g_nMaxDynamicVertices	= 6 * 28; // 6 - 2 triangles;

	//////////////////////////////////////////////////////////////////////////

	class CDynRender
	{

	protected:

#pragma pack(push, 1)
		struct SVertex
		{
			D3DXVECTOR3	pos;
			DWORD		data;
			D3DXVECTOR2	tex;
		};
#pragma pack(pop)

	public:

		CDynRender ();
		virtual ~CDynRender	();

		void								Begin ( eRenderTypes type_ );
		void								End ();

		virtual HRESULT						SetVertexShaderConstantF ( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ ) = 0;
		virtual HRESULT						GetTextureSizeFromFile ( LPCTSTR pSrcFile_, SIZE& sSize );
		virtual HRESULT						GetTextureSizeFromMemory ( const void* pSrcFile_, UINT nDataSize_, SIZE& sSize );
		virtual HRESULT						CreateTextureFromFile ( LPCTSTR pSrcFile_, CDynTexture& texture_ ) = 0;
		virtual HRESULT						CreateTextureFromMemory ( const void* pSrcFile_, UINT nDataSize_, CDynTexture& texture_ ) = 0;
		virtual void						DeleteTexture ( CDynTexture& texture_ ) = 0;
		virtual HRESULT						SetTexture ( const CDynTexture& texture_ ) = 0;
		virtual HRESULT						Draw() = 0;
		virtual bool						IsValid() const = 0;

		void								VtxTexCoord	( float u, float v )
		{
			m_TexCoord.x = u;
			m_TexCoord.y = v;
		}
		void								VtxTexCoord	( D3DXVECTOR2 &tc )
		{
			m_TexCoord = tc;
		}
		void								VtxPos ( float x, float y, float z );
		void								VtxPos ( D3DXVECTOR3 &pos )
		{
			VtxPos(pos.x, pos.y, pos.z);
		}
		void								VtxData ( DWORD data )
		{
			m_nData = data;
		}

	protected:

		D3DXVECTOR2							m_TexCoord;
		DWORD								m_nData;
		eRenderTypes						m_eRenderType;

		UINT								m_nSubVertexCount;
		UINT								m_nVertexCount;		
		UINT								m_nVertexBase;

		SVertex*							m_pSysMemVertices;
		SVertex								m_pSysMemVertexBuffer[ g_nMaxDynamicVertices ];

	private:

	};	

	//////////////////////////////////////////////////////////////////////////
}

#endif //ACGFX_DYNRENDER_H
