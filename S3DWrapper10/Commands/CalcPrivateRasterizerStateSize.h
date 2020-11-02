#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateRasterizerStateSize : public CommandWithAllocator<CalcPrivateRasterizerStateSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile();

		CalcPrivateRasterizerStateSize ()		
		{
			CommandId = idCalcPrivateRasterizerStateSize;
		}

		CalcPrivateRasterizerStateSize(const D3D10_DDI_RASTERIZER_DESC* pRasterizerDesc)
		{
			CommandId = idCalcPrivateRasterizerStateSize;
			memcpy(&RasterizerDesc_, pRasterizerDesc, sizeof(D3D10_DDI_RASTERIZER_DESC));
		}

	public:

		D3D10_DDI_RASTERIZER_DESC RasterizerDesc_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateRasterizerStateSize )( D3D10DDI_HDEVICE  hDevice, 
												   CONST D3D10_DDI_RASTERIZER_DESC*  pRasterizerDesc );
