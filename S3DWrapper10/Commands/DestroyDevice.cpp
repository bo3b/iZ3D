#include "stdafx.h"
#include "DestroyDevice.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyDevice::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		//pWrapper->OriginalDeviceFuncs.pfnDestroyDevice(pWrapper->hDevice);
		AFTER_EXECUTE(this); 
	}

	bool DestroyDevice::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyDevice" );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(pWrapper->GetWrapperHandle());
		return true;
	}

	bool DestroyDevice::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  DestroyDevice)(D3D10DDI_HDEVICE  hDevice)
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyDevice* command = new Commands::DestroyDevice();
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
#endif
	D3D10_GET_WRAPPER()->~D3DDeviceWrapper();
	{
		//THREAD_GUARD_D3D10();								// Don't call that, CS in not valid anymore 
		D3D10_GET_ORIG().pfnDestroyDevice( D3D10_DEVICE );
	}
}

