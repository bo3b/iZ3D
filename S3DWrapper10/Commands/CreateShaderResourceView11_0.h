#pragma once

#include "Command.h"

namespace Commands
{

	class CreateShaderResourceView11_0 : public CommandWithAllocator<CreateShaderResourceView11_0>
	{
	public:
		CreateShaderResourceView11_0()		
		{
			CommandId = idCreateShaderResourceView11_0;
		}

		CreateShaderResourceView11_0(CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
			D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, 
			D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView)
		{
			CommandId = idCreateShaderResourceView11_0;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof( D3D11DDIARG_CREATESHADERRESOURCEVIEW ));

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

		D3D11DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_; 
		D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView_; 
		D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView_;

		CREATES(hShaderResourceView_);
		DEPENDS_ON(CreateShaderResourceView_.hDrvResource);
	};

}

extern VOID ( APIENTRY CreateShaderResourceView11_0 )( D3D10DDI_HDEVICE  hDevice,
													  CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
													  D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView );
