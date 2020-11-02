#include "stdafx.h"
#include "VsSetSamplers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	
	void VsSetSamplers::Execute(D3DDeviceWrapper *pWrapper)
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnVsSetSamplers(pWrapper->hDevice, Offset_, NumValues_, GetPointerToVector(Values_));
		AFTER_EXECUTE(this); 
	}

	bool VsSetSamplers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "VsSetSamplers");
	}
}


VOID ( APIENTRY VsSetSamplers )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumSamplers, 
								CONST D3D10DDI_HSAMPLER*  phSamplers )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::VsSetSamplers* command = new Commands::VsSetSamplers(Offset, NumSamplers, phSamplers);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnVsSetSamplers(D3D10_DEVICE, Offset, NumSamplers, phSamplers);
	}
#endif
}
