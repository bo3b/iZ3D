#include "stdafx.h"
#include "CheckDeferredContextHandleSizes11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CheckDeferredContextHandleSizes11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCheckDeferredContextHandleSizes(pWrapper->hDevice, pHSizes_, pHandleSize_);
		AFTER_EXECUTE(this); 
	}

	bool CheckDeferredContextHandleSizes11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CheckDeferredContextHandleSizes11_0" );
		WriteStreamer::Value( "pHSizes", pHSizes_ );
		WriteStreamer::Value( "pHandleSize", pHandleSize_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CheckDeferredContextHandleSizes11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( pHSizes_ );
		//ReadStreamer::Value( pHandleSize_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CheckDeferredContextHandleSizes11_0)(D3D10DDI_HDEVICE  hDevice, 
													 UINT*  pHSizes, D3D11DDI_HANDLESIZE*  pHandleSize)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G3
	Commands::CheckDeferredContextHandleSizes11_0* command = new Commands::CheckDeferredContextHandleSizes11_0(pHSizes, pHandleSize);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCheckDeferredContextHandleSizes(D3D10_DEVICE, pHSizes, pHandleSize);
	}
#endif
}