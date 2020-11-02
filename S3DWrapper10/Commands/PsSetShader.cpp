#include "stdafx.h"
#include "PsSetShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void PsSetShader::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->PS.m_Shader = this;

		pState->PS.m_IsStereoShader = false;
		if (hShader_.pDrvPrivate)
		{
			ShaderWrapper* pPSWrp = NULL;
			InitWrapper(hShader_, pPSWrp);
			if ( pPSWrp->IsMatrixFounded() )
				pState->PS.m_IsStereoShader = true;
		}
	}

	void PsSetShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnPsSetShader( pWrapper->hDevice, GET_SHADER_HANDLE(hShader_));
		AFTER_EXECUTE(this); 
	}

	bool PsSetShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "PsSetShader");
	}

}

VOID ( APIENTRY PsSetShader )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader )
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::PsSetShader* command = new Commands::PsSetShader( hShader );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnPsSetShader( D3D10_DEVICE, GET_SHADER_HANDLE( hShader ) );
	}
#endif
}