#pragma once

#include "Command.h"

namespace Commands
{

	class AbandonCommandList11_0 : public CommandWithAllocator<AbandonCommandList11_0>
	{
	public:

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile();

		AbandonCommandList11_0( )
		{
			CommandId = idAbandonCommandList11_0;
		}
	};

}

extern VOID ( APIENTRY AbandonCommandList11_0 )( D3D10DDI_HDEVICE  hDevice );
