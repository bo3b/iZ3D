#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateShaderResourceView : public CommandWithAllocator<CreateShaderResourceView>
	{
	public:
		CreateShaderResourceView()		
		{
			CommandId = idCreateShaderResourceView;
		}

		CreateShaderResourceView(const D3D10DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
			D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, 
			D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView)
		{
			CommandId = idCreateShaderResourceView;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof( D3D10DDIARG_CREATESHADERRESOURCEVIEW ));

			hShaderResourceView_ = hShaderResourceView;
			hRTShaderResourceView_ = hRTShaderResourceView;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_; 
		D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView_; 
		D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView_;

		CREATES(hShaderResourceView_);
		DEPENDS_ON(CreateShaderResourceView_.hDrvResource);
	};

}

VOID ( APIENTRY CreateShaderResourceView )( D3D10DDI_HDEVICE  hDevice,
										   CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
										   D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView );
