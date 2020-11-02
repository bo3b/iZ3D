#include "stdafx.h"
#include "GsSetShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void GsSetShader::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->GS.m_Shader = this;

		pState->GS.m_IsStereoShader = false;
		if (hShader_.pDrvPrivate)
		{
			ShaderWrapper* pGSWrp = NULL;
			InitWrapper(hShader_, pGSWrp);
			if ( pGSWrp->IsMatrixFounded() )
				pState->GS.m_IsStereoShader = true;
		}
	}

	void GsSetShader::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnGsSetShader(pWrapper->hDevice, GET_SHADER_HANDLE(hShader_));
		AFTER_EXECUTE(this); 
	}

	bool GsSetShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "GsSetShader");
	}
}

VOID ( APIENTRY GsSetShader )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::GsSetShader* command = new Commands::GsSetShader(hShader);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnGsSetShader(D3D10_DEVICE, GET_SHADER_HANDLE(hShader));
	}
#endif
}