#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateCommandListSize11_0 : public CommandWithAllocator<CalcPrivateCommandListSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateCommandListSize11_0 ()		
		{
			CommandId = idCalcPrivateElementLayoutSize;
			memset(&CreateCommandList_, 0, sizeof( D3D10DDIARG_CREATEELEMENTLAYOUT ));
		}

		CalcPrivateCommandListSize11_0( CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList )
		{
			CommandId = idCalcPrivateElementLayoutSize;
			memcpy(&CreateCommandList_, pCreateCommandList, sizeof( D3D11DDIARG_CREATECOMMANDLIST ));
		}

	public:

		D3D11DDIARG_CREATECOMMANDLIST		CreateCommandList_;
		SIZE_T								RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateCommandListSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList );
