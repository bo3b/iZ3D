#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyUnorderedAccessView11_0 : public CommandWithAllocator<DestroyUnorderedAccessView11_0>
	{
	public:
		EMPTY_OUTPUT();

		DestroyUnorderedAccessView11_0()		
		{
			CommandId = idDestroyUnorderedAccessView11_0;
		}

		DestroyUnorderedAccessView11_0( D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView )
		{
			CommandId = idDestroyUnorderedAccessView11_0;
			hUnorderedAccessView_ = hUnorderedAccessView;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView_;
	};

}

extern VOID ( APIENTRY DestroyUnorderedAccessView11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView );
