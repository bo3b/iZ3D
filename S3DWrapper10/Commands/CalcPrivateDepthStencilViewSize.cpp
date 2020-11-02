#include "stdafx.h"
#include "CalcPrivateDepthStencilViewSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"

namespace Commands
{

	void CalcPrivateDepthStencilViewSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateDepthStencilViewSize(pWrapper->hDevice, &CreateDepthStencilView_);
	}

	bool CalcPrivateDepthStencilViewSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateDepthStencilViewSize" );
		WriteStreamer::Value( "CreateDepthStencilView", CreateDepthStencilView_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateDepthStencilViewSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateDepthStencilView_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D10DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateDepthStencilViewSize* command = new Commands::CalcPrivateDepthStencilViewSize(pCreateDepthStencilView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilViewSize(D3D10_DEVICE, pCreateDepthStencilView);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateDepthStencilViewSize(D3D10_DEVICE, pCreateDepthStencilView);
	}
#endif
	size += ADDITIONAL_DSV_SIZE;
	return size;
}
