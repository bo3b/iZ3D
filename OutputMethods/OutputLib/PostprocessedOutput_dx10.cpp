#include "StdAfx.h"
#include "PostprocessedOutput_dx10.h"
#include "S3DWrapper10/D3DSwapChain.h"
#include "S3DWrapper10/Commands/SetRenderTargets.h"
#include "S3DWrapper10/Commands/SetViewports.h"

using namespace DX10Output;

PostprocessedOutput::PostprocessedOutput(DWORD mode, DWORD spanMode)
:	OutputMethod(mode, spanMode)
{
	m_hPSShader.pDrvPrivate		= NULL;
	m_hVSShader.pDrvPrivate		= NULL;
	m_hRastState.pDrvPrivate	= NULL;
	m_hDSState.pDrvPrivate		= NULL;
	m_hBlendState.pDrvPrivate	= NULL;
	m_hSamplerState.pDrvPrivate = NULL;
}

PostprocessedOutput::~PostprocessedOutput(void)
{
}

void PostprocessedOutput::Initialize( D3DDeviceWrapper* dev )
{
	OutputMethod::Initialize(dev);
	InitStateObjects(dev);
	InitializeResources(dev);
}

void PostprocessedOutput::InitStateObjects( D3DDeviceWrapper* pWrapper_ )
{
	D3D10DDI_HDEVICE hDevice = pWrapper_->GetHandle();
	//////////////////////////////////////////////////////////////////////////
	// Rasterizer State //

	D3D10_DDI_RASTERIZER_DESC		rastDesc;
	D3D10DDI_HRTRASTERIZERSTATE		hRTRasterState = { NULL };
	ZeroMemory( &rastDesc, sizeof( D3D10_DDI_RASTERIZER_DESC ) );
	rastDesc.FillMode						= D3D10_DDI_FILL_SOLID;
	rastDesc.CullMode						= D3D10_DDI_CULL_NONE;
	rastDesc.FrontCounterClockwise			= TRUE;
	rastDesc.DepthBias						= 0;
	rastDesc.DepthBiasClamp					= 0.0f;
	rastDesc.SlopeScaledDepthBias			= 0.0f;
	rastDesc.DepthClipEnable				= FALSE;
	rastDesc.ScissorEnable					= FALSE;
	rastDesc.MultisampleEnable				= FALSE;
	rastDesc.AntialiasedLineEnable			= FALSE;

	SIZE_T nSize = pWrapper_->OriginalDeviceFuncs.pfnCalcPrivateRasterizerStateSize( hDevice, &rastDesc );	
	NEW_HANDLE(m_hRastState, nSize);
	pWrapper_->OriginalDeviceFuncs.pfnCreateRasterizerState( hDevice, &rastDesc, m_hRastState, hRTRasterState );

	//////////////////////////////////////////////////////////////////////////
	// Depth-Stencil State //

	D3D10_DDI_DEPTH_STENCIL_DESC	dsDesc;
	D3D10DDI_HRTDEPTHSTENCILSTATE	hRTDSState = { NULL };
	ZeroMemory( &dsDesc, sizeof( D3D10_DDI_DEPTH_STENCIL_DESC ) );
	dsDesc.DepthEnable						= FALSE;
	dsDesc.DepthWriteMask					= D3D10_DDI_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc						= D3D10_DDI_COMPARISON_ALWAYS;
	dsDesc.StencilEnable					= FALSE;
	dsDesc.FrontEnable						= TRUE;
	dsDesc.BackEnable						= TRUE;
	dsDesc.StencilReadMask					= 0;
	dsDesc.StencilWriteMask					= 0;

	dsDesc.FrontFace.StencilFailOp			= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp		= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp			= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc			= D3D10_DDI_COMPARISON_NEVER;

	dsDesc.BackFace.StencilFailOp			= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp		= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp			= D3D10_DDI_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc				= D3D10_DDI_COMPARISON_NEVER;

	nSize = pWrapper_->OriginalDeviceFuncs.pfnCalcPrivateDepthStencilStateSize( hDevice, &dsDesc );	
	NEW_HANDLE(m_hDSState, nSize);
	pWrapper_->OriginalDeviceFuncs.pfnCreateDepthStencilState( hDevice, &dsDesc, m_hDSState, hRTDSState );

	//////////////////////////////////////////////////////////////////////////

	UINT nVersion = pWrapper_->GetInterfaceVersion();

	if	( pWrapper_->GetD3DVersion() == TD3DVersion_10_0 )
	{
		// Blend State Dx 10 //
		D3D10_DDI_BLEND_DESC		blendDesc;
		D3D10DDI_HRTBLENDSTATE		hRTBlendState = { NULL };
		ZeroMemory( &blendDesc, sizeof( D3D10_DDI_BLEND_DESC ) );
		blendDesc.AlphaToCoverageEnable			= FALSE;
		blendDesc.SrcBlend						= D3D10_DDI_BLEND_ONE;
		blendDesc.DestBlend						= D3D10_DDI_BLEND_ZERO;
		blendDesc.BlendOp						= D3D10_DDI_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha					= D3D10_DDI_BLEND_ONE;
		blendDesc.DestBlendAlpha				= D3D10_DDI_BLEND_ZERO;
		blendDesc.BlendOpAlpha					= D3D10_DDI_BLEND_OP_ADD;
		for ( int i = 0; i < D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
		{
			blendDesc.BlendEnable[ i ]			= FALSE;
			blendDesc.RenderTargetWriteMask[i]	= D3D10_DDI_COLOR_WRITE_ENABLE_ALL;
		}

		nSize = pWrapper_->OriginalDeviceFuncs.pfnCalcPrivateBlendStateSize( hDevice, &blendDesc );	
		NEW_HANDLE(m_hBlendState, nSize);
		pWrapper_->OriginalDeviceFuncs.pfnCreateBlendState( hDevice, &blendDesc, m_hBlendState, hRTBlendState );
	}
	else
	{
		// Blend State Dx 10.1 //
		D3D10_1_DDI_BLEND_DESC		blendDesc;
		D3D10DDI_HRTBLENDSTATE		hRTBlendState = { NULL };
		ZeroMemory( &blendDesc, sizeof( D3D10_1_DDI_BLEND_DESC ) );
		blendDesc.AlphaToCoverageEnable			= FALSE;
		blendDesc.IndependentBlendEnable		= FALSE;

		for ( int i = 0; i < D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
		{
			blendDesc.RenderTarget[i].BlendEnable			= FALSE;
			blendDesc.RenderTarget[i].SrcBlend				= D3D10_DDI_BLEND_ONE;
			blendDesc.RenderTarget[i].DestBlend				= D3D10_DDI_BLEND_ZERO;
			blendDesc.RenderTarget[i].BlendOp				= D3D10_DDI_BLEND_OP_ADD;
			blendDesc.RenderTarget[i].SrcBlendAlpha			= D3D10_DDI_BLEND_ONE;
			blendDesc.RenderTarget[i].DestBlendAlpha		= D3D10_DDI_BLEND_ZERO;
			blendDesc.RenderTarget[i].BlendOpAlpha			= D3D10_DDI_BLEND_OP_ADD;
			blendDesc.RenderTarget[i].RenderTargetWriteMask	= D3D10_DDI_COLOR_WRITE_ENABLE_ALL;
		}

		nSize = pWrapper_->OriginalDeviceFuncs10_1.pfnCalcPrivateBlendStateSize( hDevice, &blendDesc );	
		NEW_HANDLE(m_hBlendState, nSize);
		pWrapper_->OriginalDeviceFuncs10_1.pfnCreateBlendState( hDevice, &blendDesc, m_hBlendState, hRTBlendState );
	}
	
	
	//////////////////////////////////////////////////////////////////////////
	// Sampler State Dx 10 //	

	D3D10_DDI_SAMPLER_DESC	samplerDesc;
	D3D10DDI_HRTSAMPLER		hRTSampler = { NULL };
	ZeroMemory( &samplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ) );	

	samplerDesc.Filter						=   D3D10_DDI_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW					=	D3D10_DDI_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias					=	0.0f;
	samplerDesc.MaxAnisotropy				=	1;
	samplerDesc.ComparisonFunc				=	D3D10_DDI_COMPARISON_NEVER;
	samplerDesc.BorderColor[0]				=	0.0f;
	samplerDesc.BorderColor[1]				=	0.0f;
	samplerDesc.BorderColor[2]				=	0.0f;
	samplerDesc.BorderColor[3]				=	0.0f;
	samplerDesc.MinLOD						=	0.0f;
	samplerDesc.MaxLOD						=	FLT_MAX;

	nSize = pWrapper_->OriginalDeviceFuncs.pfnCalcPrivateSamplerSize( hDevice, &samplerDesc );
	NEW_HANDLE(m_hSamplerState, nSize);
	pWrapper_->OriginalDeviceFuncs.pfnCreateSampler( hDevice, &samplerDesc, m_hSamplerState, hRTSampler );

	//////////////////////////////////////////////////////////////////////////	
}

void PostprocessedOutput::Clear( D3DDeviceWrapper* dev )
{
	if(!m_bInitialized)
		return;

	DestroyStateObjects(dev);
	ClearResources(dev);
	OutputMethod::Clear(dev);
}

void PostprocessedOutput::DestroyStateObjects ( D3DDeviceWrapper* pWrapper_ )
{
	D3D10DDI_HDEVICE hDevice = pWrapper_->GetHandle();
	//////////////////////////////////////////////////////////////////////////
	// destroy all //
	pWrapper_->OriginalDeviceFuncs.pfnDestroyRasterizerState( hDevice, m_hRastState );
	DELETE_HANDLE(m_hRastState);
	pWrapper_->OriginalDeviceFuncs.pfnDestroyDepthStencilState( hDevice, m_hDSState );
	DELETE_HANDLE(m_hDSState);
	pWrapper_->OriginalDeviceFuncs.pfnDestroyBlendState( hDevice, m_hBlendState );
	DELETE_HANDLE(m_hBlendState);
	pWrapper_->OriginalDeviceFuncs.pfnDestroySampler( hDevice, m_hSamplerState );	
	DELETE_HANDLE(m_hSamplerState);
}

void PostprocessedOutput::Output( D3DSwapChain* pSwapChain )
{
	D3DDeviceWrapper* pD3DDevice	= pSwapChain->m_pD3DDeviceWrapper;
	SwapChainResources* pRes		= pSwapChain->GetCurrentPrimaryResource();
	D3D10DDI_DEVICEFUNCS& GET_ORIG	= pD3DDevice->OriginalDeviceFuncs;
	D3D11DDI_DEVICEFUNCS& GET_ORIG11 = pD3DDevice->OriginalDeviceFuncs11;
	D3D10DDI_HDEVICE hDevice		= pD3DDevice->hDevice;
	D3DDeviceState& state			= pD3DDevice->m_DeviceState;

	D3D10DDI_HSHADERRESOURCEVIEW hResources[2] = { NULL, NULL };

	hResources[ 0 ] = pRes->GetMethodTextureLeft();
	hResources[ 1 ] = pRes->GetMethodTextureRight();

	GET_ORIG.pfnPsSetShaderResources( hDevice, 0, _countof( hResources ), hResources );
	ProcessSubMode();

	D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView = { NULL };
	if (!pD3DDevice->m_bTwoWindows)
	{
		D3D10DDI_HRENDERTARGETVIEW hRenderTargets [] =
		{
			pRes->GetBackBufferRTPrimary()
		};
		UINT nClearRTs = Commands::GetClearCount( _countof(hRenderTargets), state.m_NumRenderTargets );
		if ( pD3DDevice->GetD3DVersion() != TD3DVersion_11_0 )
			GET_ORIG.pfnSetRenderTargets( hDevice, hRenderTargets, _countof(hRenderTargets), nClearRTs, hDepthStencilView );
		else
			GET_ORIG11.pfnSetRenderTargets( hDevice, hRenderTargets, _countof(hRenderTargets), nClearRTs, hDepthStencilView,
				NULL, NULL, _countof(hRenderTargets), 0, _countof(hRenderTargets), 0 );
	}
	else
	{
		D3D10DDI_HRENDERTARGETVIEW hRenderTargets [] =
		{
			pRes->GetBackBufferRTPrimary(),
			pRes->GetBackBufferRTSecondary()
		};
		UINT nClearRTs = Commands::GetClearCount( _countof(hRenderTargets), state.m_NumRenderTargets );
		if ( pD3DDevice->GetD3DVersion() != TD3DVersion_11_0 )
			GET_ORIG.pfnSetRenderTargets( hDevice, hRenderTargets, _countof(hRenderTargets), nClearRTs, hDepthStencilView );
		else
			GET_ORIG11.pfnSetRenderTargets( hDevice, hRenderTargets, _countof(hRenderTargets), nClearRTs, hDepthStencilView,
				NULL, NULL, _countof(hRenderTargets), 0, _countof(hRenderTargets), 0 );
	}

	GET_ORIG.pfnIaSetTopology(hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// set all states objects //

	FLOAT pfBlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GET_ORIG.pfnSetRasterizerState( hDevice, m_hRastState );
	GET_ORIG.pfnSetDepthStencilState( hDevice, m_hDSState, 0xFFFFFFFF );	
	GET_ORIG.pfnSetBlendState( hDevice, m_hBlendState, pfBlendFactor, 0xFFFFFFFF );
	//GET_ORIG.pfnSetScissorRects( hDevice, 0, 0, NULL );	

	GET_ORIG.pfnPsSetSamplers( hDevice, 0, 1, &m_hSamplerState );

	GET_ORIG.pfnVsSetShader(hDevice, m_hVSShader);
	GET_ORIG.pfnPsSetShader(hDevice, m_hPSShader);
	D3D10DDI_HSHADER hGSShader = { NULL };
	GET_ORIG.pfnGsSetShader(hDevice, hGSShader);
	D3D10DDI_HELEMENTLAYOUT  hInputLayout = { NULL };
	GET_ORIG.pfnIaSetInputLayout(hDevice, hInputLayout);
	D3D10DDI_HRESOURCE  phBuffers[16] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
										  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	UINT  Strides[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0 };
	UINT  Offsets[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 
						0, 0, 0, 0, 0, 0, 0, 0 };
	GET_ORIG.pfnIaSetVertexBuffers(hDevice, 0, _countof(phBuffers), phBuffers, Strides, Offsets);

	// set additional states
	SetStates( pD3DDevice, pSwapChain );
	//////////////////////////////////////////////////////////////////////////
	GET_ORIG.pfnDraw(hDevice, 3, 0);

	// unbind used resources
	D3D10DDI_HSHADERRESOURCEVIEW hResourcesNull[] = { NULL, NULL };
	GET_ORIG.pfnPsSetShaderResources( hDevice, 0, _countof(hResourcesNull), hResourcesNull );
	if (pD3DDevice->m_bTwoWindows)
	{
		D3D10DDI_HRENDERTARGETVIEW hRenderTargetsNull [] = { NULL };
		D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilViewNull = { NULL };
		UINT nClearRTs = Commands::GetClearCount( _countof( hRenderTargetsNull ), state.m_NumRenderTargets );
		if ( pD3DDevice->GetD3DVersion() != TD3DVersion_11_0 )
			GET_ORIG.pfnSetRenderTargets(hDevice, hRenderTargetsNull, _countof(hRenderTargetsNull), nClearRTs, hDepthStencilViewNull);
		else
			GET_ORIG11.pfnSetRenderTargets( hDevice, hRenderTargetsNull, _countof(hRenderTargetsNull), nClearRTs, hDepthStencilViewNull,
				NULL, NULL, _countof(hRenderTargetsNull), 0, _countof(hRenderTargetsNull), 0 );
	}
}

namespace DX10Output
{

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

bool ParseShaderBlob ( CONST BYTE* pData, DWORD dataSize, CONST UINT** ppCode)
{
	_ASSERT(dataSize % 4 == 0);
	return ParseShaderBlob ( (DWORD*)pData, dataSize, ppCode);
}

}