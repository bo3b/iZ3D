#pragma once

#include "Command.h"

namespace Commands
{

	class QueryEnd : public CommandWithAllocator<QueryEnd>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		QueryEnd()
		{
			CommandId = idQueryEnd;
		}

		QueryEnd( D3D10DDI_HQUERY		hQuery )
		{
			CommandId = idQueryEnd;
			hQuery_			= hQuery;
		}

	public:

		D3D10DDI_HQUERY		hQuery_;
	};

}

extern VOID ( APIENTRY QueryEnd )(
								  D3D10DDI_HDEVICE  hDevice, 
								  D3D10DDI_HQUERY  hQuery
								  );
