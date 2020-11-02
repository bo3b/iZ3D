#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateShaderSize : public CommandWithAllocator<CalcPrivateShaderSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateShaderSize()
		{
			CommandId		= idCalcPrivateShaderSize;
			memset(&Signatures_, 0, sizeof( D3D10DDIARG_STAGE_IO_SIGNATURES ) );
		}

		CalcPrivateShaderSize(const UINT *pCode, const D3D10DDIARG_STAGE_IO_SIGNATURES* pSignatures)
		{
			CommandId		= idCalcPrivateShaderSize;
			memcpy(&Signatures_, pSignatures, sizeof(D3D10DDIARG_STAGE_IO_SIGNATURES));
			pCode_ = pCode;
		}    

	public:

		const unsigned int *pCode_;
		D3D10DDIARG_STAGE_IO_SIGNATURES  Signatures_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateShaderSize )( D3D10DDI_HDEVICE  hDevice, 
										  CONST UINT*  pCode, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures );
