#include "StdAfx.h"
#include "D3DDeviceState.h"
#include "D3DDeviceWrapper.h"
#include "Commands\CommandSet.h"

void ShaderPipelineStates::Init()
{
	m_ConstantBuffers->Init();	
	m_Samplers->Init();	
	m_Shader->Init();	
	m_ShaderResources->Init();
}

void ShaderPipelineStates::WriteToFile( D3DDeviceWrapper *pWrapper )
{
	m_ConstantBuffers->WriteToFile( pWrapper );
	m_Samplers->WriteToFile( pWrapper );
	m_Shader->WriteToFile( pWrapper );
	m_ShaderResources->WriteToFile( pWrapper );
}

D3DDeviceState::D3DDeviceState()
:	m_IaIndexBuffer( new Commands::IaSetIndexBuffer() )
,	m_IaInputLayout( new Commands::IaSetInputLayout() )
,	m_IaTopology( new Commands::IaSetTopology() )
,	m_IaVertexBuffers( new Commands::IaSetVertexBuffers() )
,	m_RSScissorRects( new Commands::SetScissorRects() )
,	m_RSState( new Commands::SetRasterizerState() )
,	m_RSViewports( new Commands::SetViewports() )
,	m_OMBlendState( new Commands::SetBlendState() )
,	m_OMDepthStencilState( new Commands::SetDepthStencilState() )
,	m_Predication( new Commands::SetPredication() )
,	m_TextFilterSize( new Commands::SetTextFilterSize() )
,	m_SOTargets( new Commands::SoSetTargets() )
,	m_NumViewports ( 0 )
,	m_NumScissorRects ( 0 )
,	m_NumRenderTargets ( 0 )
,	m_NumSOTargets ( 0 )
,	m_IsRTStereo ( false )
,	m_IsDSStereo ( false )
,	m_IsUAVStereo ( false )
{
	GS.m_ConstantBuffers = new Commands::GsSetConstantBuffers();
	GS.m_Samplers = new Commands::GsSetSamplers();
	GS.m_Shader = new Commands::GsSetShader();
	GS.m_ShaderResources = new Commands::GsSetShaderResources();
	PS.m_ConstantBuffers = new Commands::PsSetConstantBuffers();
	PS.m_Samplers = new Commands::PsSetSamplers();
	PS.m_Shader = new Commands::PsSetShader();
	PS.m_ShaderResources = new Commands::PsSetShaderResources();
	VS.m_ConstantBuffers = new Commands::VsSetConstantBuffers();
	VS.m_Samplers = new Commands::VsSetSamplers();
	VS.m_Shader = new Commands::VsSetShader();
	VS.m_ShaderResources = new Commands::VsSetShaderResources();

	GS.Init();
	VS.Init();
	PS.Init();
	m_IaIndexBuffer->Init();
	m_IaInputLayout->Init();
	m_IaTopology->Init();
	m_IaVertexBuffers->Init();
	m_OMBlendState->Init();	
	m_OMDepthStencilState->Init();
	m_Predication->Init();	
	m_RSState->Init();
	m_RSScissorRects->Init();	
	m_TextFilterSize->Init();
	m_RSViewports->Init();	
	m_SOTargets->Init();

}

void D3DDeviceState::Init( DWORD ver )
{
	if ( ver != TD3DVersion_11_0 )
	{
		m_OMRenderTargets = new Commands::SetRenderTargets();
		m_OMRenderTargets->Init();	
	}
	else
	{
		m_OMRenderTargets = new Commands::SetRenderTargets11_0();
		m_OMRenderTargets->Init();	
		m_CsUnorderedAccessViews = new Commands::CsSetUnorderedAccessViews11_0();

		HS.m_ConstantBuffers = new Commands::HsSetConstantBuffers11_0();
		HS.m_Samplers = new Commands::HsSetSamplers11_0();
		HS.m_Shader = new Commands::HsSetShader11_0();
		HS.m_ShaderResources = new Commands::HsSetShaderResources11_0();
		DS.m_ConstantBuffers = new Commands::DsSetConstantBuffers11_0();
		DS.m_Samplers = new Commands::DsSetSamplers11_0();
		DS.m_Shader = new Commands::DsSetShader11_0();
		DS.m_ShaderResources = new Commands::DsSetShaderResources11_0();
		CS.m_ConstantBuffers = new Commands::CsSetConstantBuffers11_0();
		CS.m_Samplers = new Commands::CsSetSamplers11_0();
		CS.m_Shader = new Commands::CsSetShader11_0();
		CS.m_ShaderResources = new Commands::CsSetShaderResources11_0();
		HS.Init();
		DS.Init();
		CS.Init();
		m_CsUnorderedAccessViews->Init();
	}
}

void D3DDeviceState::WriteToFile( D3DDeviceWrapper *pWrapper )
{
	if (!WriteStreamer::IsOK())
		return;

	WriteStreamer::DeviceStateBegin();
		GS.WriteToFile( pWrapper );
		VS.WriteToFile( pWrapper );
		PS.WriteToFile( pWrapper );

		m_IaIndexBuffer->WriteToFile( pWrapper );
		m_IaInputLayout->WriteToFile( pWrapper );
		m_IaTopology->WriteToFile( pWrapper );
		m_IaVertexBuffers->WriteToFile( pWrapper );

		m_RSScissorRects->WriteToFile( pWrapper );
		m_RSState->WriteToFile( pWrapper );
		m_RSViewports->WriteToFile( pWrapper );

		m_OMBlendState->WriteToFile( pWrapper );
		m_OMDepthStencilState->WriteToFile( pWrapper );
		m_OMRenderTargets->WriteToFile( pWrapper );
		m_Predication->WriteToFile( pWrapper );
		m_TextFilterSize->WriteToFile( pWrapper );
		m_SOTargets->WriteToFile( pWrapper );	
	WriteStreamer::DeviceStateEnd();
}

