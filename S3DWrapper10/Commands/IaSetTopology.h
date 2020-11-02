#pragma once

#include "Command.h"

namespace Commands
{

	class IaSetTopology : public CommandWithAllocator<IaSetTopology>
	{
	public:
		IaSetTopology()
		{
			CommandId = idIaSetTopology;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		IaSetTopology( D3D10_DDI_PRIMITIVE_TOPOLOGY  PrimitiveTopology )
		{
			CommandId = idIaSetTopology;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			PrimitiveTopology_ = PrimitiveTopology;
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;	
		virtual bool	ReadFromFile();

		virtual void Init()
		{
			PrimitiveTopology_ = D3D10_DDI_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}

		virtual void		UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10_DDI_PRIMITIVE_TOPOLOGY  PrimitiveTopology_;
	};

}


extern VOID ( APIENTRY IaSetTopology )( D3D10DDI_HDEVICE  hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY  PrimitiveTopology );
