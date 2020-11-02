#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateQuerySize : public CommandWithAllocator<CalcPrivateQuerySize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool ReadFromFile();

		CalcPrivateQuerySize ()		
		{
			CommandId = idCalcPrivateQuerySize;
		}

		CalcPrivateQuerySize( CONST D3D10DDIARG_CREATEQUERY*  pCreateQuery )
		{
			CommandId = idCalcPrivateQuerySize;
			memcpy(&CreateQuery_, pCreateQuery, sizeof(D3D10DDIARG_CREATEQUERY));
		}

	public:

		D3D10DDIARG_CREATEQUERY  CreateQuery_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateQuerySize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATEQUERY*  pCreateQuery );
