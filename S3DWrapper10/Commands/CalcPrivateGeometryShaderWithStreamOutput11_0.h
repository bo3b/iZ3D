#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateGeometryShaderWithStreamOutput11_0 : public CommandWithAllocator<CalcPrivateGeometryShaderWithStreamOutput11_0>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateGeometryShaderWithStreamOutput11_0 ()		
		{
			CommandId = idCalcPrivateGeometryShaderWithStreamOutput11_0;
		}

		CalcPrivateGeometryShaderWithStreamOutput11_0(const D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT* pCreateGeometryShaderWithStreamOutput,
			const D3D10DDIARG_STAGE_IO_SIGNATURES *pSignatures)
		{
			CommandId = idCalcPrivateGeometryShaderWithStreamOutput11_0;
			memcpy(&CreateGeometryShaderWithStreamOutput_, pCreateGeometryShaderWithStreamOutput, sizeof( D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT ));
			memcpy(&Signatures_, pSignatures, sizeof( D3D10DDIARG_STAGE_IO_SIGNATURES ));
		}

	public:

		D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT  CreateGeometryShaderWithStreamOutput_;
		D3D10DDIARG_STAGE_IO_SIGNATURES Signatures_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T (APIENTRY  CalcPrivateGeometryShaderWithStreamOutput11_0)(D3D10DDI_HDEVICE  hDevice, 
															 CONST D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryShaderWithStreamOutput, 
															 CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures);