Commands::TCmdPtr D3DDeviceState::GetLastCommand(CommandsId id, bool immutable)
{
	Commands::TCmdPtr pCmd;
	switch(id)
	{
	case idGsSetShader:
		pCmd = GS.m_Shader;
		break;
	case idGsSetConstantBuffers:
		pCmd = immutable ? GS.m_ConstantBuffers->Clone() : GS.m_ConstantBuffers;
		break;
	case idGsSetSamplers:
		pCmd = immutable ? GS.m_Samplers->Clone() : GS.m_Samplers;
		break;
	case idGsSetShaderResources:
		pCmd = immutable ? GS.m_ShaderResources->Clone() : GS.m_ShaderResources;
		break;
	case idPsSetShader:
		pCmd = PS.m_Shader;
		break;
	case idPsSetConstantBuffers:
		pCmd = immutable ? PS.m_ConstantBuffers->Clone() : PS.m_ConstantBuffers;
		break;
	case idPsSetSamplers:
		pCmd = immutable ? PS.m_Samplers->Clone() : PS.m_Samplers;
		break;
	case idPsSetShaderResources:
		pCmd = immutable ? PS.m_ShaderResources->Clone() : PS.m_ShaderResources;
		break;
	case idVsSetShader:
		pCmd = VS.m_Shader;
		break;
	case idVsSetConstantBuffers:
		pCmd = immutable ? VS.m_ConstantBuffers->Clone() : VS.m_ConstantBuffers;
		break;
	case idVsSetSamplers:
		pCmd = immutable ? VS.m_Samplers->Clone() : VS.m_Samplers;
		break;
	case idVsSetShaderResources:
		pCmd = immutable ? VS.m_ShaderResources->Clone() : VS.m_ShaderResources;
		break;
	case idIaSetIndexBuffer:
		pCmd = m_IaIndexBuffer;
		break;
	case idIaSetInputLayout:
		pCmd = m_IaInputLayout;
		break;
	case idIaSetTopology:
		pCmd = m_IaTopology;
		break;
	case idIaSetVertexBuffers:
		pCmd = immutable ? m_IaVertexBuffers->Clone() : m_IaVertexBuffers;
		break;
	case idSetRasterizerState:
		pCmd = m_RSState;
		break;
	case idSetViewports:
		pCmd = m_RSViewports;
		break;
	case idSetScissorRects:
		pCmd = m_RSScissorRects;
		break;
	case idSetBlendState:
		pCmd = m_OMBlendState;
		break;
	case idSetDepthStencilState:
		pCmd = m_OMDepthStencilState;
		break;
	case idSetRenderTargets:
	case idSetRenderTargets11_0:
		pCmd = immutable ? m_OMRenderTargets->Clone() : m_OMRenderTargets;
		break;
	case idSetPredication:
		pCmd = m_Predication;
		break;
	case idSetTextFilterSize:
		pCmd = m_TextFilterSize;
		break;
	case idSoSetTargets:
		pCmd = m_SOTargets;
		break;
	case idHsSetShader11_0:
		pCmd = HS.m_Shader;
		break;
	case idHsSetConstantBuffers11_0:
		pCmd = immutable ? HS.m_ConstantBuffers->Clone() : HS.m_ConstantBuffers;
		break;
	case idHsSetSamplers11_0:
		pCmd = immutable ? HS.m_Samplers->Clone() : HS.m_Samplers;
		break;
	case idHsSetShaderResources11_0:
		pCmd = immutable ? HS.m_ShaderResources->Clone() : HS.m_ShaderResources;
		break;
	case idDsSetShader11_0:
		pCmd = DS.m_Shader;
		break;
	case idDsSetConstantBuffers11_0:
		pCmd = immutable ? DS.m_ConstantBuffers->Clone() : DS.m_ConstantBuffers;
		break;
	case idDsSetSamplers11_0:
		pCmd = immutable ? DS.m_Samplers->Clone() : DS.m_Samplers;
		break;
	case idDsSetShaderResources11_0:
		pCmd = immutable ? DS.m_ShaderResources->Clone() : DS.m_ShaderResources;
		break;
	case idCsSetShader11_0:
		pCmd = CS.m_Shader;
		break;
	case idCsSetConstantBuffers11_0:
		pCmd = immutable ? CS.m_ConstantBuffers->Clone() : CS.m_ConstantBuffers;
		break;
	case idCsSetSamplers11_0:
		pCmd = immutable ? CS.m_Samplers->Clone() : CS.m_Samplers;
		break;
	case idCsSetShaderResources11_0:
		pCmd = immutable ? CS.m_ShaderResources->Clone() : CS.m_ShaderResources;
		break;
	case idCsSetUnorderedAccessViews11_0:
		pCmd = m_CsUnorderedAccessViews;
		break;
	default:
		_ASSERT(false);
		pCmd = NULL;
		break;
	}
	return pCmd;
}
