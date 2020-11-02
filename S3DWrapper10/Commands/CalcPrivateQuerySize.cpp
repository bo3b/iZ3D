#include "stdafx.h"
#include "CalcPrivateQuerySize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateQuerySize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateQuerySize(pWrapper->hDevice, &CreateQuery_);
	}

	bool CalcPrivateQuerySize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateQuerySize" );
		WriteStreamer::Value( "CreateQuery", CreateQuery_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateQuerySize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );		
		ReadStreamer::Value( CreateQuery_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateQuerySize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATEQUERY*  pCreateQuery )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateQuerySize* command = new Commands::CalcPrivateQuerySize(pCreateQuery);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateQuerySize(D3D10_DEVICE, pCreateQuery);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateQuerySize(D3D10_DEVICE, pCreateQuery);
	}
#endif
	return size;
}
