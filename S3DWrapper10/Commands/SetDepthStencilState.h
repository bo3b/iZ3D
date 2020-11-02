#pragma once

#include "Command.h"

namespace Commands
{

	class SetDepthStencilState : public CommandWithAllocator<SetDepthStencilState>
	{
	public:
		SetDepthStencilState()
		{
			CommandId = idSetDepthStencilState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetDepthStencilState(     
			D3D10DDI_HDEPTHSTENCILSTATE  hState, 
			UINT  StencilRef
			)
		{
			CommandId = idSetDepthStencilState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hState_ = hState;
			StencilRef_ = StencilRef;
		}

		virtual void Init()
		{
			hState_ = MAKE_D3D10DDI_HDEPTHSTENCILSTATE(NULL);
			StencilRef_ = 0;
		}

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HDEPTHSTENCILSTATE  hState_; 
		UINT  StencilRef_;

		DEPENDS_ON(hState_);
	};

}

extern VOID ( APIENTRY SetDepthStencilState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILSTATE  hState, 
											  UINT  StencilRef );
