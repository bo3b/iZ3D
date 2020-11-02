#pragma once

#include "Command.h"
#include <memory.h>
#include <vector>
#include "CreateResourceBase.h"

namespace Commands
{

	class CreateResource11_0 : public CommandWithAllocator<CreateResource11_0, CreateResourceBase >
	{
	public:

		CreateResource11_0()		
		{
			CommandId = idCreateResource11_0;
		}

		CreateResource11_0(const D3D11DDIARG_CREATERESOURCE*  pCreateResource, 
			D3D10DDI_HRESOURCE  hResource, 
			D3D10DDI_HRTRESOURCE  hRTResource)
		{
			CommandId = idCreateResource11_0;
			memcpy(&CreateResource11_, pCreateResource, sizeof( D3D11DDIARG_CREATERESOURCE ));

			hResource_ = hResource;
			hRTResource_ = hRTResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const 
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();
	};

}

VOID ( APIENTRY CreateResource11_0 )( D3D10DDI_HDEVICE  hDevice, CONST D3D11DDIARG_CREATERESOURCE*  pCreateResource, 
								 D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource );
