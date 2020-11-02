#include "stdafx.h"
#include "CheckCounterInfo.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CheckCounterInfo::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCheckCounterInfo(pWrapper->hDevice, &CounterInfo_);
		AFTER_EXECUTE(this); 
	}

	bool CheckCounterInfo::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CheckCounterInfo" );
		WriteStreamer::Value( "CounterInfo", CounterInfo_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CheckCounterInfo::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CounterInfo_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CheckCounterInfo)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_COUNTER_INFO*  pCounterInfo)
{
#ifndef EXECUTE_IMMEDIATELY_G3
	Commands::CheckCounterInfo* command = new Commands::CheckCounterInfo(pCounterInfo);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCheckCounterInfo(D3D10_DEVICE, pCounterInfo);
	}
#endif
}