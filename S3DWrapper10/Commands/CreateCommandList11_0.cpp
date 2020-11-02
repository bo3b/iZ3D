#include "stdafx.h"
#include "CreateCommandList11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateCommandList11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCreateCommandList(pWrapper->hDevice, pCreateCommandList_, hCommandList_, hRTCommandList_);
		AFTER_EXECUTE(this); 
	}

	bool CreateCommandList11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateCommandList11_0" );
		//WriteStreamer::ValueByRef( "pCreateCommandList", pCreateCommandList_ );
		WriteStreamer::Reference( "hCommandList", hCommandList_ );
		WriteStreamer::Handle( "hRTCommandList", hRTCommandList_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateCommandList11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::ValueByRef( pCreateCommandList_ );
		ReadStreamer::Reference( hCommandList_ );
		ReadStreamer::Handle( hRTCommandList_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CreateCommandList11_0)(D3D10DDI_HDEVICE  hDevice, 
									   CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList, 
									   D3D11DDI_HCOMMANDLIST  hCommandList, 
									   D3D11DDI_HRTCOMMANDLIST  hRTCommandList)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	DEBUG_MESSAGE(_T("\tWarning: CreateCommandList called!\n"));
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateCommandList11_0* command = new Commands::CreateCommandList11_0(pCreateCommandList, hCommandList, hRTCommandList);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCreateCommandList(D3D10_DEVICE, pCreateCommandList, hCommandList, hRTCommandList);
	}
#endif
}