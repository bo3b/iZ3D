#pragma once

#include "xxSet.hpp"

namespace Commands
{

	class xxSetRenderTargets : 
		public xxSet<D3D10DDI_HRENDERTARGETVIEW>
	{
	public:
		typedef xxSet<D3D10DDI_HRENDERTARGETVIEW> base_type;

	public:
		xxSetRenderTargets()
		:	bNeedCheckStereoResources_(false)
		,	bIsRTStereo_(false)
		,	bIsDSStereo_(false)
		,	ClearTargets_(0)
		,	hDepthStencilView_(NULL_DEPTH_STENCIL_VIEW)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}

		xxSetRenderTargets( CONST D3D10DDI_HRENDERTARGETVIEW*	phRenderTargetView, 
							UINT								RTargets, 
							UINT								ClearTargets, 
							D3D10DDI_HDEPTHSTENCILVIEW			hDepthStencilView )
		:	base_type(0, RTargets, phRenderTargetView)
		,	bNeedCheckStereoResources_(true)
		,	ClearTargets_(ClearTargets)
		,	hDepthStencilView_(hDepthStencilView)
		{
			State_	= COMMAND_CHANGE_DEVICE_STATE | COMMAND_CHANGE_DEVICE_STEREO_STATE;
		}
		
		// Override xxSet
		virtual void CheckIsStereo(D3DDeviceWrapper* /*pWrapper*/) override { bNeedCheckStereoResources_ = true; }
		virtual bool IsFull() const override								   { return NumValues_ == D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT; }
		virtual void MergeCommands( D3DDeviceWrapper* pWrapper, const base_type* pCmd, const xxSet* pFullCmd = 0 ) override;

		// Override Command
		virtual void Init() override;
		virtual void UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState ) override;
		virtual bool IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const override;
		virtual void SetDestResourceType( TextureType type ) override;
		virtual void GetDestResources(DestResourcesSet &res) override;
				
		virtual TSetRenderTargetsCmdPtr Clone() const = 0;

		void BuildCommand(CDumpState *ds);

	public:
		mutable bool				bNeedCheckStereoResources_;
		mutable bool				bIsRTStereo_;
		mutable bool				bIsDSStereo_;
		UINT						ClearTargets_;
		D3D10DDI_HDEPTHSTENCILVIEW	hDepthStencilView_;

	protected:
		static const D3D10DDI_HRENDERTARGETVIEW	NULL_RENDER_TARGET_VIEW;
		static const D3D10DDI_HDEPTHSTENCILVIEW	NULL_DEPTH_STENCIL_VIEW;
		static D3D10DDI_HRENDERTARGETVIEW		phTempRenderTargetView_[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
	};

}
