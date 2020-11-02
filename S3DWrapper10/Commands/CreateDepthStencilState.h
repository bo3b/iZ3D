#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateDepthStencilState : public CommandWithAllocator<CreateDepthStencilState>
	{
	public:
		CreateDepthStencilState()
		{
			CommandId	  = idCreateDepthStencilState;
		}

		CreateDepthStencilState(const D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc, 
			D3D10DDI_HDEPTHSTENCILSTATE  hDepthStencilState, 
			D3D10DDI_HRTDEPTHSTENCILSTATE  hRTDepthStencilState)
		{
			CommandId	  = idCreateDepthStencilState;
			memcpy(&DepthStencilDesc_, pDepthStencilDesc, sizeof(D3D10_DDI_DEPTH_STENCIL_DESC));

			hDepthStencilState_ = hDepthStencilState;
			hRTDepthStencilState_ = hRTDepthStencilState;
		}

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile();

		void BuildCommand(CDumpState *ds);

	public:
		D3D10_DDI_DEPTH_STENCIL_DESC  DepthStencilDesc_; 
		D3D10DDI_HDEPTHSTENCILSTATE  hDepthStencilState_; 
		D3D10DDI_HRTDEPTHSTENCILSTATE  hRTDepthStencilState_;

		CREATES(hDepthStencilState_);
	};

}

VOID ( APIENTRY CreateDepthStencilState )( D3D10DDI_HDEVICE  hDevice, 
										  CONST D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc, 
										  D3D10DDI_HDEPTHSTENCILSTATE  hDepthStencilState, 
										  D3D10DDI_HRTDEPTHSTENCILSTATE  hRTDepthStencilState );
