#pragma once

#include "xxSetRenderTargets.h"

namespace Commands
{

	class SetRenderTargets : public CommandWithAllocator<SetRenderTargets, xxSetRenderTargets>
	{
	public:
		SetRenderTargets()
		{
			CommandId = idSetRenderTargets;
		}

		SetRenderTargets(CONST D3D10DDI_HRENDERTARGETVIEW*  phRenderTargetView, 
						 UINT								RTargets, 
						 UINT								ClearTargets, 
						 D3D10DDI_HDEPTHSTENCILVIEW			hDepthStencilView)
		:	base_type(phRenderTargetView, RTargets, ClearTargets, hDepthStencilView)
		{
			CommandId = idSetRenderTargets;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		TSetRenderTargetsCmdPtr Clone() const { return TSetRenderTargetsCmdPtr( new SetRenderTargets(NumValues_ > 0 ? &Values_[0] : 0, NumValues_, ClearTargets_, hDepthStencilView_) ) ;}

		DEPENDS_ON_SCALARVECTOR(hDepthStencilView_,Values_);
	};

}

extern VOID ( APIENTRY SetRenderTargets )( D3D10DDI_HDEVICE  hDevice,  CONST D3D10DDI_HRENDERTARGETVIEW*  phRenderTargetView, 
										  UINT  RTargets, UINT  ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView );
