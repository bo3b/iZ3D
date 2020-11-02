#include "stdafx.h"
#include "DestroyDepthStencilState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyDepthStencilState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyDepthStencilState(pWrapper->hDevice, hDepthStencilState_);
		AFTER_EXECUTE(this); 
	}

	bool DestroyDepthStencilState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyDepthStencilState" );
		WriteStreamer::Reference( "hDepthStencilState", hDepthStencilState_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hDepthStencilState_);
		return true;
	}

	bool DestroyDepthStencilState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDepthStencilState_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyDepthStencilState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyDepthStencilState* command = new Commands::DestroyDepthStencilState( hDepthStencilState );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyDepthStencilState( D3D10_DEVICE, hDepthStencilState );
	}
#endif
}
