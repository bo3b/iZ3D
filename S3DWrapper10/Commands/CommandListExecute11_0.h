#pragma once

#include "Command.h"

namespace Commands
{

	class CommandListExecute11_0 : public CommandWithAllocator<CommandListExecute11_0>
	{
	public:

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile();

		CommandListExecute11_0( )
		{
			CommandId = idCommandListExecute11_0;
		}
		CommandListExecute11_0( D3D11DDI_HCOMMANDLIST  hCommandList )
		{
			CommandId = idCommandListExecute11_0;
			hCommandList_ = hCommandList;
		}

	public:

		D3D11DDI_HCOMMANDLIST  hCommandList_;
	};

}

extern VOID ( APIENTRY CommandListExecute11_0 )( D3D10DDI_HDEVICE  hDevice, 
												D3D11DDI_HCOMMANDLIST  hCommandList  );
