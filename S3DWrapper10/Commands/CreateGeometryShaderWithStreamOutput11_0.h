#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateGeometryShaderWithStreamOutput11_0 : public CommandWithAllocator<CreateGeometryShaderWithStreamOutput11_0>
	{
	public:

		CreateGeometryShaderWithStreamOutput11_0()		
		{
			CommandId = idCreateGeometryShaderWithStreamOutput11_0;
		}

		CreateGeometryShaderWithStreamOutput11_0(const D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryWithShaderOutput, 
			D3D10DDI_HSHADER  hShader, 
			D3D10DDI_HRTSHADER  hRTShader, 
			const D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
		{    
			CommandId = idCreateGeometryShaderWithStreamOutput11_0;
			memcpy(&CreateGeometryWithShaderOutput_, pCreateGeometryWithShaderOutput, sizeof(D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT));
			memcpy(&Signatures_, pSignatures, sizeof(D3D10DDIARG_STAGE_IO_SIGNATURES));


			hShader_ = hShader;
			hRTShader_ = hRTShader;    
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

	public:

		D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT  CreateGeometryWithShaderOutput_; 
		D3D10DDI_HSHADER  hShader_; 
		D3D10DDI_HRTSHADER  hRTShader_; 
		D3D10DDIARG_STAGE_IO_SIGNATURES  Signatures_;

		CREATES(hShader_);
	};

}

VOID (APIENTRY  CreateGeometryShaderWithStreamOutput11_0)(D3D10DDI_HDEVICE  hDevice, 
													  CONST D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryWithShaderOutput, 
													  D3D10DDI_HSHADER  hShader, D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures);
