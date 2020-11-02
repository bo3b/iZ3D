#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateDepthStencilView : public CommandWithAllocator<CreateDepthStencilView>
	{
	public:
		CreateDepthStencilView()		
		{
			CommandId = idCreateDepthStencilView;
		}

		CreateDepthStencilView(const D3D10DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView, 
			D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, 
			D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView)
		{ 
			CommandId = idCreateDepthStencilView;
			memcpy(&CreateDepthStencilView_, pCreateDepthStencilView, sizeof(D3D10DDIARG_CREATEDEPTHSTENCILVIEW));

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

		D3D10DDIARG_CREATEDEPTHSTENCILVIEW  CreateDepthStencilView_; 
		D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView_; 
		D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView_;

		CREATES(hDepthStencilView_);
		DEPENDS_ON(CreateDepthStencilView_.hDrvResource);
	};

}

VOID ( APIENTRY CreateDepthStencilView )( D3D10DDI_HDEVICE  hDevice, 
										 CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView, 
										 D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView );
