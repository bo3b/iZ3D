#pragma once

#include "Command.h"

namespace Commands
{

	class CreateShaderResourceView10_1 : public CommandWithAllocator<CreateShaderResourceView10_1>
	{
	public:
		CreateShaderResourceView10_1()		
		{
			CommandId = idCreateShaderResourceView10_1;
		}

		CreateShaderResourceView10_1(CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
			D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, 
			D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView)
		{
			CommandId = idCreateShaderResourceView10_1;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof( D3D10_1DDIARG_CREATESHADERRESOURCEVIEW ));

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

		D3D10_1DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_; 
		D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView_; 
		D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView_;

		CREATES(hShaderResourceView_);
		DEPENDS_ON(CreateShaderResourceView_.hDrvResource);
	};

}

extern VOID ( APIENTRY CreateShaderResourceView10_1 )( D3D10DDI_HDEVICE  hDevice,
													  CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
													  D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView );

