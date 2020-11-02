#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateShaderResourceViewSize11_0 : public CommandWithAllocator<CalcPrivateShaderResourceViewSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateShaderResourceViewSize11_0 ()		
		{
			CommandId		= idCalcPrivateShaderResourceViewSize11_0;
		}

		CalcPrivateShaderResourceViewSize11_0( CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW* pCreateShaderResourceView)
		{
			CommandId		= idCalcPrivateShaderResourceViewSize11_0;
			memcpy(&CreateShaderResourceView_, pCreateShaderResourceView, sizeof(D3D11DDIARG_CREATESHADERRESOURCEVIEW));
		}

	public:

		D3D11DDIARG_CREATESHADERRESOURCEVIEW  CreateShaderResourceView_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

extern SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
																 CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView );
