#include "stdafx.h"
#include "QueryBegin.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void QueryBegin::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnQueryBegin( pWrapper->hDevice,
			hQuery_	
			);
		AFTER_EXECUTE(this); 
	}

	bool QueryBegin::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "QueryBegin" );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool QueryBegin::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hQuery_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY QueryBegin )(
							 D3D10DDI_HDEVICE  hDevice, 
							 D3D10DDI_HQUERY  hQuery
							 )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::QueryBegin* command = new Commands::QueryBegin( hQuery );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnQueryBegin( D3D10_DEVICE, hQuery );
	}
#endif
}