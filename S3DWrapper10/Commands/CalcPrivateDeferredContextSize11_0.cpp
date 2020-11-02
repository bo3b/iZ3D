#include "stdafx.h"
#include "CalcPrivateDeferredContextSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateDeferredContextSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateDeferredContextSize(pWrapper->hDevice, &CalcPrivateDeferredContextSize_);
	}

	bool CalcPrivateDeferredContextSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateDeferredContextSize11_0" );
		//WriteStreamer::Value( "CalcPrivateDeferredContextSize", CalcPrivateDeferredContextSize_ ); // TODO
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateDeferredContextSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CalcPrivateDeferredContextSize_ ); // TODO
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateDeferredContextSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D11DDIARG_CALCPRIVATEDEFERREDCONTEXTSIZE*  pCalcPrivateDeferredContextSize )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateDeferredContextSize11_0* command = new Commands::CalcPrivateDeferredContextSize11_0(pCalcPrivateDeferredContextSize);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateDeferredContextSize(D3D10_DEVICE, pCalcPrivateDeferredContextSize);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateDeferredContextSize(D3D10_DEVICE, pCalcPrivateDeferredContextSize);
	}
#endif
	return size;
}
