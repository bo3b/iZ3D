#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyQuery : public CommandWithAllocator<DestroyQuery>
	{
	public:
		EMPTY_OUTPUT();

		DestroyQuery()		
		{
			CommandId = idDestroyQuery;
		}

		DestroyQuery( D3D10DDI_HQUERY hQuery )
		{
			CommandId = idDestroyQuery;
			hQuery_ = hQuery;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HQUERY	hQuery_;
	};

}

extern VOID ( APIENTRY DestroyQuery )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery );
