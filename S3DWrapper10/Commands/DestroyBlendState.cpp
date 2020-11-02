#include "stdafx.h"
#include "DestroyBlendState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyBlendState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyBlendState( pWrapper->hDevice, hBlendState_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroyBlendState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyBlendState" );
		WriteStreamer::Reference( "hBlendState", hBlendState_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hBlendState_);
		return true;
	}

	bool DestroyBlendState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hBlendState_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyBlendState )(  D3D10DDI_HDEVICE hDevice, D3D10DDI_HBLENDSTATE hBlendState )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyBlendState* command = new Commands::DestroyBlendState( hBlendState );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyBlendState( D3D10_DEVICE, hBlendState );
	}
#endif
}
