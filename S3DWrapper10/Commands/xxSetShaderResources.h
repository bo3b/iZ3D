#pragma once

#include "xxSet.hpp"

namespace Commands
{

	class xxSetShaderResources : 
		public xxSet<D3D10DDI_HSHADERRESOURCEVIEW>
	{
	public:
		typedef xxSet<D3D10DDI_HSHADERRESOURCEVIEW> base_type;

	public:
		xxSetShaderResources(SHADER_PIPELINE ShaderPipeline)
		:	ShaderPipeline_(ShaderPipeline)
		,	bIsUsedStereoResources_(false)
		,	bNeedCheckStereoResources_(false)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}

		xxSetShaderResources( SHADER_PIPELINE						ShaderPipeline,
							  UINT									Offset, 
							  UINT									NumViews, 
							  CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews )
		:	base_type(Offset, NumViews, phShaderResourceViews)
		,	ShaderPipeline_(ShaderPipeline)
		,	bIsUsedStereoResources_(false)
		,	bNeedCheckStereoResources_(true)
		{
			State_ = COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}

		virtual TSetShaderResourcesCmdPtr Clone() const = 0;
		
		bool WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const;

		// Override xxSet
		virtual void	CheckIsStereo(D3DDeviceWrapper* /*pWrapper*/) override { bNeedCheckStereoResources_ = true; }
		virtual bool	IsFull() const override								   { return (NumValues_ == D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT); }

		// Override Command
		virtual void	Init() override;
		virtual bool	ReadFromFile() override;
		virtual void	UpdateDeviceState(D3DDeviceWrapper* pWrapper, D3DDeviceState* pState) override;
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
		static const D3D10DDI_HSHADERRESOURCEVIEW	NULL_RESOURCE_VIEW;
		static D3D10DDI_HSHADERRESOURCEVIEW			phTempShaderResourceViews_[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

		DEPENDS_ON_VECTOR(Values_);
	};

}
