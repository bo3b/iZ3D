#include "stdafx.h"
#include "DestroyRasterizerState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyRasterizerState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyRasterizerState( pWrapper->hDevice, hRasterizerState_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroyRasterizerState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyRasterizerState" );
		WriteStreamer::Reference( "hRasterizerState", hRasterizerState_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hRasterizerState_);
		return true;
	}

	bool DestroyRasterizerState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hRasterizerState_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyRasterizerState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRASTERIZERSTATE hRasterizerState )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyRasterizerState* command = new Commands::DestroyRasterizerState( hRasterizerState );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyRasterizerState( D3D10_DEVICE, hRasterizerState );
	}
#endif
}