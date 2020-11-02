#include "stdafx.h"
#include "PsSetSamplers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void PsSetSamplers::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnPsSetSamplers( pWrapper->hDevice,
			Offset_,
			NumValues_,
			GetPointerToVector(Values_)
			);
		AFTER_EXECUTE(this); 
	}

	bool PsSetSamplers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "PsSetSamplers");
	}

}

VOID ( APIENTRY PsSetSamplers )(
								D3D10DDI_HDEVICE  hDevice, 
								UINT  Offset, 
								UINT  NumSamplers, 
								CONST D3D10DDI_HSAMPLER*  phSamplers
								)
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::PsSetSamplers* command = new Commands::PsSetSamplers( Offset, NumSamplers, phSamplers );
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnPsSetSamplers( D3D10_DEVICE, Offset, NumSamplers, phSamplers );
	}
#endif
}