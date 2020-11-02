#pragma once

#include "Command.h"
#include <memory.h>


namespace Commands
{

	class CalcPrivateDepthStencilViewSize11_0  : public CommandWithAllocator<CalcPrivateDepthStencilViewSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateDepthStencilViewSize11_0 ()
		{
			CommandId = idCalcPrivateDepthStencilViewSize11_0;
		}

		CalcPrivateDepthStencilViewSize11_0( CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView)
		{
			CommandId = idCalcPrivateDepthStencilViewSize11_0;
			memcpy(&CreateDepthStencilView_, pCreateDepthStencilView, sizeof(D3D11DDIARG_CREATEDEPTHSTENCILVIEW));
		}


	public:

		D3D11DDIARG_CREATEDEPTHSTENCILVIEW  CreateDepthStencilView_;
		SIZE_T								RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView );
