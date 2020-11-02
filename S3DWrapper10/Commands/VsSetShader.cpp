#include "stdafx.h"
#include "VsSetShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void VsSetShader::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->VS.m_Shader = this;

		pState->VS.m_IsStereoShader = false;
		if (hShader_.pDrvPrivate)
		{
			ShaderWrapper* pVSWrp = NULL;
			InitWrapper(hShader_, pVSWrp);
			if ( pVSWrp->IsMatrixFounded() )
				pState->VS.m_IsStereoShader = true;
		}
	}

	void VsSetShader::Execute(D3DDeviceWrapper *pWrapper)
	{
		BEFORE_EXECUTE(this);
		D3D10DDI_HSHADER shader = GET_SHADER_HANDLE( hShader_ );
		ShaderWrapper* pWrp;

		if(pWrapper->IsStereoActive())
		{
			InitWrapper(hShader_, pWrp);
			if(pWrp && pWrp->IsShaderModified())
				shader = pWrp->m_ModifiedShaderData.ModifiedShaderHandle;
		}

		pWrapper->OriginalDeviceFuncs.pfnVsSetShader(pWrapper->hDevice, shader);
		AFTER_EXECUTE(this); 
	}

	bool VsSetShader::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "VsSetShader");
	}

}


VOID ( APIENTRY VsSetShader )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::VsSetShader* command = new Commands::VsSetShader(hShader);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();

		D3D10DDI_HSHADER shader = GET_SHADER_HANDLE( hShader );
		ShaderWrapper* pWrp;

		if(D3D10_GET_WRAPPER()->IsStereoActive())
		{
			InitWrapper(hShader, pWrp);
			if(pWrp && pWrp->IsShaderModified())
				shader = pWrp->m_ModifiedShaderData.ModifiedShaderHandle;
		}
		D3D10_GET_ORIG().pfnVsSetShader(D3D10_DEVICE, shader);
	}
#endif
}
