#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateDeferredContextSize11_0 : public CommandWithAllocator<CalcPrivateDeferredContextSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateDeferredContextSize11_0 ()		
		{
			CommandId = idCalcPrivateDeferredContextSize11_0;
			memset(&CalcPrivateDeferredContextSize_, 0, sizeof( D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE ));
		}

		CalcPrivateDeferredContextSize11_0( CONST D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE* pCalcPrivateDeferredContextSize)
		{
			CommandId = idCalcPrivateDeferredContextSize11_0;
			memcpy(&CalcPrivateDeferredContextSize_, pCalcPrivateDeferredContextSize, sizeof( D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE ));
		}

	public:

		D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE		CalcPrivateDeferredContextSize_;
		SIZE_T											RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateDeferredContextSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE*  pCalcPrivateDeferredContextSize );
