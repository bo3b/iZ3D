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
#include "Shaders_dx10/shaderMono.h"
#include "Shaders_dx10/shaderDLP3D.h"

OUTPUT_API void* CALLBACK CreateOutputDX10( DWORD nMode_, DWORD nSpanMode_ )
{
	return new ATIDLP3DOutput( nMode_, nSpanMode_ );
}

ATIDLP3DOutput::ATIDLP3DOutput( DWORD nMode_, DWORD nSpanMode_ )
: PostprocessedOutput( nMode_, nSpanMode_ )
{
	//--- create signature ---
	m_SigVS.NumInputSignatureEntries	= 1;
	m_SigVS.NumOutputSignatureEntries	= 2;
	m_SigVS.pInputSignature				= &m_SigEntryVS[ 0 ];
	m_SigEntryVS[0].SystemValue			= D3D10_SB_NAME_VERTEX_ID;
	m_SigEntryVS[0].Register			= 0;
	m_SigEntryVS[0].Mask				= 1;
	m_SigVS.pOutputSignature			= &m_SigEntryVS[ m_SigVS.NumInputSignatureEntries ];
	m_SigEntryVS[1].SystemValue			= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryVS[1].Register			= 0;
	m_SigEntryVS[1].Mask				= 0x03;
	m_SigEntryVS[2].SystemValue			= D3D10_SB_NAME_POSITION;
	m_SigEntryVS[2].Register			= 1;
	m_SigEntryVS[2].Mask				= 0x0F;

	m_SigPS.NumInputSignatureEntries	= 1;
	m_SigPS.NumOutputSignatureEntries	= 1;
	m_SigPS.pInputSignature				= &m_SigEntryPS[ 0 ];
	m_SigEntryPS[0].SystemValue			= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[0].Register			= 0;
	m_SigEntryPS[0].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y;
	m_SigPS.pOutputSignature			= &m_SigEntryPS[ m_SigPS.NumInputSignatureEntries ];
	m_SigEntryPS[1].SystemValue			= D3D10_SB_NAME_UNDEFINED;
	m_SigEntryPS[1].Register			= 0;
	m_SigEntryPS[1].Mask				= D3D10_SB_COMPONENT_MASK_X | D3D10_SB_COMPONENT_MASK_Y | D3D10_SB_COMPONENT_MASK_Z | D3D10_SB_COMPONENT_MASK_W;

	m_hCB.pDrvPrivate					= NULL;
	m_hSamplerState.pDrvPrivate			= NULL;
}

ATIDLP3DOutput::~ATIDLP3DOutput()
{
}

void ATIDLP3DOutput::InitializeResources( D3DDeviceWrapper* pD3DWrapper_ )
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

	ParseShaderBlob( g_shaderDLP3D, sizeof( g_shaderDLP3D ), &pCode );
	nSize = GET_ORIG.pfnCalcPrivateShaderSize( hDevice, pCode, &m_SigPS );
	NEW_HANDLE( m_hPSShader, nSize );
	GET_ORIG.pfnCreatePixelShader( hDevice, pCode, m_hPSShader, hRTShader, &m_SigPS );
	// end create  shaders
	/************************************************************************/	
	// begin create  constant buffer
	D3D11DDIARG_CREATERESOURCE CreateCBRes;
	memset( &CreateCBRes, 0, sizeof( D3D11DDIARG_CREATERESOURCE ) );

	D3D10DDI_MIPINFO CBMipInfo;
	CBMipInfo.TexelWidth				= m_FloatCount * 4;
	CBMipInfo.TexelHeight				= 1;
	CBMipInfo.TexelDepth				= 1;
	CBMipInfo.PhysicalWidth				= m_FloatCount * 4;
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
	D3D10DDI_HRTRESOURCE hRTCBView		= { RTData };

	nSize = GET_ORIG11.pfnCalcPrivateResourceSize( hDevice, &CreateCBRes );
	NEW_HANDLE( m_hCB, nSize );
	GET_ORIG11.pfnCreateResource( hDevice, &CreateCBRes, m_hCB, hRTCBView );

	// end create  constant buffer	
	/************************************************************************/
	// begin create sampler state
	D3D10_DDI_SAMPLER_DESC	samplerDesc;
	D3D10DDI_HRTSAMPLER		hRTSampler = { NULL };
	ZeroMemory( &samplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ) );	

	samplerDesc.Filter						=   D3D10_DDI_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU					=	D3D10_DDI_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV					=	D3D10_DDI_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW					=	D3D10_DDI_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias					=	0.0f;
	samplerDesc.MaxAnisotropy				=	1;
	samplerDesc.ComparisonFunc				=	D3D10_DDI_COMPARISON_NEVER;
	samplerDesc.BorderColor[0]				=	0.0f;
	samplerDesc.BorderColor[1]				=	0.0f;
	samplerDesc.BorderColor[2]				=	0.0f;
	samplerDesc.BorderColor[3]				=	0.0f;
	samplerDesc.MinLOD						=	0.0f;
	samplerDesc.MaxLOD						=	FLT_MAX;

	nSize = GET_ORIG.pfnCalcPrivateSamplerSize( hDevice, &samplerDesc );
	NEW_HANDLE( m_hSamplerState, nSize );
	GET_ORIG.pfnCreateSampler( hDevice, &samplerDesc, m_hSamplerState, hRTSampler );
	// end create sampler state
	/************************************************************************/
}

