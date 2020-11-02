#include "stdafx.h"
#include "CalcPrivateRasterizerStateSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateRasterizerStateSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateRasterizerStateSize(pWrapper->hDevice, &RasterizerDesc_);
	}

	bool CalcPrivateRasterizerStateSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateRasterizerStateSize" );
		WriteStreamer::Value( "RasterizerDesc", RasterizerDesc_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateRasterizerStateSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( RasterizerDesc_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateRasterizerStateSize )( D3D10DDI_HDEVICE  hDevice, 
												   CONST D3D10_DDI_RASTERIZER_DESC*  pRasterizerDesc )
{  
	SIZE_T size = 0;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateRasterizerStateSize* command = new Commands::CalcPrivateRasterizerStateSize(pRasterizerDesc);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateRasterizerStateSize(D3D10_DEVICE, pRasterizerDesc);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 	
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateRasterizerStateSize(D3D10_DEVICE, pRasterizerDesc);
	}
#endif
	return size;
}
