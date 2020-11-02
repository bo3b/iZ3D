#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateOpenedResourceSize : public CommandWithAllocator<CalcPrivateOpenedResourceSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool ReadFromFile();

		CalcPrivateOpenedResourceSize ()		
		{
			CommandId = idCalcPrivateOpenedResourceSize;
		}

		CalcPrivateOpenedResourceSize(const D3D10DDIARG_OPENRESOURCE* pOpenResource)
		{
			CommandId = idCalcPrivateOpenedResourceSize;
			memcpy(&OpenResource_, pOpenResource, sizeof( D3D10DDIARG_OPENRESOURCE ));
		}

	public:

		D3D10DDIARG_OPENRESOURCE  OpenResource_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateOpenedResourceSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource );
