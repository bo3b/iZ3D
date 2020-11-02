#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateResourceSize11_0 : public CommandWithAllocator<CalcPrivateResourceSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateResourceSize11_0 ()		
		{
			CommandId = idCalcPrivateResourceSize11_0;
		}

		CalcPrivateResourceSize11_0(const D3D11DDIARG_CREATERESOURCE* pCreateResource)
		{
			CommandId = idCalcPrivateResourceSize11_0;
			memcpy(&CreateResource_, pCreateResource, sizeof(D3D11DDIARG_CREATERESOURCE));
		}

	public:

		D3D11DDIARG_CREATERESOURCE  CreateResource_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateResourceSize11_0 )( D3D10DDI_HDEVICE  hDevice, CONST D3D11DDIARG_CREATERESOURCE*  pCreateResource );
