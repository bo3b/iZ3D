#include "stdafx.h"
#include "GsSetSamplers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void GsSetSamplers::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnGsSetSamplers(pWrapper->hDevice, Offset_, NumValues_, GetPointerToVector(Values_));
		AFTER_EXECUTE(this); 
	}

	bool GsSetSamplers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		return WriteToFileEx(pWrapper, "GsSetSamplers");
	}

}

VOID ( APIENTRY GsSetSamplers )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumSamplers, 
								CONST D3D10DDI_HSAMPLER*  phSamplers )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::GsSetSamplers* command = new Commands::GsSetSamplers(Offset, NumSamplers, phSamplers);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnGsSetSamplers(D3D10_DEVICE, Offset, NumSamplers, phSamplers);
	}
#endif
}