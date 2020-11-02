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
#include "acgfx_dynrender.h"

#include <IL/il.h>
#include <IL/ilu.h>

namespace acGraphics
{
	CDynRender::CDynRender()
		:	m_pSysMemVertices	( NULL	)
		,	m_nSubVertexCount	( 0		)
		,	m_nVertexCount		( 0		)
		,	m_nVertexBase		( 0		)
	{
		memset( &m_pSysMemVertexBuffer, 0, sizeof( SVertex ) * g_nMaxDynamicVertices );
	}

	CDynRender::~CDynRender()
	{
	}

	void CDynRender::Begin( eRenderTypes type_ )
	{
		_ASSERT( IsValid() );
		m_eRenderType		= type_;
		m_nVertexCount		= 0;
		m_nSubVertexCount	= 0;
		m_pSysMemVertices	= ( SVertex* )&m_pSysMemVertexBuffer[0];
	}

	void CDynRender::End()
	{
		HRESULT hr = S_OK;

		_ASSERT( IsValid() );

		if( m_nVertexCount > 0 )
		{
			Draw();

			m_nVertexBase += m_nVertexCount;
		}		

		m_pSysMemVertices = 0;

		return;
	}

	void CDynRender::VtxPos(float x, float y, float z)
	{
		_ASSERT( m_pSysMemVertices );

		if( m_nVertexCount >= g_nMaxDynamicVertices )
		{
			// If too many vertices are used, render the current ones and then start over
			End();
			Begin( m_eRenderType );
		}

		// Increase the number of vertices used
		m_nVertexCount++;

		m_pSysMemVertices->pos.x = x;
		m_pSysMemVertices->pos.y = y;
		m_pSysMemVertices->pos.z = z;

		m_pSysMemVertices->tex = m_TexCoord;
		m_pSysMemVertices->data = m_nData;

		m_pSysMemVertices++;

		if( m_eRenderType == RENDER_QUAD_LIST )
		{
			m_nSubVertexCount = (++m_nSubVertexCount) % 4;
			if( m_nSubVertexCount == 0 )
			{
				// Duplicate vertex 0 in the quad
				memcpy(m_pSysMemVertices, m_pSysMemVertices - 4, sizeof( SVertex ));
				m_pSysMemVertices++;

				// Duplicate vertex 2 in the quad
				memcpy(m_pSysMemVertices, m_pSysMemVertices - 3, sizeof( SVertex ));
				m_pSysMemVertices++;

				m_nVertexCount += 2;
			}
		}
	}

	HRESULT	CDynRender::GetTextureSizeFromFile ( LPCTSTR pSrcFile_, SIZE& sSize )
	{
		ILuint nImageId = ilGenImage();
		ilBindImage( nImageId );
		ILboolean bLoadImg = ilLoadImage( pSrcFile_ );

		if ( !bLoadImg )
		{
			DEBUG_MESSAGE(_T("unsupported font file format: %s"), pSrcFile_ );
			return E_FAIL;
		}

		sSize.cx = ilGetInteger ( IL_IMAGE_WIDTH  );
		sSize.cy = ilGetInteger ( IL_IMAGE_HEIGHT );
		ilDeleteImage( nImageId );
		
		return S_OK;
	}

	HRESULT	CDynRender::GetTextureSizeFromMemory ( const void* pSrcFile_, UINT nDataSize_, SIZE& sSize )
	{
		ILuint nImageId = ilGenImage();
		ilBindImage( nImageId );
		ILboolean bLoadImg = ilLoadL(IL_TYPE_UNKNOWN, pSrcFile_, (ILuint)nDataSize_);

		if ( !bLoadImg )
		{
			DEBUG_MESSAGE(_T("unsupported font file format: %s"), pSrcFile_ );
			return E_FAIL;
		}

		sSize.cx = ilGetInteger ( IL_IMAGE_WIDTH  );
		sSize.cy = ilGetInteger ( IL_IMAGE_HEIGHT );
		ilDeleteImage( nImageId );

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////

} // end namespace
