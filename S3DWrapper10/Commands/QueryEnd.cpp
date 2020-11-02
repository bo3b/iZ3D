#include "stdafx.h"
#include "QueryEnd.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void QueryEnd::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnQueryEnd( pWrapper->hDevice, 
			hQuery_
			);
		AFTER_EXECUTE(this); 
	}

	bool QueryEnd::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "QueryEnd" );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool QueryEnd::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hQuery_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY QueryEnd )(
						   D3D10DDI_HDEVICE  hDevice, 
						   D3D10DDI_HQUERY  hQuery
						   )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::QueryEnd* command = new Commands::QueryEnd( hQuery );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnQueryEnd( D3D10_DEVICE, hQuery );
	}
#endif
}