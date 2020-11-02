#include "stdafx.h"
#include "SetTextFilterSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetTextFilterSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetTextFilterSize(pWrapper->hDevice, Width_, Height_);
		AFTER_EXECUTE(this); 
	}

	void SetTextFilterSize::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_TextFilterSize = this;
	}

	bool SetTextFilterSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetTextFilterSize" );
		WriteStreamer::Value( "Width", Width_ );
		WriteStreamer::Value( "Height", Height_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetTextFilterSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Width_ );
		ReadStreamer::Value( Height_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  SetTextFilterSize)(D3D10DDI_HDEVICE  hDevice, UINT  Width, UINT  Height )
{  
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetTextFilterSize* command = new Commands::SetTextFilterSize(Width, Height);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetTextFilterSize(D3D10_DEVICE, Width, Height);
	}
#endif
}