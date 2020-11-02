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
#include "stdafx.h"
#include "Output_dx10.h"

using namespace DX10Output;

#include "Shaders_dx10/shaderFullScreenQuadVS.h"
#include "Shaders_dx10/shaderStereoMirror.h"

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new StereoMirrorOutput( nMode_, nSpanMode_ );
}

StereoMirrorOutput::StereoMirrorOutput( DWORD nMode_, DWORD nSpanMode_ )
: PostprocessedOutput( nMode_, nSpanMode_ )
{
	m_SigVS.NumInputSignatureEntries	= 1;
	m_SigVS.NumOutputSignatureEntries	= 3;
	m_SigVS.pInputSignature				= &m_SigEntryVS[ 0 ];
	m_SigEntryVS[ 0 ].SystemValue		= D3D10_SB_NAME_VERTEX_ID;
	m_SigEntryVS[ 0 ].Register			= 0;
	m_SigEntryVS[ 0 ].Mask				= D3D10_SB_COMPONENT_MASK_X;

	m_SigVS.pOutputSignature			= &m_SigEntryVS[ m_SigVS.NumInputSignatureEntries ];
	m_SigEntryVS[ 1 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryVS[ 1 ].Register			= 0;
	m_SigEntryVS[ 1 ].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y;
	m_SigEntryVS[ 2 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryVS[ 2 ].Register			= 0;
	m_SigEntryVS[ 2 ].Mask				= D3D10_SB_COMPONENT_MASK_Z | D3D10_SB_COMPONENT_MASK_W;
	m_SigEntryVS[ 3 ].SystemValue		= D3D10_SB_NAME_POSITION;
	m_SigEntryVS[ 3 ].Register			= 1;
	m_SigEntryVS[ 3 ].Mask				= D3D10_SB_COMPONENT_MASK_ALL;

	m_SigPS.NumInputSignatureEntries	= 2;
	m_SigPS.NumOutputSignatureEntries	= 2;
	m_SigPS.pInputSignature				= &m_SigEntryPS[ 0 ];
	m_SigEntryPS[ 0 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 0 ].Register			= 0;
	m_SigEntryPS[ 0 ].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y;
	m_SigEntryPS[ 1 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 1 ].Register			= 0;
	m_SigEntryPS[ 1 ].Mask				= D3D10_SB_COMPONENT_MASK_Z | D3D10_SB_COMPONENT_MASK_W;

	m_SigPS.pOutputSignature			= &m_SigEntryPS[ m_SigPS.NumInputSignatureEntries ];
	m_SigEntryPS[ 2 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 2 ].Register			= 0;
	m_SigEntryPS[ 2 ].Mask				= D3D10_SB_COMPONENT_MASK_ALL;
	m_SigEntryPS[ 3 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 3 ].Register			= 1;
	m_SigEntryPS[ 3 ].Mask				= D3D10_SB_COMPONENT_MASK_ALL;

	m_hCB.pDrvPrivate = NULL;

	m_pCB[ 0 ] = ( m_OutputMode == 0 || m_OutputMode == 3 || m_OutputMode == 4 ) ? 1.0f : 0.0f;
	m_pCB[ 1 ] = ( m_OutputMode == 1 || m_OutputMode == 2 || m_OutputMode == 5 ) ? 1.0f : 0.0f;
	m_pCB[ 2 ] = ( m_OutputMode == 2 || m_OutputMode == 4 ) ? 1.0f : 0.0f;
	m_pCB[ 3 ] = ( m_OutputMode == 3 || m_OutputMode == 5 ) ? 1.0f : 0.0f;

	m_hSState.pDrvPrivate = NULL;
}

StereoMirrorOutput::~StereoMirrorOutput()
{
}

void StereoMirrorOutput::InitializeResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG		= pD3DWrapper_->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& GET_ORIG11	= pD3DWrapper_->OriginalDeviceFuncs11;
	D3D10DDI_HDEVICE hDevice			= pD3DWrapper_->hDevice;
	CONST UINT* pCode					= NULL;

	/************************************************************************/
	// begin create  shaders
	ParseShaderBlob( g_shaderFullScreenQuadVS, sizeof( g_shaderFullScreenQuadVS ), &pCode );
	SIZE_T nSize = GET_ORIG.pfnCalcPrivateShaderSize( hDevice, pCode, &m_SigVS );
	NEW_HANDLE( m_hVSShader, nSize );
	D3D10DDI_HRTSHADER hRTShader = { NULL };
	GET_ORIG.pfnCreateVertexShader( hDevice, pCode, m_hVSShader, hRTShader, &m_SigVS );

	pCode = NULL;
	ParseShaderBlob( g_shaderStereoMirror, sizeof( g_shaderStereoMirror ), &pCode );
	nSize = GET_ORIG.pfnCalcPrivateShaderSize( hDevice, pCode, &m_SigPS );
	NEW_HANDLE( m_hPSShader, nSize );
	GET_ORIG.pfnCreatePixelShader( hDevice, pCode, m_hPSShader, hRTShader, &m_SigPS );
	// end create  shaders
	/************************************************************************/
	// begin create CB
	D3D11DDIARG_CREATERESOURCE CreateCBRes;
	memset( &CreateCBRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	D3D10DDI_MIPINFO CBMipInfo;
	CBMipInfo.TexelWidth				= 16;
	CBMipInfo.TexelHeight				= 1;
	CBMipInfo.TexelDepth				= 1;
	CBMipInfo.PhysicalWidth				= 16;
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

	m_hCB.pDrvPrivate					= NULL;
	static char RTData[16]				= {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
	D3D10DDI_HRTRESOURCE hRTCB			= { RTData };
	nSize = GET_ORIG11.pfnCalcPrivateResourceSize( hDevice, &CreateCBRes );
	NEW_HANDLE( m_hCB, nSize );
	GET_ORIG11.pfnCreateResource( hDevice, &CreateCBRes, m_hCB, hRTCB );

	GET_ORIG.pfnDefaultConstantBufferUpdateSubresourceUP( hDevice, m_hCB, 0, NULL, ( const void* )m_pCB, 16, 16 );
	// end create CB
	/************************************************************************/
	// start sampler state
	D3D10_DDI_SAMPLER_DESC	SamplerDesc;
	D3D10DDI_HRTSAMPLER		hRTSampler = { NULL };
	ZeroMemory( &SamplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ) );	

	SamplerDesc.Filter						=   D3D10_DDI_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressV					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.AddressW					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	SamplerDesc.MipLODBias					=	0.0f;
	SamplerDesc.MaxAnisotropy				=	1;
	SamplerDesc.ComparisonFunc				=	D3D10_DDI_COMPARISON_NEVER;
	SamplerDesc.BorderColor[ 0 ]			=	0.0f;
	SamplerDesc.BorderColor[ 1 ]			=	0.0f;
	SamplerDesc.BorderColor[ 2 ]			=	0.0f;
	SamplerDesc.BorderColor[ 3 ]			=	0.0f;
	SamplerDesc.MinLOD						=	0.0f;
	SamplerDesc.MaxLOD						=	FLT_MAX;

	nSize = GET_ORIG.pfnCalcPrivateSamplerSize( hDevice, &SamplerDesc );
	NEW_HANDLE( m_hSState, nSize );
	GET_ORIG.pfnCreateSampler( hDevice, &SamplerDesc, m_hSState, hRTSampler );
	// end sampler state
	/************************************************************************/
}

void StereoMirrorOutput::SetStates( D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pDevice_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pDevice_->hDevice;

	UINT nStartBuffer = 0;
	UINT nNumBuffers  = 1;
	GET_ORIG.pfnVsSetConstantBuffers( hDevice, nStartBuffer, nNumBuffers, &m_hCB );
	GET_ORIG.pfnPsSetSamplers( hDevice, 0, 1, &m_hSState );
}

void StereoMirrorOutput::ClearResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->hDevice;

	GET_ORIG.pfnDestroyShader( hDevice, m_hVSShader );
	DELETE_HANDLE( m_hVSShader );
	GET_ORIG.pfnDestroyShader( hDevice, m_hPSShader );
	DELETE_HANDLE( m_hPSShader );
	GET_ORIG.pfnDestroyResource( hDevice, m_hCB );
	DELETE_HANDLE( m_hCB );
	GET_ORIG.pfnDestroySampler( hDevice, m_hSState );
	DELETE_HANDLE( m_hSState );
}

DWORD StereoMirrorOutput::GetSecondWindowCaps()
{
	DWORD c = 0;
	if (m_OutputMode == 0 || m_OutputMode == 3 || m_OutputMode == 4)
		c |= swcInvertHorizontal;
	if (m_OutputMode == 1 || m_OutputMode == 2 || m_OutputMode == 5)
		c |= swcInvertVertical;
	return c;
}