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

#include "stdafx.h"
#include "acgfx_dynrender_dx10um.h"
#include "D3DDeviceWrapper.h"

#include "../Font/Shaders/PSBlack_dx10.h"
#include "../Font/Shaders/PSFont_dx10.h"
#include "../Font/Shaders/VSFont_dx10.h"

#include "IL/il.h"
#include "IL/ilu.h"

namespace acGraphics
{
	//////////////////////////////////////////////////////////////////////////

	CONST DWORD* FindDword(CONST DWORD* pData, DWORD dataSize, DWORD XXX)
	{
		for(UINT i = 0; i < dataSize / sizeof(DWORD); i++)
			if(pData[i] == XXX)
				return &pData[i];

		return 0;
	}

	bool ParseShaderBlob ( CONST DWORD* pData, DWORD dataSize, CONST UINT**  ppCode)
	{
		const DWORD DXBC = MAKEFOURCC('D', 'X', 'B', 'C');
		const DWORD RDEF = MAKEFOURCC('R', 'D', 'E', 'F');
		const DWORD ISGN = MAKEFOURCC('I', 'S', 'G', 'N');
		const DWORD OSGN = MAKEFOURCC('O', 'S', 'G', 'N');
		const DWORD SHDR = MAKEFOURCC('S', 'H', 'D', 'R');
		const DWORD STAT = MAKEFOURCC('S', 'T', 'A', 'T');
		*ppCode = NULL;
		//ppSignatures->pInputSignature = NULL;
		//ppSignatures->pOutputSignature = NULL;
		if (pData[0] != DXBC)
			return false;

		*ppCode = (UINT*)FindDword(pData, dataSize, SHDR);
		if(*ppCode)
		{
			*ppCode += 2;
			return true;
			/*
			CONST DWORD* p = FindDword(pData, dataSize, ISGN);
			ppSignatures->pInputSignature = (D3D10DDIARG_SIGNATURE_ENTRY*)&p[3];
			ppSignatures->NumInputSignatureEntries = (UINT)p[2];
			if(ppSignatures->pInputSignature)
			{
			p = FindDword(pData, dataSize, OSGN);
			ppSignatures->pOutputSignature = (D3D10DDIARG_SIGNATURE_ENTRY*)&p[3];
			ppSignatures->NumOutputSignatureEntries = (UINT)p[2];
			if(ppSignatures->pOutputSignature)
			return true;
			}
			*/
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	static const D3D10DDIARG_INPUT_ELEMENT_DESC	g_pDx10UMVertexElements[]	=
	{
		{ 0, 0,		DXGI_FORMAT_R32G32B32_FLOAT,	D3D10_DDI_INPUT_PER_VERTEX_DATA, 0, 0 },
		{ 0, 12,	DXGI_FORMAT_R8G8B8A8_UINT,		D3D10_DDI_INPUT_PER_VERTEX_DATA, 0, 1 },
		{ 0, 16,	DXGI_FORMAT_R32G32_FLOAT,		D3D10_DDI_INPUT_PER_VERTEX_DATA, 0, 2 },
	};

	//////////////////////////////////////////////////////////////////////////

	CDynRender_dx10UM::CDynRender_dx10UM( D3DDeviceWrapper* pDevice_ )
	:	m_pDevice( pDevice_ )
	{
		m_tTexture.m_pDxTex		= NULL;
		CreateVertexBuffer();
		CreateElementLayout();
		CreateShaders();
		CreateConstantBuffer();
		CreateBlendStates();
		CreateSamplerState();
		CreateRasterizerState();
	}

	CDynRender_dx10UM::~CDynRender_dx10UM()
	{
		D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();

		m_pDevice->OriginalDeviceFuncs.pfnDestroySampler( hDevice, m_hSamplerState );
		DELETE_HANDLE( m_hSamplerState );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyResource( hDevice, m_hVertexBuffer );
		DELETE_HANDLE( m_hVertexBuffer );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyElementLayout( hDevice, m_hElementLayout );
		DELETE_HANDLE( m_hElementLayout );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyShader( hDevice, m_hVSFontShader );
		DELETE_HANDLE( m_hVSFontShader );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyShader( hDevice, m_hPSFontShader );
		DELETE_HANDLE( m_hPSFontShader );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyShader( hDevice, m_hPSBlackShader );
		DELETE_HANDLE( m_hPSBlackShader );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyResource( hDevice, m_hCBView );
		DELETE_HANDLE( m_hCBView );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyBlendState( hDevice, m_hBlendStateEnable );
		DELETE_HANDLE( m_hBlendStateEnable );
 
		m_pDevice->OriginalDeviceFuncs.pfnDestroyBlendState( hDevice, m_hBlendStateDisable );
		DELETE_HANDLE( m_hBlendStateDisable );
 
		m_pDevice->OriginalDeviceFuncs.pfnDestroyRasterizerState( hDevice, m_hRastState );
		DELETE_HANDLE( m_hRastState );
		m_pDevice = 0;
	}

	void CDynRender_dx10UM::CreateSamplerState()
	{
		D3D10DDI_HDEVICE hDevice	= m_pDevice->GetHandle();

		m_hSamplerState.pDrvPrivate				= NULL;
		D3D10DDI_HRTSAMPLER		hRTSamplerDesc	= { NULL };

		D3D10_DDI_SAMPLER_DESC	SamplerDesc;
		ZeroMemory( &SamplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ) );	

		SamplerDesc.Filter						=   D3D10_DDI_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDesc.AddressU					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressV					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.AddressW					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
		SamplerDesc.MipLODBias					=	0.0f;
		SamplerDesc.MaxAnisotropy				=	1;
		SamplerDesc.ComparisonFunc				=	D3D10_DDI_COMPARISON_NEVER;
		SamplerDesc.BorderColor[0]				=	0.0f;
		SamplerDesc.BorderColor[1]				=	0.0f;
		SamplerDesc.BorderColor[2]				=	0.0f;
		SamplerDesc.BorderColor[3]				=	0.0f;
		SamplerDesc.MinLOD						=	0.0f;
		SamplerDesc.MaxLOD						=	FLT_MAX;

		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateSamplerSize( hDevice, &SamplerDesc );
		NEW_HANDLE( m_hSamplerState, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreateSampler( hDevice, &SamplerDesc, m_hSamplerState, hRTSamplerDesc );
	}

	void CDynRender_dx10UM::CreateConstantBuffer()
	{
		D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();

		D3D11DDIARG_CREATERESOURCE CreateCBRes;
		memset( &CreateCBRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

		D3D10DDI_MIPINFO CBMipInfo;
		CBMipInfo.TexelWidth				= 64;
		CBMipInfo.TexelHeight				= 1;
		CBMipInfo.TexelDepth				= 1;
		CBMipInfo.PhysicalWidth				= 64;
		CBMipInfo.PhysicalHeight			= 1;
		CBMipInfo.PhysicalDepth				= 1;

		CreateCBRes.pMipInfoList			= &CBMipInfo;
		CreateCBRes.ResourceDimension		= D3D10DDIRESOURCE_BUFFER;
		CreateCBRes.Usage					= D3D10_DDI_USAGE_DEFAULT;
		CreateCBRes.BindFlags				= D3D10_DDI_BIND_CONSTANT_BUFFER;
		CreateCBRes.Format					= DXGI_FORMAT_UNKNOWN;
		CreateCBRes.SampleDesc.Count		= 1;
		CreateCBRes.SampleDesc.Quality		= 0;
		CreateCBRes.MipLevels				= 1;
		CreateCBRes.ArraySize				= 1;

		m_hCBView.pDrvPrivate				= NULL;

		static char RTData[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
		D3D10DDI_HRTRESOURCE hRTCBView		= { RTData };

		m_pDevice->CreateResource(m_hCBView, &CreateCBRes, hRTCBView);
	}

	void CDynRender_dx10UM::CreateBlendState(D3D10DDI_HBLENDSTATE& hBlendState, BOOL bEnabled)
	{
		D3D10DDI_HDEVICE hDevice	= m_pDevice->GetHandle();
		SIZE_T nSize				= 0;

		if	( m_pDevice->GetD3DVersion() == TD3DVersion_10_0 )
		{
			// Blend State Dx 10 //
			D3D10_DDI_BLEND_DESC		BlendDesc;
			memset( &BlendDesc, 0, sizeof( D3D10_DDI_BLEND_DESC ) );
			D3D10DDI_HRTBLENDSTATE		hRTBlendState		= { NULL };
			BlendDesc.AlphaToCoverageEnable					= FALSE;
			BlendDesc.SrcBlend								= D3D10_DDI_BLEND_SRC_ALPHA; // D3D10_DDI_BLEND_ONE
			BlendDesc.DestBlend								= D3D10_DDI_BLEND_INV_SRC_ALPHA; // D3D10_DDI_BLEND_ZERO
			BlendDesc.BlendOp								= D3D10_DDI_BLEND_OP_ADD;
			BlendDesc.SrcBlendAlpha							= D3D10_DDI_BLEND_ONE;
			BlendDesc.DestBlendAlpha						= D3D10_DDI_BLEND_ZERO;
			BlendDesc.BlendOpAlpha							= D3D10_DDI_BLEND_OP_ADD;
			for ( int i = 0; i < D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
			{
				BlendDesc.BlendEnable[ i ]					= bEnabled;
				BlendDesc.RenderTargetWriteMask[i]			= D3D10_DDI_COLOR_WRITE_ENABLE_ALL;
			}

			nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateBlendStateSize( hDevice, &BlendDesc );	
			NEW_HANDLE( hBlendState, nSize );
			m_pDevice->OriginalDeviceFuncs.pfnCreateBlendState( hDevice, &BlendDesc, hBlendState, hRTBlendState );
		}
		else
		{
			// Blend State Dx 10.1 //
			D3D10_1_DDI_BLEND_DESC		BlendDesc;
			memset( &BlendDesc, 0, sizeof( D3D10_1_DDI_BLEND_DESC ) );
			D3D10DDI_HRTBLENDSTATE		hRTBlendState		= { NULL };
			BlendDesc.AlphaToCoverageEnable					= FALSE;
			BlendDesc.IndependentBlendEnable				= FALSE;

			for ( int i = 0; i < D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
			{
				BlendDesc.RenderTarget[i].BlendEnable			= bEnabled;
				BlendDesc.RenderTarget[i].SrcBlend				= D3D10_DDI_BLEND_SRC_ALPHA; // D3D10_DDI_BLEND_ONE
				BlendDesc.RenderTarget[i].DestBlend				= D3D10_DDI_BLEND_INV_SRC_ALPHA; // D3D10_DDI_BLEND_ZERO
				BlendDesc.RenderTarget[i].BlendOp				= D3D10_DDI_BLEND_OP_ADD;
				BlendDesc.RenderTarget[i].SrcBlendAlpha			= D3D10_DDI_BLEND_ONE;
				BlendDesc.RenderTarget[i].DestBlendAlpha		= D3D10_DDI_BLEND_ZERO;
				BlendDesc.RenderTarget[i].BlendOpAlpha			= D3D10_DDI_BLEND_OP_ADD;
				BlendDesc.RenderTarget[i].RenderTargetWriteMask	= D3D10_DDI_COLOR_WRITE_ENABLE_ALL;	
			}

			nSize = m_pDevice->OriginalDeviceFuncs10_1.pfnCalcPrivateBlendStateSize( hDevice, &BlendDesc );	
			NEW_HANDLE( hBlendState, nSize );
			m_pDevice->OriginalDeviceFuncs10_1.pfnCreateBlendState( hDevice, &BlendDesc, hBlendState, hRTBlendState );
		}
	}

	void CDynRender_dx10UM::CreateBlendStates()
	{
		CreateBlendState(m_hBlendStateEnable, true);
		CreateBlendState(m_hBlendStateDisable, false);
	}

	void CDynRender_dx10UM::CreateVertexBuffer()
	{
		D3D10DDI_HDEVICE hDevice		= m_pDevice->GetHandle();

		D3D10DDI_MIPINFO VBMipInfo;
		VBMipInfo.TexelWidth			= g_nMaxDynamicVertices * sizeof( SVertex );
		VBMipInfo.TexelHeight			= 1;
		VBMipInfo.TexelDepth			= 1;
		VBMipInfo.PhysicalWidth			= g_nMaxDynamicVertices * sizeof( SVertex );
		VBMipInfo.PhysicalHeight		= 1;
		VBMipInfo.PhysicalDepth			= 1;

		D3D11DDIARG_CREATERESOURCE CreateVBRes;
		memset( &CreateVBRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );
		CreateVBRes.pMipInfoList		= &VBMipInfo;
		CreateVBRes.ResourceDimension	= D3D10DDIRESOURCE_BUFFER;
		CreateVBRes.Usage				= D3D10_DDI_USAGE_DYNAMIC;
		CreateVBRes.BindFlags			= D3D10_DDI_BIND_VERTEX_BUFFER;
		CreateVBRes.MapFlags			= D3D10_DDI_MAP_READ;
		CreateVBRes.Format				= DXGI_FORMAT_UNKNOWN;
		CreateVBRes.SampleDesc.Count	= 1;
		CreateVBRes.SampleDesc.Quality	= 0;
		CreateVBRes.MipLevels			= 1;
		CreateVBRes.ArraySize			= 1;

		m_hVertexBuffer.pDrvPrivate		= NULL;
		D3D10DDI_HRTRESOURCE hRTBuffer	= { NULL };		

		m_pDevice->CreateResource(m_hVertexBuffer, &CreateVBRes, hRTBuffer);
	}

	void CDynRender_dx10UM::CreateElementLayout()
	{
		D3D10DDI_HDEVICE hDevice		= m_pDevice->GetHandle();

		D3D10DDIARG_CREATEELEMENTLAYOUT	CreateLayout;
		CreateLayout.pVertexElements	= g_pDx10UMVertexElements;
		CreateLayout.NumElements		= sizeof( g_pDx10UMVertexElements ) / sizeof( g_pDx10UMVertexElements[0] );

		m_hElementLayout.pDrvPrivate	= NULL;
		D3D10DDI_HRTELEMENTLAYOUT  hRTElementLayout = { NULL } ;

		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateElementLayoutSize( hDevice, &CreateLayout );
		NEW_HANDLE( m_hElementLayout, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreateElementLayout( hDevice, &CreateLayout, m_hElementLayout, hRTElementLayout );
	}

	void CDynRender_dx10UM::CreateShaders()
	{
		D3D10DDI_HDEVICE hDevice		= m_pDevice->GetHandle();

		/************************************************************************/
		// Create VS Shader VSFont_dx10

		CONST UINT* pVSFontCode = NULL;
		bool bParse = ParseShaderBlob( (DWORD*)g_VSFont, sizeof( g_VSFont ), &pVSFontCode );
		_ASSERT( bParse );

		D3D10DDIARG_SIGNATURE_ENTRY		VSSigEntryInput[ 3 ];
		VSSigEntryInput[ 0 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		VSSigEntryInput[ 0 ].Register		=	0;
		VSSigEntryInput[ 0 ].Mask			=	7;
		VSSigEntryInput[ 1 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		VSSigEntryInput[ 1 ].Register		=	1;
		VSSigEntryInput[ 1 ].Mask			=	15;
		VSSigEntryInput[ 2 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		VSSigEntryInput[ 2 ].Register		=	2;
		VSSigEntryInput[ 2 ].Mask			=	3;

		D3D10DDIARG_SIGNATURE_ENTRY		VSSigEntryOutput[ 2 ];
		VSSigEntryOutput[ 0 ].SystemValue	=	D3D10_SB_NAME_POSITION;
		VSSigEntryOutput[ 0 ].Register		=	0;
		VSSigEntryOutput[ 0 ].Mask			=	15;
		VSSigEntryOutput[ 1 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		VSSigEntryOutput[ 1 ].Register		=	1;
		VSSigEntryOutput[ 1 ].Mask			=	3;


		D3D10DDIARG_STAGE_IO_SIGNATURES VSSig;
		VSSig.NumInputSignatureEntries	= sizeof( VSSigEntryInput ) / sizeof( VSSigEntryInput[ 0 ] );
		VSSig.pInputSignature			= VSSigEntryInput;
		VSSig.NumOutputSignatureEntries	= sizeof( VSSigEntryOutput ) / sizeof( VSSigEntryOutput[ 0 ] );
		VSSig.pOutputSignature			= VSSigEntryOutput;

		m_hVSFontShader.pDrvPrivate			= NULL;
		D3D10DDI_HRTSHADER hVSRTShader	= { NULL };

		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateShaderSize( hDevice, pVSFontCode, &VSSig );
		NEW_HANDLE( m_hVSFontShader, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreateVertexShader( hDevice, pVSFontCode, m_hVSFontShader, hVSRTShader, &VSSig );

		/************************************************************************/
		// Create PS Shader PSFont_dx10

		CONST UINT*  pPSFontCode = NULL;
		bParse = ParseShaderBlob( (DWORD*)g_PSFont, sizeof( g_PSFont ), &pPSFontCode );
		_ASSERT( bParse );

		D3D10DDIARG_SIGNATURE_ENTRY		PSSigEntryInput[ 2 ];
		PSSigEntryInput[ 0 ].SystemValue	=	D3D10_SB_NAME_POSITION;
		PSSigEntryInput[ 0 ].Register		=	0;
		PSSigEntryInput[ 0 ].Mask			=	15;
		PSSigEntryInput[ 1 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		PSSigEntryInput[ 1 ].Register		=	1;
		PSSigEntryInput[ 1 ].Mask			=	3;

		D3D10DDIARG_SIGNATURE_ENTRY		PSSigEntryOutput[ 1 ];
		PSSigEntryOutput[ 0 ].SystemValue	=	D3D10_SB_NAME_UNDEFINED;
		PSSigEntryOutput[ 0 ].Register		=	0;
		PSSigEntryOutput[ 0 ].Mask			=	15;

		D3D10DDIARG_STAGE_IO_SIGNATURES PSSig;
		PSSig.NumInputSignatureEntries	= sizeof( PSSigEntryInput ) / sizeof( PSSigEntryInput[ 0 ] );
		PSSig.pInputSignature			= PSSigEntryInput;
		PSSig.NumOutputSignatureEntries	= sizeof( PSSigEntryOutput ) / sizeof( PSSigEntryOutput[ 0 ] );
		PSSig.pOutputSignature			= PSSigEntryOutput;

		m_hPSFontShader.pDrvPrivate			= NULL;
		D3D10DDI_HRTSHADER hPSRTShader	= { NULL };

		nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateShaderSize( hDevice, pPSFontCode, &PSSig );
		NEW_HANDLE( m_hPSFontShader, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreatePixelShader( hDevice, pPSFontCode, m_hPSFontShader, hPSRTShader, &PSSig );

		/************************************************************************/
		// Create PS Shader PSBlack_dx10

		CONST UINT*  pPSBlackCode = NULL;
		bParse = ParseShaderBlob( (DWORD*)g_PSBlack, sizeof( g_PSBlack ), &pPSBlackCode );
		_ASSERT( bParse );		

		m_hPSBlackShader.pDrvPrivate		= NULL;
		D3D10DDI_HRTSHADER hPSBlackRTShader	= { NULL };

		nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateShaderSize( hDevice, pPSBlackCode, &PSSig );
		NEW_HANDLE( m_hPSBlackShader, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreatePixelShader( hDevice, pPSBlackCode, m_hPSBlackShader, hPSBlackRTShader, &PSSig );		

		/************************************************************************/
		// Create GS Shader

		m_hGSNullShader.pDrvPrivate			= NULL;

		/************************************************************************/
	}

	void CDynRender_dx10UM::CreateRasterizerState()
	{
		D3D10DDI_HDEVICE hDevice		= m_pDevice->GetHandle();

		D3D10_DDI_RASTERIZER_DESC		RastDesc;
		D3D10DDI_HRTRASTERIZERSTATE		hRTRasterState = { NULL };
		ZeroMemory( &RastDesc, sizeof( D3D10_DDI_RASTERIZER_DESC ) );
		RastDesc.FillMode						= D3D10_DDI_FILL_SOLID;
		RastDesc.CullMode						= D3D10_DDI_CULL_NONE;
		RastDesc.FrontCounterClockwise			= TRUE;
		RastDesc.DepthBias						= 0;
		RastDesc.DepthBiasClamp					= 0.0f;
		RastDesc.SlopeScaledDepthBias			= 0.0f;
		RastDesc.DepthClipEnable				= FALSE;
		RastDesc.ScissorEnable					= FALSE;
		RastDesc.MultisampleEnable				= FALSE;
		RastDesc.AntialiasedLineEnable			= FALSE;

		SIZE_T nSize = m_pDevice->OriginalDeviceFuncs.pfnCalcPrivateRasterizerStateSize( hDevice, &RastDesc );
		NEW_HANDLE( m_hRastState, nSize );
		m_pDevice->OriginalDeviceFuncs.pfnCreateRasterizerState( hDevice, &RastDesc, m_hRastState, hRTRasterState );
	}

	HRESULT	CDynRender_dx10UM::SetVertexShaderConstantF ( UINT nStartRegister_, CONST float* pConstantData_, UINT nVector4fCount_ )
	{
		if ( !IsValid() )
			return E_FAIL;

		D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();
		UINT size = 4 * nVector4fCount_ * sizeof(FLOAT);
		_ASSERT(size == 64);
		m_pDevice->OriginalDeviceFuncs.pfnDefaultConstantBufferUpdateSubresourceUP( hDevice, m_hCBView, 0, NULL, ( const void* )pConstantData_, size, size );		

		return S_OK;
	}

	HRESULT	CDynRender_dx10UM::CreateTextureFromFile ( LPCTSTR pSrcFile_, CDynTexture& texture_ )
	{
		if ( !IsValid() )
			return E_FAIL;

		/************************************************************************/

		UINT nTexWidth				= 0;
		UINT nTexHeight				= 0;
		int  nBitDepth				= 0;
		SIZE_T nRowBytes			= 0;
		BYTE* pTexData				= NULL;

		/************************************************************************/
		// read data from png file

		ILuint nImageId = ilGenImage();
		ilBindImage( nImageId );
		ILboolean bLoadImg = ilLoadImage( pSrcFile_ );

		if ( !bLoadImg )
		{
			DEBUG_MESSAGE(_T("unsupported font file format: %\n"), pSrcFile_ );
			return E_FAIL;
		}
		
		if(ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA || ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) != 32)
		{
			ILboolean bConvert = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			if ( !bConvert )
			{
				ilDeleteImage( nImageId );
				DEBUG_MESSAGE(_T("can't conver image to RGBA format: %s\n"), pSrcFile_ );
				return E_FAIL;
			}
		}

		nTexWidth  = ilGetInteger ( IL_IMAGE_WIDTH  );
		nTexHeight = ilGetInteger ( IL_IMAGE_HEIGHT );
		nBitDepth  = ilGetInteger ( IL_IMAGE_BITS_PER_PIXEL  );
		nRowBytes  = nTexWidth * nBitDepth / 8;
		pTexData   = ilGetData ();

		CreateTexture( nTexWidth, nTexHeight, nRowBytes, pTexData, texture_ );

		ilDeleteImage( nImageId );
		/************************************************************************/	
		
		return S_OK;
	}
	
	HRESULT CDynRender_dx10UM::CreateTextureFromMemory ( const void* pSrcFile_, UINT nDataSize_, CDynTexture& texture_ )
	{
		if ( !IsValid() )
			return E_FAIL;
		/************************************************************************/

		UINT nTexWidth				= 0;
		UINT nTexHeight				= 0;
		int  nBitDepth				= 0;
		SIZE_T nRowBytes			= 0;
		BYTE* pTexData				= NULL;

		/************************************************************************/
		// read data from memory
	
		ILuint nImageId = ilGenImage();
		ilBindImage( nImageId );
		ILboolean bLoadImg = ilLoadL(IL_TYPE_UNKNOWN, pSrcFile_, (ILuint)nDataSize_);

		if ( !bLoadImg )
		{
			DEBUG_MESSAGE(_T("unsupported font file format: %s\n"), pSrcFile_ );
			return E_FAIL;
		}

		if(ilGetInteger(IL_IMAGE_FORMAT) != IL_RGBA || ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) != 32)
		{
			ILboolean bConvert = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			if ( !bConvert )
			{
				ilDeleteImage( nImageId );
				DEBUG_MESSAGE(_T("can't conver image to RGBA format: %s\n"), pSrcFile_ );
				return E_FAIL;
			}
		}

		nTexWidth  = ilGetInteger ( IL_IMAGE_WIDTH  );
		nTexHeight = ilGetInteger ( IL_IMAGE_HEIGHT );
		nBitDepth  = ilGetInteger ( IL_IMAGE_BITS_PER_PIXEL  );
		nRowBytes  = nTexWidth * nBitDepth / 8;
		pTexData   = ilGetData();

		CreateTexture( nTexWidth, nTexHeight, nRowBytes, pTexData, texture_ );

		ilDeleteImage( nImageId );
		/************************************************************************/	

		return S_OK;
	}

	void CDynRender_dx10UM::CreateTexture( UINT nTexWidth, UINT nTexHeight, SIZE_T nRowBytes, BYTE* pTexData, CDynTexture& texture_ )
	{
		D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();

		D3D11DDIARG_CREATERESOURCE CreateTexRes;
		memset( &CreateTexRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

		D3D10DDI_MIPINFO TexMipInfo;
		TexMipInfo.TexelWidth				= nTexWidth;
		TexMipInfo.TexelHeight				= nTexHeight;
		TexMipInfo.TexelDepth				= 1;
		TexMipInfo.PhysicalWidth			= nTexWidth;
		TexMipInfo.PhysicalHeight			= nTexHeight;
		TexMipInfo.PhysicalDepth			= 1;

		D3D10_DDIARG_SUBRESOURCE_UP InitialTexTes;
		InitialTexTes.pSysMem				= pTexData;
		InitialTexTes.SysMemPitch			= (UINT)nRowBytes;
		InitialTexTes.SysMemSlicePitch		= (UINT)nRowBytes * nTexHeight;

		CreateTexRes.pMipInfoList			= &TexMipInfo;
		CreateTexRes.pInitialDataUP			= &InitialTexTes;
		CreateTexRes.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
		CreateTexRes.Usage					= D3D10_DDI_USAGE_DEFAULT;
		CreateTexRes.BindFlags				= D3D10_DDI_BIND_SHADER_RESOURCE;
		CreateTexRes.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		CreateTexRes.SampleDesc.Count		= 1;
		CreateTexRes.SampleDesc.Quality		= 0;
		CreateTexRes.MipLevels				= 1;
		CreateTexRes.ArraySize				= 1;

		D3D10DDI_HRESOURCE	hTexture		= { NULL };
		D3D10DDI_HRTRESOURCE hRTTexture		= { NULL };

		m_pDevice->CreateResource(hTexture, &CreateTexRes, hRTTexture);

		D3D11DDIARG_CREATESHADERRESOURCEVIEW	CreateResView;
		memset( &CreateResView, 0, sizeof( D3D11DDIARG_CREATESHADERRESOURCEVIEW ) );
		CreateResView.hDrvResource			= hTexture;
		CreateResView.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		CreateResView.ResourceDimension		= D3D10DDIRESOURCE_TEXTURE2D;
		CreateResView.Tex2D.MostDetailedMip	= 0;
		CreateResView.Tex2D.FirstArraySlice	= 0;
		CreateResView.Tex2D.MipLevels		= 1;
		CreateResView.Tex2D.ArraySize		= 1;

		D3D10DDI_HSHADERRESOURCEVIEW	hResView	= { NULL };
		D3D10DDI_HRTSHADERRESOURCEVIEW	hRTResView	= { NULL };
		m_pDevice->CreateShaderResourceView(hResView, &CreateResView, hRTResView);

		TD3D10UMShaderResourceView* pResHandle = DNew TD3D10UMShaderResourceView;
		pResHandle->m_hRes		= hTexture.pDrvPrivate;
		pResHandle->m_hResView	= hResView.pDrvPrivate;

		texture_.m_pDxTex = pResHandle;	
	}

	void	CDynRender_dx10UM::DeleteTexture ( CDynTexture& texture_ )
	{
		D3D10DDI_HDEVICE hDevice		= m_pDevice->GetHandle();

		TD3D10UMShaderResourceView* pResHandle = texture_.m_pDxTex;

		D3D10DDI_HRESOURCE	hTexture	= { NULL };
		hTexture.pDrvPrivate			= pResHandle->m_hRes;

		D3D10DDI_HSHADERRESOURCEVIEW	hResView	= { NULL };
		hResView.pDrvPrivate			= pResHandle->m_hResView;

		m_pDevice->OriginalDeviceFuncs.pfnDestroyShaderResourceView( hDevice, hResView );
		DELETE_HANDLE( hResView );

		m_pDevice->OriginalDeviceFuncs.pfnDestroyResource( hDevice, hTexture );
		DELETE_HANDLE( hTexture );		

		delete pResHandle;
	}

	HRESULT	CDynRender_dx10UM::SetTexture( const CDynTexture& texture_ )
	{
		if ( !IsValid() )
			return E_FAIL;

		m_tTexture = texture_;
		return S_OK;
	}

	HRESULT	CDynRender_dx10UM::Draw()
	{
		if ( !IsValid() )
			return E_FAIL;

		if( !m_nVertexCount )
			return S_OK;

		D3D10DDI_HDEVICE hDevice = m_pDevice->GetHandle();		

		/************************************************************************/

		D3D10DDI_MAPPED_SUBRESOURCE MapInfo;
		memset( &MapInfo, 0, sizeof( D3D10DDI_MAPPED_SUBRESOURCE ) );
		m_pDevice->OriginalDeviceFuncs.pfnDynamicIABufferMapDiscard( hDevice, m_hVertexBuffer, 0, D3D10_DDI_MAP_WRITE_DISCARD, 0, &MapInfo );
		memcpy( MapInfo.pData, &m_pSysMemVertexBuffer[ 0 ], m_nVertexCount * sizeof( SVertex ) );
		m_pDevice->OriginalDeviceFuncs.pfnDynamicIABufferUnmap( hDevice, m_hVertexBuffer, 0 );

		/************************************************************************/

		UINT nStride = sizeof( SVertex );
		UINT nOffset = 0;
		m_pDevice->OriginalDeviceFuncs.pfnIaSetVertexBuffers( hDevice, 0, 1, &m_hVertexBuffer, &nStride, &nOffset );
		m_pDevice->OriginalDeviceFuncs.pfnIaSetInputLayout( hDevice, m_hElementLayout );
		m_pDevice->OriginalDeviceFuncs.pfnSetRasterizerState( hDevice, m_hRastState );
			
		UINT nStartBuffer = 0;
		UINT nNumBuffers  = 1;
		m_pDevice->OriginalDeviceFuncs.pfnVsSetConstantBuffers( hDevice, nStartBuffer, nNumBuffers, &m_hCBView );
		m_pDevice->OriginalDeviceFuncs.pfnGsSetShader( hDevice, m_hGSNullShader );
		m_pDevice->OriginalDeviceFuncs.pfnVsSetShader( hDevice, m_hVSFontShader );		

		if ( m_eRenderType != RENDER_QUAD_CLIPPING )
		{
			m_pDevice->OriginalDeviceFuncs.pfnPsSetShader( hDevice, m_hPSFontShader );

			if ( m_tTexture.m_pDxTex )
			{
				TD3D10UMShaderResourceView*	pResHandle	= ( TD3D10UMShaderResourceView* ) m_tTexture.m_pDxTex;
				D3D10DDI_HSHADERRESOURCEVIEW hResView	= { pResHandle->m_hResView };

				FLOAT pfBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_pDevice->OriginalDeviceFuncs.pfnSetBlendState( hDevice, m_hBlendStateEnable, pfBlendFactor, 0xFFFFFFFF );
				m_pDevice->OriginalDeviceFuncs.pfnPsSetShaderResources( hDevice, 0, 1, &hResView );
				m_pDevice->OriginalDeviceFuncs.pfnPsSetSamplers( hDevice, 0, 1, &m_hSamplerState );
			}
		}
		else
		{
			m_pDevice->OriginalDeviceFuncs.pfnPsSetShader( hDevice, m_hPSBlackShader );

			FLOAT pfBlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			m_pDevice->OriginalDeviceFuncs.pfnSetBlendState( hDevice, m_hBlendStateDisable, pfBlendFactor, 0xFFFFFFFF );
		}

		if( m_eRenderType != RENDER_LINE_LIST )
			m_pDevice->OriginalDeviceFuncs.pfnIaSetTopology( hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		else
			m_pDevice->OriginalDeviceFuncs.pfnIaSetTopology( hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY_LINELIST );

		m_pDevice->OriginalDeviceFuncs.pfnDraw( hDevice, m_nVertexCount, 0 );

		/************************************************************************/			

		m_pSysMemVertices = 0;

		return S_OK;
	}

	bool	CDynRender_dx10UM::IsValid() const
	{
		return ( m_pDevice ) ? true : false;		
	}
	//////////////////////////////////////////////////////////////////////////

}
