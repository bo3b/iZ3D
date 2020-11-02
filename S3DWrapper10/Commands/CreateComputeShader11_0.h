#pragma once

#include "Command.h"
//#include "..\ShaderAnalyzer.h" 
#include "CreateShaderBase.h"

namespace Commands
{

	class CreateComputeShader11_0 : public CommandWithAllocator<CreateComputeShader11_0,CreateShaderBase>
	{
	public:
		CreateComputeShader11_0()
		{
			CommandId = idCreateComputeShader11_0;
		}

		CreateComputeShader11_0(const unsigned int*  pCode, 
			D3D10DDI_HSHADER  hShader, 
			D3D10DDI_HRTSHADER  hRTShader)
		{
			CommandId = idCreateComputeShader11_0;
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

		CREATES(hShader_);
	};

}

VOID (APIENTRY  CreateComputeShader11_0)(D3D10DDI_HDEVICE  hDevice, CONST UINT*  pCode, D3D10DDI_HSHADER  hShader, 
									D3D10DDI_HRTSHADER  hRTShader);
