#pragma once

#include "Command.h"

namespace Commands
{

	class RelocateDeviceFuncs10_1 : public CommandWithAllocator<RelocateDeviceFuncs10_1>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		RelocateDeviceFuncs10_1()
		{
			CommandId = idRelocateDeviceFuncs10_1;
		}

		RelocateDeviceFuncs10_1( struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions )
		{
			CommandId = idRelocateDeviceFuncs10_1;
			pDeviceFunctions_ = pDeviceFunctions;
		}

	public:

		struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions_;
	};

}

extern VOID ( APIENTRY RelocateDeviceFuncs10_1 )( D3D10DDI_HDEVICE  hDevice,  struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions );
