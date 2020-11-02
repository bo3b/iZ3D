#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyCommandList11_0 : public CommandWithAllocator<DestroyCommandList11_0>
	{
	public:
		EMPTY_OUTPUT();

		DestroyCommandList11_0()		
		{
			CommandId = idDestroyCommandList11_0;
		}

		DestroyCommandList11_0( D3D11DDI_HCOMMANDLIST  hCommandList )
		{
			CommandId = idDestroyCommandList11_0;
			hCommandList_ = hCommandList;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

	public:
		D3D11DDI_HCOMMANDLIST  hCommandList_;
	};

}

extern VOID ( APIENTRY DestroyCommandList11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HCOMMANDLIST  hCommandList );
