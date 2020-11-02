#include "stdafx.h"
#include "DestroyQuery.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyQuery::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyQuery( pWrapper->hDevice, hQuery_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroyQuery::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyQuery" );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hQuery_);
		return true;
	}

	bool DestroyQuery::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hQuery_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyQuery )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HQUERY hQuery )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyQuery* command = new Commands::DestroyQuery( hQuery );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyQuery( D3D10_DEVICE, hQuery );
	}
#endif
}
