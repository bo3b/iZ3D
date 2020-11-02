#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyDevice : public CommandWithAllocator<DestroyDevice>
	{
	public:
		EMPTY_OUTPUT();

		DestroyDevice()
		{
			CommandId = idDestroyDevice;
		}

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool ReadFromFile	();
	};

}

extern VOID ( APIENTRY DestroyDevice)( D3D10DDI_HDEVICE  hDevice );
