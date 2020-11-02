#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateUnorderedAccessViewSize11_0 : public CommandWithAllocator<CalcPrivateUnorderedAccessViewSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateUnorderedAccessViewSize11_0 ()
		{
			CommandId = idCalcPrivateUnorderedAccessViewSize11_0;
		}

		CalcPrivateUnorderedAccessViewSize11_0(const D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView)
		{
			CommandId = idCalcPrivateUnorderedAccessViewSize11_0;
			memcpy(&CreateUnorderedAccessView_, pCreateUnorderedAccessView, sizeof(D3D11DDIARG_CREATEUNORDEREDACCESSVIEW));
		}

	public:

		D3D11DDIARG_CREATEUNORDEREDACCESSVIEW  CreateUnorderedAccessView_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateUnorderedAccessViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*  pCreateUnorderedAccessView );
