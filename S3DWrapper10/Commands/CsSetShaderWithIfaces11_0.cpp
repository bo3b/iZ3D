#include "stdafx.h"
#include "CsSetShaderWithIfaces11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CsSetShaderWithIfaces11_0::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->CS.m_Shader = this;

		pState->CS.m_IsStereoShader = false;
		if (hShader_.pDrvPrivate)
		{
			ShaderWrapper* pCSWrp = NULL;
			InitWrapper(hShader_, pCSWrp);
			if ( pCSWrp->IsMatrixFounded() )
				pState->CS.m_IsStereoShader = true;
		}
	}

	void CsSetShaderWithIfaces11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCsSetShaderWithIfaces( pWrapper->hDevice, 
			GET_SHADER_HANDLE(hShader_), NumClassInstances_, GetPointerToVector(pIfaces_), GetPointerToVector(pPointerData_));
		AFTER_EXECUTE(this); 
	}

	bool CsSetShaderWithIfaces11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "CsSetShaderWithIfaces11_0");
	}
}

VOID ( APIENTRY CsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader, 
				UINT  NumClassInstances, const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData )
{	
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::CsSetShaderWithIfaces11_0* command = new Commands::CsSetShaderWithIfaces11_0(hShader,
		NumClassInstances, pIfaces, pPointerData);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCsSetShaderWithIfaces( D3D10_DEVICE, GET_SHADER_HANDLE( hShader ),
			NumClassInstances, pIfaces, pPointerData );
	}
#endif
}