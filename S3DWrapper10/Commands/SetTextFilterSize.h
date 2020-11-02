#pragma once

#include "Command.h"

namespace Commands
{

	class SetTextFilterSize : public CommandWithAllocator<SetTextFilterSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;	
		virtual bool	ReadFromFile	();

		SetTextFilterSize()		
		{
			CommandId = idSetTextFilterSize;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetTextFilterSize(     
			UINT  Width, 
			UINT  Height
			)
		{
			CommandId = idSetTextFilterSize;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			Width_ = Width;
			Height_ = Height;
		}

		virtual void Init()
		{
			Width_ = 0;
			Height_ = 0;
		}

		virtual void		UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

	public:

		UINT  Width_; 
		UINT  Height_;   
	};

}

VOID (APIENTRY  SetTextFilterSize)(D3D10DDI_HDEVICE  hDevice, UINT  Width, UINT  Height );
