#pragma once

class CommandBuffer;

#include "Commands\Command.h"
#include "Commands\xxSetConstantBuffers.h"
#include "Commands\xxSetSamplers.h"
#include "Commands\xxSetShaderResources.h"
#include "Commands\CsSetUnorderedAccessViews11_0.h"

class ShaderPipelineStates
{
public:
	ShaderPipelineStates()
	:	m_IsStereoShader(false)
	{}

	void Init();
	void WriteToFile( D3DDeviceWrapper *pWrapper );
	
	Commands::TCmdPtr					m_Shader;
	Commands::TSetConstantBuffersCmdPtr	m_ConstantBuffers;
	Commands::TSetSamplersCmdPtr		m_Samplers;
	Commands::TSetShaderResourcesCmdPtr	m_ShaderResources;
	bool								m_IsStereoShader;
};

class D3DDeviceState
{
public:
	ShaderPipelineStates GS;
	ShaderPipelineStates PS;
	ShaderPipelineStates VS;
	ShaderPipelineStates HS;
	ShaderPipelineStates DS;
	ShaderPipelineStates CS;

	Commands::TCsSetUnorderedAccessViewsCmdPtr	m_CsUnorderedAccessViews;

	Commands::TCmdPtr					m_IaIndexBuffer;
	Commands::TCmdPtr					m_IaInputLayout;
	Commands::TCmdPtr					m_IaTopology;
	Commands::TIaSetVertexBuffersCmdPtr	m_IaVertexBuffers;

	Commands::TCmdPtr					m_RSState;
	Commands::TCmdPtr					m_RSViewports;
	UINT								m_NumViewports;

	Commands::TCmdPtr					m_RSScissorRects;
	UINT								m_NumScissorRects;

	Commands::TCmdPtr					m_OMBlendState;
	Commands::TCmdPtr					m_OMDepthStencilState;	
	Commands::TSetRenderTargetsCmdPtr	m_OMRenderTargets;
	UINT								m_NumRenderTargets;

	Commands::TCmdPtr	m_Predication;
	Commands::TCmdPtr	m_TextFilterSize;
	Commands::TCmdPtr	m_SOTargets;
	UINT				m_NumSOTargets;

	bool				m_IsRTStereo;
	bool				m_IsDSStereo;
	bool				m_IsUAVStereo;

	D3DDeviceState();
	void Init(DWORD ver);
	void WriteToFile( D3DDeviceWrapper *pWrapper );

	ShaderPipelineStates* GetShaderPipeline(SHADER_PIPELINE shader)
	{
		switch(shader)
		{
		case SP_VS:
			return &VS;
		case SP_GS:
			return &GS;
		case SP_DS:
			return DS.m_ConstantBuffers ? &DS : 0; // return NULL if DX10
		case SP_HS:
			return HS.m_ConstantBuffers ? &HS : 0; // return NULL if DX10
		case SP_PS:
			return &PS;
		case SP_CS:
			return CS.m_ConstantBuffers ? &CS : 0; // return NULL if DX10
		}
		return NULL;
	}

	Commands::TCmdPtr GetLastCommand(CommandsId id, bool immutable = false);
};
