#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{

	class DrawAuto : public CommandWithAllocator<DrawAuto,DrawBase>
	{
	public:
		DrawAuto() 
		{ 
			CommandId = idDrawAuto; 		
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );

		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool	ReadFromFile();

		void BuildCommand(CDumpState *ds);

		UINT64 vertexCount;
	};

}

VOID ( APIENTRY DrawAuto )( D3D10DDI_HDEVICE hDevice );
