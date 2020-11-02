#include "stdafx.h"
#include "CalcPrivateDepthStencilViewSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"

namespace Commands
{

	void CalcPrivateDepthStencilViewSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateDepthStencilViewSize(pWrapper->hDevice, &CreateDepthStencilView_);
	}

	bool CalcPrivateDepthStencilViewSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateDepthStencilViewSize11_0" );
		//WriteStreamer::Value( "CreateDepthStencilView", CreateDepthStencilView_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateDepthStencilViewSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateDepthStencilView_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

SIZE_T ( APIENTRY CalcPrivateDepthStencilViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateDepthStencilViewSize11_0* command = new Commands::CalcPrivateDepthStencilViewSize11_0(pCreateDepthStencilView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateDepthStencilViewSize(D3D10_DEVICE, pCreateDepthStencilView);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateDepthStencilViewSize(D3D10_DEVICE, pCreateDepthStencilView);
	}
#endif
	size += ADDITIONAL_DSV_SIZE;
	return size;
}
