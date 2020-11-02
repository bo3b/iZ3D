#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateResourceSize : public CommandWithAllocator<CalcPrivateResourceSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateResourceSize ()		
		{
			CommandId = idCalcPrivateResourceSize;
		}

		CalcPrivateResourceSize(const D3D10DDIARG_CREATERESOURCE* pCreateResource)
		{
			CommandId = idCalcPrivateResourceSize;
			memcpy(&CreateResource_, pCreateResource, sizeof(D3D10DDIARG_CREATERESOURCE));
		}

	public:

		D3D10DDIARG_CREATERESOURCE  CreateResource_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateResourceSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATERESOURCE*  pCreateResource );
