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

#ifndef ACGFX_DYNRENDER_DX10UM_H
#define ACGFX_DYNRENDER_DX10UM_H

#include "../Font/acgfx_dynrender.h"

namespace acGraphics
{
	//////////////////////////////////////////////////////////////////////////

	class CDynRender_dx10UM: public CDynRender
	{
	public:
		CDynRender_dx10UM	( D3DDeviceWrapper* pDevice_ );
		virtual ~CDynRender_dx10UM	();

		virtual HRESULT						SetVertexShaderConstantF ( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ );		
		virtual HRESULT						CreateTextureFromFile ( LPCTSTR pSrcFile_, CDynTexture& texture_ );
		virtual HRESULT						CreateTextureFromMemory ( const void* pSrcFile_, UINT nDataSize_, CDynTexture& texture_ );
		virtual void						DeleteTexture ( CDynTexture& texture_ );
		virtual HRESULT						SetTexture ( const CDynTexture& texture_ );
		virtual HRESULT						Draw();
		virtual bool						IsValid() const;
		void								CreateTexture( UINT nTexWidth, UINT nTexHeight, SIZE_T nRowBytes, BYTE* pTexData, CDynTexture& texture_ );

	protected:
		void								CreateBlendStates	();
		void								CreateBlendState	(D3D10DDI_HBLENDSTATE& hBlendState, BOOL bEnabled);
		void								CreateVertexBuffer	();
		void								CreateElementLayout	();
		void								CreateShaders		();
		void								CreateConstantBuffer();
		void								CreateSamplerState	();
		void								CreateRasterizerState ();
	private:
		D3DDeviceWrapper*					m_pDevice;
		CDynTexture							m_tTexture;

		D3D10DDI_HRASTERIZERSTATE			m_hRastState;
		D3D10DDI_HRESOURCE					m_hVertexBuffer;
		D3D10DDI_HELEMENTLAYOUT				m_hElementLayout;
		D3D10DDI_HSHADER					m_hVSFontShader;
		D3D10DDI_HSHADER					m_hPSFontShader;
		D3D10DDI_HSHADER					m_hGSNullShader;
		D3D10DDI_HRESOURCE					m_hCBView;

		D3D10DDI_HBLENDSTATE				m_hBlendStateEnable;
		D3D10DDI_HBLENDSTATE				m_hBlendStateDisable;
		D3D10DDI_HSAMPLER					m_hSamplerState;		

		D3D10DDI_HSHADER					m_hPSBlackShader;
	};

	//////////////////////////////////////////////////////////////////////////
}

#endif // ACGFX_DYNRENDER_DX10UM_H
