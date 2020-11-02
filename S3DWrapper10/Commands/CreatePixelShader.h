#pragma once

#include "Command.h"
#include <memory.h>
#include "CreateShaderBase.h"

namespace Commands
{
	class CreatePixelShader : public CommandWithAllocator<CreatePixelShader,CreateShaderBase>
	{
	public:

		CreatePixelShader()
			:	pCode_ ( NULL )
		{
			CommandId = idCreatePixelShader;
		}

		CreatePixelShader(const unsigned int*  pCode, 
			D3D10DDI_HSHADER  hShader, 
			D3D10DDI_HRTSHADER  hRTShader, 
			const D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
		{
			CommandId = idCreatePixelShader;
			memcpy(&Signatures_, pSignatures, sizeof(D3D10DDIARG_STAGE_IO_SIGNATURES));

			pCode_ = pCode;
			hShader_ = hShader; 
			hRTShader_ = hRTShader;    

			LookForFullCode(pCode_);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:
		const unsigned int*               pCode_; 
		D3D10DDI_HSHADER                  hShader_; 
		D3D10DDI_HRTSHADER                hRTShader_; 
		D3D10DDIARG_STAGE_IO_SIGNATURES   Signatures_;

		CREATES(hShader_);
	};

}

VOID (APIENTRY  CreatePixelShader)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
								   D3D10DDI_HRTSHADER  hRTShader, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures);
