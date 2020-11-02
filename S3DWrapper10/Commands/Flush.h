#pragma once

#include "Command.h"

namespace Commands
{

	class Flush : public CommandWithAllocator<Flush>
	{
	public:

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile();

		Flush( )
		{
			CommandId = idFlush;
		}
	};

}

extern VOID ( APIENTRY Flush )( D3D10DDI_HDEVICE  hDevice );
