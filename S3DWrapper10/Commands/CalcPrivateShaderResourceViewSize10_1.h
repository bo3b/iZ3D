#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateShaderResourceViewSize10_1 : public CommandWithAllocator<CalcPrivateShaderResourceViewSize10_1>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateShaderResourceViewSize10_1 ()		
		{
			CommandId		= idCalcPrivateShaderResourceViewSize10_1;
		}

		CalcPrivateShaderResourceViewSize10_1( CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView)
		{
			CommandId		= idCalcPrivateShaderResourceViewSize10_1;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof(D3D10_1DDIARG_CREATESHADERRESOURCEVIEW));
		}

	public:

		D3D10_1DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

extern SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize10_1 )( D3D10DDI_HDEVICE  hDevice, 
																 CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView );
