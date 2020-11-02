#pragma once

#include "Command.h"

namespace Commands
{

	class QueryGetData : public CommandWithAllocator<QueryGetData>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		QueryGetData()
			:	pData_		( NULL )
		{
			CommandId = idQueryGetData;
		}

		QueryGetData(
			D3D10DDI_HQUERY		hQuery,
			VOID*				pData, 
			UINT				DataSize, 
			UINT				Flags
			)
		{
			CommandId = idQueryGetData;
			hQuery_			= hQuery;
			pData_			= pData; 
			DataSize_		= DataSize; 
			Flags_			= Flags;
		}

	public:

		D3D10DDI_HQUERY		hQuery_;
		VOID*				pData_; 
		UINT				DataSize_; 
		UINT				Flags_;
	};

}

extern VOID ( APIENTRY QueryGetData )(
									  D3D10DDI_HDEVICE  hDevice, 
									  D3D10DDI_HQUERY  hQuery, 
									  VOID*  pData, 
									  UINT  DataSize, 
									  UINT  Flags
									  );
