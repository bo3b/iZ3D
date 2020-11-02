#pragma once

#include "Command.h"

namespace Commands
{

	class RelocateDeviceFuncs : public CommandWithAllocator<RelocateDeviceFuncs>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		RelocateDeviceFuncs()
		:	pDeviceFunctions_(0)
		{
			CommandId = idRelocateDeviceFuncs;
		}

		RelocateDeviceFuncs( struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions )
		{
			CommandId = idRelocateDeviceFuncs;
			pDeviceFunctions_ = pDeviceFunctions;
		}

	public:

		struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions_;
	};

}

extern VOID ( APIENTRY RelocateDeviceFuncs )( D3D10DDI_HDEVICE  hDevice,  struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions );
