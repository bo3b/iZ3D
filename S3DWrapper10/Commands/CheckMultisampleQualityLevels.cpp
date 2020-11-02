#include "stdafx.h"
#include "CheckMultisampleQualityLevels.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CheckMultisampleQualityLevels::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCheckMultisampleQualityLevels(pWrapper->hDevice, Format_, SampleCount_, pNumQualityLevels_);
		AFTER_EXECUTE(this); 
	}

	bool CheckMultisampleQualityLevels::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CheckMultisampleQualityLevels" );
		WriteStreamer::Value( "Format", Format_ );
		WriteStreamer::Value( "SampleCount", SampleCount_ );
		WriteStreamer::Value( "NumQualityLevels", *pNumQualityLevels_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CheckMultisampleQualityLevels::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value(  Format_ );
		ReadStreamer::Value(  SampleCount_ );
		UINT NumQualityLevels_ = 0;
		ReadStreamer::Value(  NumQualityLevels_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CheckMultisampleQualityLevels)(D3D10DDI_HDEVICE  hDevice, DXGI_FORMAT  Format, 
											   UINT  SampleCount, UINT*  pNumQualityLevels)
{
#ifndef EXECUTE_IMMEDIATELY_G3
	Commands::CheckMultisampleQualityLevels* command = new Commands::CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCheckMultisampleQualityLevels(D3D10_DEVICE, Format, SampleCount, pNumQualityLevels);
	}
#endif
}