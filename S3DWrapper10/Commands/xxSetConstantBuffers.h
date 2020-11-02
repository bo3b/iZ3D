#pragma once

#include "xxSet.hpp"

namespace Commands
{

	class xxSetConstantBuffers : 
		public xxSet<D3D10DDI_HRESOURCE>
	{
	public:
		typedef xxSet<D3D10DDI_HRESOURCE> base_type;

	public:
		xxSetConstantBuffers(SHADER_PIPELINE ShaderPipeline)
			:	ShaderPipeline_(ShaderPipeline)
			,	bIsUsedStereoResources_(false)
			,	bNeedCheckStereoResources_(false)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}

		xxSetConstantBuffers( SHADER_PIPELINE			ShaderPipeline,
							  UINT						StartBuffer, 
							  UINT						NumBuffers, 
							  CONST D3D10DDI_HRESOURCE* phBuffers)
		:	base_type(StartBuffer, NumBuffers, phBuffers)
		,	ShaderPipeline_(ShaderPipeline)
		,	bIsUsedStereoResources_(false)
		,	bNeedCheckStereoResources_(true)
		{
			State_ = COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}

		virtual TSetConstantBuffersCmdPtr Clone() const = 0;
		
		bool WriteToFileEx( D3DDeviceWrapper* pWrapper, const char* szCommandName ) const;

		template<typename T>
		void SetMatrices( D3DDeviceWrapper* pWrapper, const T* pShaderData, DWORD shaderCRC32 );

		// Override xxSet
		virtual void	CheckIsStereo(D3DDeviceWrapper* /*pWrapper*/) override { bNeedCheckStereoResources_ = true; }
		virtual bool	IsFull() const override								   { return NumValues_ == D3D10_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT; }

		// Override Command
		virtual void	Init() override;
		virtual bool	ReadFromFile() override;
		virtual void	UpdateDeviceState( D3DDeviceWrapper* pWrapper, D3DDeviceState* pState ) override;
		virtual bool	IsUsedStereoResources(D3DDeviceWrapper* pWrapper) const override;

		void BuildCommand(CDumpState *ds);
		
	public:
		SHADER_PIPELINE	ShaderPipeline_;
		mutable bool	bIsUsedStereoResources_;
		mutable bool	bNeedCheckStereoResources_;
		
		const char Char()
		{
			return GetShaderPipelineChar(ShaderPipeline_);
		}

	protected:
		static const D3D10DDI_HRESOURCE	NULL_RESOURCE;
		static D3D10DDI_HRESOURCE		phTempBuffers_[D3D10_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT];

		DEPENDS_ON_VECTOR(Values_);
	};

}
