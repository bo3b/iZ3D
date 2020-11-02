#pragma once

#include "Command.h"

namespace Commands
{

	class IaSetInputLayout : public CommandWithAllocator<IaSetInputLayout>
	{
	public:
		IaSetInputLayout()		
		{
			CommandId = idIaSetInputLayout;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		IaSetInputLayout( D3D10DDI_HELEMENTLAYOUT  hInputLayout )
		{
			CommandId = idIaSetInputLayout;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hInputLayout_ = hInputLayout;
		}

		virtual void Init()
		{
			hInputLayout_ = MAKE_D3D10DDI_HELEMENTLAYOUT(NULL);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HELEMENTLAYOUT  hInputLayout_;

		DEPENDS_ON(hInputLayout_);
	};

}

extern VOID ( APIENTRY IaSetInputLayout )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HELEMENTLAYOUT  hInputLayout );
