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

#ifndef ACGFX_DYNRENDER_DX10_H
#define ACGFX_DYNRENDER_DX10_H

#include "acgfx_dynrender.h"

interface ID3D10Device;
interface ID3D10Buffer;
interface ID3D10InputLayout;
interface ID3D10Effect;
interface ID3D10EffectTechnique;
interface ID3D10BlendState;

namespace acGraphics
{	
	//////////////////////////////////////////////////////////////////////////

	class CDynRender_dx10:	public CDynRender
	{
	public:
		CDynRender_dx10	( ID3D10Device* pDevice_ );
		virtual ~CDynRender_dx10();

		virtual HRESULT						SetVertexShaderConstantF ( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ );
		virtual HRESULT						CreateTextureFromFile ( LPCTSTR pSrcFile_, CDynTexture& texture_ );
		virtual void						DeleteTexture ( CDynTexture& texture_ );
		virtual HRESULT						SetTexture ( const CDynTexture& texture_ );
		virtual HRESULT						Draw();
		virtual bool						IsValid() const;

	protected:
	private:
		interface ID3D10Device*				m_pDevice;
		interface ID3D10Buffer*				m_pVertexBuffer;
		interface ID3D10InputLayout*		m_pInputLayout;
		interface ID3D10Effect *			m_pFontEffect;
		interface ID3D10EffectTechnique*	m_pFontTechnique;
		interface ID3D10BlendState*			m_pBlendState;
	};

	//////////////////////////////////////////////////////////////////////////
}

#endif //  ACGFX_DYNRENDER_DX10_H
