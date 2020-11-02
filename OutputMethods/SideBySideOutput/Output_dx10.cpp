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
#include "Shaders_dx10/shaderHorizontalSideBySide.h"
#include "Shaders_dx10/shaderVerticalSideBySide.h"

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new SideBySideOutput( nMode_, nSpanMode_ );
}

SideBySideOutput::SideBySideOutput( DWORD nMode_, DWORD nSpanMode_ )
: PostprocessedOutput( nMode_, nSpanMode_ )
{
	if ( nMode_ != 2 )
		m_OutputMode &= 1;
	else
	{
		m_bCrosseyed = true;
		m_OutputMode = 0;
	}

	m_SigVS.NumInputSignatureEntries	= 1;
	m_SigVS.NumOutputSignatureEntries	= 2;
	m_SigVS.pInputSignature				= &m_SigEntryVS[ 0 ];
	m_SigEntryVS[ 0 ].SystemValue		= D3D10_SB_NAME_VERTEX_ID;
	m_SigEntryVS[ 0 ].Register			= 0;
	m_SigEntryVS[ 0 ].Mask				= 1;
	m_SigVS.pOutputSignature			= &m_SigEntryVS[ m_SigVS.NumInputSignatureEntries ];
	m_SigEntryVS[ 1 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryVS[ 1 ].Register			= 0;
	m_SigEntryVS[ 1 ].Mask				= 0x03;
	m_SigEntryVS[ 2 ].SystemValue		= D3D10_SB_NAME_POSITION;
	m_SigEntryVS[ 2 ].Register			= 1;
	m_SigEntryVS[ 2 ].Mask				= 0x0F;

	m_SigPS.NumInputSignatureEntries	= 1;
	m_SigPS.NumOutputSignatureEntries	= 1;
	m_SigPS.pInputSignature				= &m_SigEntryPS[ 0 ];
	m_SigEntryPS[ 0 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 0 ].Register			= 0;
	m_SigEntryPS[ 0 ].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y;
	m_SigPS.pOutputSignature			= &m_SigEntryPS[ m_SigPS.NumInputSignatureEntries ];
	m_SigEntryPS[ 1 ].SystemValue		= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[ 1 ].Register			= 0;
	m_SigEntryPS[ 1 ].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y | D3D10_SB_COMPONENT_MASK_Z | D3D10_SB_COMPONENT_MASK_W;

	m_hSamplerState.pDrvPrivate			= NULL;
}

SideBySideOutput::~SideBySideOutput()
{
}

void SideBySideOutput::InitializeResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DWrapper_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pD3DWrapper_->hDevice;
	CONST UINT* pCode				= NULL;

	/************************************************************************/
	// begin create  shaders
	ParseShaderBlob( g_shaderFullScreenQuadVS, sizeof( g_shaderFullScreenQuadVS ), &pCode );
	SIZE_T nSize = GET_ORIG.pfnCalcPrivateShaderSize( hDevice, pCode, &m_SigVS );
	NEW_HANDLE( m_hVSShader, nSize );
	D3D10DDI_HRTSHADER hRTShader = { NULL };
	GET_ORIG.pfnCreateVertexShader( hDevice, pCode, m_hVSShader, hRTShader, &m_SigVS );

	pCode = NULL;
	if( m_OutputMode == 0 )
	{
		ParseShaderBlob( g_shaderHorizontalSideBySide, sizeof( g_shaderHorizontalSideBySide ), &pCode );
	}
	else
	{
		ParseShaderBlob( g_shaderVerticalSideBySide, sizeof( g_shaderVerticalSideBySide ), &pCode );
	}

	nSize = GET_ORIG.pfnCalcPrivateShaderSize( hDevice, pCode, &m_SigPS );
	NEW_HANDLE( m_hPSShader, nSize );
	GET_ORIG.pfnCreatePixelShader( hDevice, pCode, m_hPSShader, hRTShader, &m_SigPS );
	// end create  shaders
	/************************************************************************/
	// begin create sampler state
	D3D10_DDI_SAMPLER_DESC	samplerDesc;
	D3D10DDI_HRTSAMPLER		hRTSampler = { NULL };
	ZeroMemory( &samplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ) );	

	samplerDesc.Filter						=   D3D10_DDI_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU					=	D3D10_DDI_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV					=	D3D10_DDI_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW					=	D3D10_DDI_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias					=	0.0f;
	samplerDesc.MaxAnisotropy				=	1;
	samplerDesc.ComparisonFunc				=	D3D10_DDI_COMPARISON_NEVER;
	samplerDesc.BorderColor[ 0 ]			=	0.0f;
	samplerDesc.BorderColor[ 1 ]			=	0.0f;
	samplerDesc.BorderColor[ 2 ]			=	0.0f;
	samplerDesc.BorderColor[ 3 ]			=	0.0f;
	samplerDesc.MinLOD						=	0.0f;
	samplerDesc.MaxLOD						=	FLT_MAX;

	nSize = GET_ORIG.pfnCalcPrivateSamplerSize( hDevice, &samplerDesc );
	NEW_HANDLE( m_hSamplerState, nSize );
	GET_ORIG.pfnCreateSampler( hDevice, &samplerDesc, m_hSamplerState, hRTSampler );
	// end create sampler state
	/************************************************************************/
}

void SideBySideOutput::SetStates(  D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pDevice_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pDevice_->hDevice;

	GET_ORIG.pfnPsSetSamplers( hDevice, 0, 1, &m_hSamplerState );
}

void SideBySideOutput::ClearResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->hDevice;

	GET_ORIG.pfnDestroyShader( hDevice, m_hVSShader );
	DELETE_HANDLE( m_hVSShader );
	GET_ORIG.pfnDestroyShader( hDevice, m_hPSShader );
	DELETE_HANDLE( m_hPSShader );
	GET_ORIG.pfnDestroySampler( hDevice, m_hSamplerState );	
	DELETE_HANDLE( m_hSamplerState );
}