void ATIDLP3DOutput::SetStates(  D3DDeviceWrapper* pDevice_, D3DSwapChain* pSwapChain_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pDevice_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice		= pDevice_->hDevice;

	float fWidth	= (float)pSwapChain_->m_Description.BufferDesc.Width;
	float fHeight	= (float)pSwapChain_->m_Description.BufferDesc.Height;

	m_pCB[ 0 ] = fWidth	* 0.5f;
	m_pCB[ 1 ] = fHeight * 0.5f;
	m_pCB[ 2 ] = 0.5f / fWidth;
	m_pCB[ 3 ] = 0.5f / fHeight;

	m_pCB[ 4 ] = (pSwapChain_->m_DstMonitorPoint.x % 2) * 0.5f;
	m_pCB[ 5 ] = (pSwapChain_->m_DstMonitorPoint.y % 2) * 0.5f;
	m_pCB[ 6 ] = 0.0f;
	m_pCB[ 7 ] = 0.0f;	

	GET_ORIG.pfnDefaultConstantBufferUpdateSubresourceUP( hDevice, m_hCB, 0, NULL, ( const void* )m_pCB, m_FloatCount * 4, m_FloatCount * 4 );

	UINT nStartBuffer = 0;
	UINT nNumBuffers  = 1;
	GET_ORIG.pfnPsSetConstantBuffers( hDevice, nStartBuffer, nNumBuffers, &m_hCB );
	GET_ORIG.pfnPsSetSamplers( hDevice, 0, 1, &m_hSamplerState );
}

void ATIDLP3DOutput::ClearResources( D3DDeviceWrapper* pD3DWrapper_ )
{
	D3D10DDI_DEVICEFUNCS& GET_ORIG = pD3DWrapper_->OriginalDeviceFuncs;
	D3D10DDI_HDEVICE hDevice = pD3DWrapper_->hDevice;

	GET_ORIG.pfnDestroyShader( hDevice, m_hVSShader );
	DELETE_HANDLE( m_hVSShader );
	GET_ORIG.pfnDestroyShader( hDevice, m_hPSShader );
	DELETE_HANDLE( m_hPSShader );
	GET_ORIG.pfnDestroyResource( hDevice, m_hCB );
	DELETE_HANDLE( m_hCB );
	GET_ORIG.pfnDestroySampler( hDevice, m_hSamplerState );	
	DELETE_HANDLE( m_hSamplerState );
}

bool ATIDLP3DOutput::FindAdapter( IDXGIFactory* pDXGIFactory_ )
{
	IDXGIAdapter*		pDXGIAdapter	= NULL;
	const DWORD			ATIVendorId		= 0x1002;
	DXGI_ADAPTER_DESC	desc;
	
	for ( UINT i = 0; pDXGIFactory_->EnumAdapters( i, &pDXGIAdapter ) != DXGI_ERROR_NOT_FOUND; ++i )
	{
		pDXGIAdapter->GetDesc( &desc );
		pDXGIAdapter->Release();
		if ( desc.VendorId == ATIVendorId )
			return true;
	}
	

#ifndef FINAL_RELEASE
	return true;
#else
	return false;
#endif

}