#pragma once

#include "Command.h"
#include <memory.h>


namespace Commands
{

	class CalcPrivateDepthStencilViewSize : public CommandWithAllocator<CalcPrivateDepthStencilViewSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateDepthStencilViewSize ()
		{
			CommandId = idCalcPrivateDepthStencilViewSize;
		}

		CalcPrivateDepthStencilViewSize( CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView)
		{
			CommandId = idCalcPrivateDepthStencilViewSize;
			memcpy(&CreateDepthStencilView_, pCreateDepthStencilView, sizeof(D3D10DDIARG_CREATEDEPTHSTENCILVIEW));
		}


	public:

		D3D10DDIARG_CREATEDEPTHSTENCILVIEW  CreateDepthStencilView_;
		SIZE_T								RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView );

