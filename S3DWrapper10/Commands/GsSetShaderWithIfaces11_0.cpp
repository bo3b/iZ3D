#include "stdafx.h"
#include "GsSetShaderWithIfaces11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void GsSetShaderWithIfaces11_0::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
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

	void GsSetShaderWithIfaces11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnGsSetShaderWithIfaces( pWrapper->hDevice, GET_SHADER_HANDLE(hShader_),
			NumClassInstances_, GetPointerToVector(pIfaces_), GetPointerToVector(pPointerData_) );
		AFTER_EXECUTE(this); 
	}

	bool GsSetShaderWithIfaces11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "GsSetShaderWithIfaces11_0");
	}
}

VOID ( APIENTRY GsSetShaderWithIfaces11_0 )(  D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader,
		UINT  NumClassInstances, const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData  )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::GsSetShaderWithIfaces11_0* command = new Commands::GsSetShaderWithIfaces11_0(hShader,
		NumClassInstances, pIfaces, pPointerData);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnGsSetShaderWithIfaces( D3D10_DEVICE, GET_SHADER_HANDLE( hShader ),
			NumClassInstances, pIfaces, pPointerData );
	}
#endif
}