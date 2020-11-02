#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class xxSetShaderWithIfaces11_0 : public xxSetShader
	{
	public:

		bool	WriteToFileEx		( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const;
		virtual bool	ReadFromFile	();

		xxSetShaderWithIfaces11_0( SHADER_PIPELINE ShaderPipeline )
			:	xxSetShader(ShaderPipeline)
		{
		}		
		
		xxSetShaderWithIfaces11_0( SHADER_PIPELINE					ShaderPipeline,
								   D3D10DDI_HSHADER					hShader, 
								   UINT								NumClassInstances, 
								   const UINT*						pIfaces, 
								   const D3D11DDIARG_POINTERDATA*	pPointerData )
		:	xxSetShader(ShaderPipeline, hShader)
		,	NumClassInstances_(NumClassInstances)
		,	pIfaces_(pIfaces, pIfaces + NumClassInstances)
		,	pPointerData_(pPointerData, pPointerData + NumClassInstances)
		{
		}

		void BuildCommand(CDumpState *ds);

	public:
		UINT NumClassInstances_;
		fast_vector<UINT>::type	pIfaces_;
		fast_vector<D3D11DDIARG_POINTERDATA>::type pPointerData_;
	};

}
