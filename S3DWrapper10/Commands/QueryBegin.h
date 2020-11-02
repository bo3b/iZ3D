#pragma once

#include "Command.h"

namespace Commands
{

	class QueryBegin : public CommandWithAllocator<QueryBegin>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		QueryBegin()
		{
			CommandId = idQueryBegin;
		}

		QueryBegin( D3D10DDI_HQUERY		hQuery )
		{
			CommandId = idQueryBegin;
			hQuery_			 = hQuery;
		}

	public:

		D3D10DDI_HQUERY		hQuery_;
	};

}

extern VOID ( APIENTRY QueryBegin )(
									D3D10DDI_HDEVICE  hDevice, 
									D3D10DDI_HQUERY  hQuery
									);

