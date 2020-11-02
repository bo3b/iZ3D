#pragma once

#include "Command.h"
//#include "..\ShaderAnalyzer.h" 
#include "CreateShaderBase.h"

namespace Commands
{

	class CreateDomainShader11_0 : public CommandWithAllocator<CreateDomainShader11_0,CreateShaderBase>
	{
	public:
		CreateDomainShader11_0()
		{
			CommandId = idCreateDomainShader11_0;
		}

		CreateDomainShader11_0(const unsigned int*  pCode, 
			D3D10DDI_HSHADER  hShader, 
			D3D10DDI_HRTSHADER  hRTShader, 
			const D3D11DDIARG_TESSELLATION_IO_SIGNATURES*  pSignatures)
		{
			CommandId = idCreateDomainShader11_0;
			memcpy( &Signatures_, pSignatures, sizeof( D3D11DDIARG_TESSELLATION_IO_SIGNATURES ) );

			pCode_		= pCode;
			hShader_	= hShader; 
			hRTShader_	= hRTShader;    

			LookForFullCode(pCode_);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		const unsigned int*					pCode_;
		D3D10DDI_HSHADER					hShader_;
		D3D10DDI_HRTSHADER					hRTShader_;
		D3D11DDIARG_TESSELLATION_IO_SIGNATURES		Signatures_;

		CREATES(hShader_);
	};

}

VOID (APIENTRY  CreateDomainShader11_0)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
									D3D10DDI_HRTSHADER  hRTShader, CONST  D3D11DDIARG_TESSELLATION_IO_SIGNATURES*  pSignatures);
