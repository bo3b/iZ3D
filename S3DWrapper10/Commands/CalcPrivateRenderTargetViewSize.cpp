#include "stdafx.h"
#include "CalcPrivateRenderTargetViewSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void CalcPrivateRenderTargetViewSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateRenderTargetViewSize(pWrapper->hDevice, &CreateRenderTargetView_);
	}

	bool CalcPrivateRenderTargetViewSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateRenderTargetViewSize" );
		WriteStreamer::Value( "CreateRenderTargetView", CreateRenderTargetView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateRenderTargetViewSize::ReadFromFile	()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateRenderTargetView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateRenderTargetViewSize )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D10DDIARG_CREATERENDERTARGETVIEW*  pCreateRenderTargetView )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateRenderTargetViewSize* command = new Commands::CalcPrivateRenderTargetViewSize(pCreateRenderTargetView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateRenderTargetViewSize(D3D10_DEVICE, pCreateRenderTargetView);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateRenderTargetViewSize(D3D10_DEVICE, pCreateRenderTargetView);
	}
#endif
	size += ADDITIONAL_RTV_SIZE;
	return size;
}
