#include "stdafx.h"
#include "CalcDeferredContextHandleSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcDeferredContextHandleSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateElementLayoutSize(pWrapper->hDevice, &CreateElementLayout_);
	}

	bool CalcDeferredContextHandleSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcDeferredContextHandleSize11_0" );
		WriteStreamer::Value( "HandleType", HandleType_ );
		WriteStreamer::Value( "pICObject", pICObject_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcDeferredContextHandleSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( HandleType_ );
		ReadStreamer::Value( pICObject_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcDeferredContextHandleSize11_0 )( D3D10DDI_HDEVICE  hDevice,  
													  D3D11DDI_HANDLETYPE  HandleType, VOID*  pICObject )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcDeferredContextHandleSize11_0* command = new Commands::CalcDeferredContextHandleSize11_0(HandleType, pICObject);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcDeferredContextHandleSize(D3D10_DEVICE, HandleType, pICObject);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcDeferredContextHandleSize(D3D10_DEVICE, HandleType, pICObject);
	}
#endif
	return size;
}
