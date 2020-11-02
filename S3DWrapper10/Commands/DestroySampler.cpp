#include "stdafx.h"
#include "DestroySampler.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroySampler::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroySampler( pWrapper->hDevice, hSampler_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroySampler::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroySampler" );
		WriteStreamer::Reference( "hSampler", hSampler_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hSampler_);
		return true;
	}

	bool DestroySampler::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hSampler_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroySampler )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSAMPLER hSampler )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroySampler* command = new Commands::DestroySampler( hSampler );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroySampler( D3D10_DEVICE, hSampler );
	}
#endif
}
