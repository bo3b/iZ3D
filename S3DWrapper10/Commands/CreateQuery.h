#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateQuery : public CommandWithAllocator<CreateQuery>
	{
	public:
		EMPTY_OUTPUT();

		CreateQuery()		
		{
			CommandId = idCreateQuery;
		}

		CreateQuery(const D3D10DDIARG_CREATEQUERY*  pCreateQuery, 
			D3D10DDI_HQUERY  hQuery, 
			D3D10DDI_HRTQUERY  hRTQuery)
		{
			CommandId = idCreateQuery;
			memcpy(&CreateQuery_, pCreateQuery, sizeof(D3D10DDIARG_CREATEQUERY));

			hQuery_ = hQuery;
			hRTQuery_ = hRTQuery;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

	public:

		D3D10DDIARG_CREATEQUERY   CreateQuery_; 
		D3D10DDI_HQUERY           hQuery_; 
		D3D10DDI_HRTQUERY         hRTQuery_;
	};

}

VOID (APIENTRY  CreateQuery)(D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATEQUERY*  pCreateQuery, 
							 D3D10DDI_HQUERY  hQuery, D3D10DDI_HRTQUERY  hRTQuery);
