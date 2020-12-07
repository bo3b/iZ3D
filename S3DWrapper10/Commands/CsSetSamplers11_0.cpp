#include "stdafx.h"
#include "CsSetSamplers11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	void CsSetSamplers11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCsSetSamplers( pWrapper->hDevice,
			Offset_,
			NumValues_,
			GetPointerToVector(Values_)
			);
		AFTER_EXECUTE(this); 
	}

	bool CsSetSamplers11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "CsSetSamplers11_0");
	}

}

VOID ( APIENTRY CsSetSamplers11_0 )(
								D3D10DDI_HDEVICE  hDevice, 
								UINT  Offset, 
								UINT  NumSamplers, 
								CONST D3D10DDI_HSAMPLER*  phSamplers
								)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::CsSetSamplers11_0* command = new Commands::CsSetSamplers11_0( Offset, NumSamplers, phSamplers );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}
#endif
}