#include "stdafx.h"
#include "CalcPrivateCommandListSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateCommandListSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateCommandListSize(pWrapper->hDevice, &CreateElementLayout_);
	}

	bool CalcPrivateCommandListSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateCommandListSize11_0" );
		//WriteStreamer::Value( "CreateCommandList", CreateCommandList_ ); // TODO: D3D11DDIARG_CREATECOMMANDLIST
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateCommandListSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateCommandList_ ); // TODO: D3D11DDIARG_CREATECOMMANDLIST
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateCommandListSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateCommandListSize11_0* command = new Commands::CalcPrivateCommandListSize11_0(pCreateCommandList);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateCommandListSize(D3D10_DEVICE, pCreateCommandList);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateCommandListSize(D3D10_DEVICE, pCreateCommandList);
	}
#endif
	return size;
}
