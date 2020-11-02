#pragma once

#include "xxSet.hpp"

namespace Commands
{
	class xxSetSamplers : 
		public xxSet<D3D10DDI_HSAMPLER>
	{
	public:
		typedef xxSet<D3D10DDI_HSAMPLER> base_type;

	public:
		xxSetSamplers( SHADER_PIPELINE ShaderPipeline )
			:	ShaderPipeline_(ShaderPipeline)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		xxSetSamplers( SHADER_PIPELINE			ShaderPipeline,
					   UINT						Offset, 
					   UINT						NumSamplers, 
					   CONST D3D10DDI_HSAMPLER* phSamplers )
		:	base_type(Offset, NumSamplers, phSamplers)
		,	ShaderPipeline_(ShaderPipeline)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		virtual void Init()
		{
			Offset_ = 0;
			NumValues_ = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT;
			Values_.assign(NumValues_, NULL_SAMPLER);
		}
		
		virtual TSetSamplersCmdPtr Clone() const = 0;

		// Override xxSet
		virtual bool IsFull() const override
		{
			return (NumValues_ == D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
		}

		bool WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const;

		// Override Command
		virtual bool ReadFromFile() override;
		virtual void UpdateDeviceState( D3DDeviceWrapper* pWrapper, D3DDeviceState* pState ) override;

		void BuildCommand(CDumpState *ds);

	public:
		SHADER_PIPELINE	ShaderPipeline_;

		const char Char()
		{
			return GetShaderPipelineChar(ShaderPipeline_);
		}

		DEPENDS_ON_VECTOR(Values_);

	protected:
		static const D3D10DDI_HSAMPLER NULL_SAMPLER;
	};

}
