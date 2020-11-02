#pragma once

#include "Command.h"
#include <memory.h>
#include <vector>
#include "CreateResourceBase.h"

#include <fstream>

namespace Commands
{

	class CreateResource : public CommandWithAllocator<CreateResource, CreateResourceBase >
	{
	public:

		CreateResource()		
		{
			CommandId = idCreateResource;
		}

		CreateResource(const D3D10DDIARG_CREATERESOURCE*  pCreateResource, 
			D3D10DDI_HRESOURCE  hResource, 
			D3D10DDI_HRTRESOURCE  hRTResource)
		{
			CommandId = idCreateResource;
			memcpy(&CreateResource10_, pCreateResource, sizeof( D3D10DDIARG_CREATERESOURCE ));

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

VOID ( APIENTRY CreateResource )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATERESOURCE*  pCreateResource, 
								 D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource );
