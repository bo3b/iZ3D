#pragma once

#include "Command.h"

namespace Commands
{

	class SetRasterizerState : public CommandWithAllocator<SetRasterizerState>
	{
	public:
		SetRasterizerState()
		{
			CommandId = idSetRasterizerState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetRasterizerState( D3D10DDI_HRASTERIZERSTATE  hRasterizerState )
		{
			CommandId = idSetRasterizerState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hRasterizerState_ = hRasterizerState;
		}

		virtual void Init()
		{
			hRasterizerState_ = MAKE_D3D10DDI_HRASTERIZERSTATE(NULL);
		}

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HRASTERIZERSTATE  hRasterizerState_;

		DEPENDS_ON(hRasterizerState_);
	};

}

extern VOID ( APIENTRY SetRasterizerState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRASTERIZERSTATE  hRasterizerState );
