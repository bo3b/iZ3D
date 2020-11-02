#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateDeferredContext11_0 : public CommandWithAllocator<CreateDeferredContext11_0>
	{
	public:

		CreateDeferredContext11_0()
		{
			CommandId = idCreateDeferredContext11_0;
		}

		CreateDeferredContext11_0(CONST D3D11DDIARG_CREATEDEFERREDCONTEXT*  pCreateDeferredContext )
		{    
			CommandId = idCreateDeferredContext11_0;

			memcpy(&CreateDeferredContext_, pCreateDeferredContext, sizeof(D3D11DDIARG_CREATEDEFERREDCONTEXT));
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();


	public:

		D3D11DDIARG_CREATEDEFERREDCONTEXT  CreateDeferredContext_; 
	};

}

VOID ( APIENTRY CreateDeferredContext11_0 )( D3D10DDI_HDEVICE  hDevice, 
									  CONST D3D11DDIARG_CREATEDEFERREDCONTEXT*  pCreateDeferredContext  );
