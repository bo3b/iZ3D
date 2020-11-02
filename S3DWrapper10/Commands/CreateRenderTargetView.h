#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateRenderTargetView : public CommandWithAllocator<CreateRenderTargetView>
	{
	public:
		CreateRenderTargetView()		
		{
			CommandId = idCreateRenderTargetView;
		}

		CreateRenderTargetView(const D3D10DDIARG_CREATERENDERTARGETVIEW*  pCreateRenderTargetView, 
			D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, 
			D3D10DDI_HRTRENDERTARGETVIEW  hRTRenderTargetView)
		{    
			CommandId = idCreateRenderTargetView;
			memcpy(&CreateRenderTargetView_, pCreateRenderTargetView, sizeof(D3D10DDIARG_CREATERENDERTARGETVIEW));

			hRenderTargetView_ = hRenderTargetView;
			hRTRenderTargetView_ = hRTRenderTargetView;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDIARG_CREATERENDERTARGETVIEW  CreateRenderTargetView_; 
		D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView_; 
		D3D10DDI_HRTRENDERTARGETVIEW  hRTRenderTargetView_;

		CREATES(hRenderTargetView_);
		DEPENDS_ON(CreateRenderTargetView_.hDrvResource);
	};

}

VOID ( APIENTRY CreateRenderTargetView )( D3D10DDI_HDEVICE  hDevice, 
										 CONST D3D10DDIARG_CREATERENDERTARGETVIEW*  pCreateRenderTargetView, 
										 D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, D3D10DDI_HRTRENDERTARGETVIEW  hRTRenderTargetView );
