#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateDepthStencilView11_0 : public CommandWithAllocator<CreateDepthStencilView11_0>
	{
	public:
		CreateDepthStencilView11_0()		
		{
			CommandId = idCreateDepthStencilView11_0;
		}

		CreateDepthStencilView11_0(const D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView, 
			D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, 
			D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView)
		{ 
			CommandId = idCreateDepthStencilView11_0;
			memcpy(&CreateDepthStencilView_, pCreateDepthStencilView, sizeof(D3D11DDIARG_CREATEDEPTHSTENCILVIEW));

			hDepthStencilView_    = hDepthStencilView;
			hRTDepthStencilView_  = hRTDepthStencilView;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D11DDIARG_CREATEDEPTHSTENCILVIEW  CreateDepthStencilView_; 
		D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView_; 
		D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView_;

		CREATES(hDepthStencilView_);
		DEPENDS_ON(CreateDepthStencilView_.hDrvResource);
	};

}

VOID ( APIENTRY CreateDepthStencilView11_0 )( D3D10DDI_HDEVICE  hDevice, 
										 CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView, 
										 D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView );
