#pragma once

#include "Command.h"

namespace Commands
{

	class RelocateDeviceFuncs11_0 : public CommandWithAllocator<RelocateDeviceFuncs11_0>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		RelocateDeviceFuncs11_0()
		{
			CommandId = idRelocateDeviceFuncs11_0;
		}

		RelocateDeviceFuncs11_0( struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions )
		{
			CommandId = idRelocateDeviceFuncs11_0;
			pDeviceFunctions_ = pDeviceFunctions;
		}

	public:

		struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions_;
	};

}

extern VOID ( APIENTRY RelocateDeviceFuncs11_0 )( D3D10DDI_HDEVICE  hDevice,  struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions );

