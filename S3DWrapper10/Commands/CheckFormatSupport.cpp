#include "stdafx.h"
#include "CheckFormatSupport.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CheckFormatSupport::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCheckFormatSupport(pWrapper->hDevice, Format_, pFormatCaps_);
		AFTER_EXECUTE(this); 
	}

	bool CheckFormatSupport::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CheckFormatSupport" );
		WriteStreamer::Value( "Format", Format_ );
		WriteStreamer::Value( "FormatCaps", *pFormatCaps_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CheckFormatSupport::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Format_ );
		UINT FormatCaps_ = 0;
		ReadStreamer::Value( FormatCaps_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CheckFormatSupport)(D3D10DDI_HDEVICE  hDevice, DXGI_FORMAT  Format, UINT*  pFormatCaps)
{
#ifndef EXECUTE_IMMEDIATELY_G3
	Commands::CheckFormatSupport* command = new Commands::CheckFormatSupport(Format, pFormatCaps);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCheckFormatSupport(D3D10_DEVICE, Format, pFormatCaps);
	}
#endif
}