#include "stdafx.h"
#include "CalcPrivateDepthStencilStateSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateDepthStencilStateSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateDepthStencilStateSize(pWrapper->hDevice, &DepthStencilDesc_);
	}

	bool CalcPrivateDepthStencilStateSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateDepthStencilStateSize" );
		WriteStreamer::Value( "DepthStencilDesc", DepthStencilDesc_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateDepthStencilStateSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( DepthStencilDesc_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

SIZE_T ( APIENTRY CalcPrivateDepthStencilStateSize )( D3D10DDI_HDEVICE  hDevice, 
													 CONST D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateDepthStencilStateSize* command = new Commands::CalcPrivateDepthStencilStateSize(pDepthStencilDesc);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilStateSize(D3D10_DEVICE, pDepthStencilDesc);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilStateSize(D3D10_DEVICE, pDepthStencilDesc);
	}
#endif
	return size;
}
