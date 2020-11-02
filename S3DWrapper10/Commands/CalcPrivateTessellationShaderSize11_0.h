#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateTessellationShaderSize11_0 : public CommandWithAllocator<CalcPrivateTessellationShaderSize11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateTessellationShaderSize11_0()
		{
			CommandId		= idCalcPrivateTessellationShaderSize11_0;
			memset(&Signatures_, 0, sizeof( D3D11DDIARG_TESSELLATION_IO_SIGNATURES ) );
		}

		CalcPrivateTessellationShaderSize11_0(const UINT *pCode, const D3D11DDIARG_TESSELLATION_IO_SIGNATURES* pSignatures)
		{
			CommandId		= idCalcPrivateTessellationShaderSize11_0;
			memcpy(&Signatures_, pSignatures, sizeof(D3D11DDIARG_TESSELLATION_IO_SIGNATURES));
			pCode_ = pCode;
		}    

	public:

		const unsigned int *pCode_;
		D3D11DDIARG_TESSELLATION_IO_SIGNATURES  Signatures_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateTessellationShaderSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
										  CONST UINT*  pCode, CONST D3D11DDIARG_TESSELLATION_IO_SIGNATURES*  pSignatures );
