#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateShaderResourceViewSize : public CommandWithAllocator<CalcPrivateShaderResourceViewSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateShaderResourceViewSize ()		
		{
			CommandId = idCalcPrivateShaderResourceViewSize;
		}

		CalcPrivateShaderResourceViewSize(const D3D10DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView)
		{
			CommandId = idCalcPrivateShaderResourceViewSize;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof(D3D10DDIARG_CREATESHADERRESOURCEVIEW));
		}

	public:

		D3D10DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize )( D3D10DDI_HDEVICE  hDevice, 
													  CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView );
