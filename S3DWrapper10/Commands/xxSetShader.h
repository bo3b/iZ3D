#pragma once

#include "Command.h"

namespace Commands
{

	class xxSetShader : public Command
	{
	public:

		bool	WriteToFileEx		( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const;
		virtual bool	ReadFromFile	();

		xxSetShader( SHADER_PIPELINE ShaderPipeline )
			:	ShaderPipeline_(ShaderPipeline)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		xxSetShader( SHADER_PIPELINE ShaderPipeline, D3D10DDI_HSHADER  hShader )
			:	ShaderPipeline_(ShaderPipeline)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hShader_ = hShader;
		}

		virtual void Init()
		{
			hShader_ = MAKE_D3D10DDI_HSHADER(NULL);
		}

		void BuildCommand(CDumpState *ds);

	public:
		D3D10DDI_HSHADER hShader_;

		DEPENDS_ON(hShader_);

	public:
		SHADER_PIPELINE	ShaderPipeline_;

		const char Char()
		{
			return GetShaderPipelineChar(ShaderPipeline_);
		}
	};

}
