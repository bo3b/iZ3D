#pragma once

#include "xxSetRenderTargets.h"

namespace Commands
{

	class SetRenderTargets11_0 : public CommandWithAllocator<SetRenderTargets11_0, xxSetRenderTargets>
	{
	public:
		typedef fast_vector<D3D11DDI_HUNORDEREDACCESSVIEW>::type	handle_vector;
		typedef fast_vector<UINT>::type								uint_vector;

	public:
		SetRenderTargets11_0()
		{
			CommandId = idSetRenderTargets11_0;
		}

		SetRenderTargets11_0(CONST D3D10DDI_HRENDERTARGETVIEW*		phRenderTargetView, 
							 UINT									RTargets, 
							 UINT									ClearTargets, 
							 D3D10DDI_HDEPTHSTENCILVIEW				hDepthStencilView,
							 CONST D3D11DDI_HUNORDEREDACCESSVIEW*	phUnorderedAccessView,  
							 CONST UINT*							pUAV, 
							 UINT									UAVIndex, 
							 UINT									NumUAVs, 
							 UINT									UAVFirsttoSet, 
							 UINT									UAVNumberUpdated)
		:	base_type(phRenderTargetView, RTargets, ClearTargets, hDepthStencilView)
		,	UAVIndex_(UAVIndex)
		,	NumUAVs_(NumUAVs)
		,	UAVFirsttoSet_(UAVFirsttoSet)
		,	UAVNumberUpdated_(UAVNumberUpdated)
		,	phUnorderedAccessView_(phUnorderedAccessView, phUnorderedAccessView + NumUAVs)
		,	pUAVInitialCounts_(pUAV, pUAV + NumUAVs)
		{
			CommandId = idSetRenderTargets11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );

		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual void Init()
		{
			xxSetRenderTargets::Init();
			phUnorderedAccessView_.clear();
			pUAVInitialCounts_.clear();
			UAVIndex_ = 0;
			NumUAVs_ = 0;
			UAVFirsttoSet_ = 0;
			UAVNumberUpdated_ = 0;
		}
		
		TSetRenderTargetsCmdPtr Clone() const 
		{ 
			return TSetRenderTargetsCmdPtr( new SetRenderTargets11_0( NumValues_ > 0 ? &Values_[0] : 0, 
																	  NumValues_, 
																	  ClearTargets_, 
																	  hDepthStencilView_, 
																	  phUnorderedAccessView_.empty() ? 0 : &phUnorderedAccessView_[0], 
																	  pUAVInitialCounts_.empty() ? 0 : &pUAVInitialCounts_[0], 
																	  UAVIndex_, 
																	  NumUAVs_, 
																	  UAVFirsttoSet_, 
																	  UAVNumberUpdated_ ) );
		}

	public:
		UINT			UAVIndex_;
		UINT			NumUAVs_;
		UINT			UAVFirsttoSet_;
		UINT			UAVNumberUpdated_;
		handle_vector	phUnorderedAccessView_;
		uint_vector		pUAVInitialCounts_;		

		DEPENDS_ON_SCALARVECTOR2(hDepthStencilView_,Values_,phUnorderedAccessView_);

	protected:
		static D3D11DDI_HUNORDEREDACCESSVIEW phTempUnorderedAccessView_[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
	};

}

extern VOID ( APIENTRY SetRenderTargets11_0 )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDI_HRENDERTARGETVIEW*  phRenderTargetView, 
			UINT  RTargets,  UINT  ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStecilView,
			CONST D3D11DDI_HUNORDEREDACCESSVIEW*  phUnorderedAccessView,  CONST UINT*  pUAV, 
			UINT  UAVIndex, UINT  NumUAVs, UINT  UAVFirsttoSet, UINT  UAVNumberUpdated );
