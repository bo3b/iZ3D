#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateElementLayoutSize : public CommandWithAllocator<CalcPrivateElementLayoutSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateElementLayoutSize ()		
		{
			CommandId = idCalcPrivateElementLayoutSize;
			memset(&CreateElementLayout_, 0, sizeof( D3D10DDIARG_CREATEELEMENTLAYOUT ));
		}

		CalcPrivateElementLayoutSize( CONST D3D10DDIARG_CREATEELEMENTLAYOUT* pCreateElementLayout)
		{
			CommandId = idCalcPrivateElementLayoutSize;
			memcpy(&CreateElementLayout_, pCreateElementLayout, sizeof( D3D10DDIARG_CREATEELEMENTLAYOUT ));
		}

	public:

		D3D10DDIARG_CREATEELEMENTLAYOUT		CreateElementLayout_;
		SIZE_T								RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateElementLayoutSize )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D10DDIARG_CREATEELEMENTLAYOUT*  pCreateElementLayout );

