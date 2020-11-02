#include "stdafx.h"
#include "QueryGetData.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void QueryGetData::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnQueryGetData( pWrapper->hDevice,
			hQuery_,
			pData_,
			DataSize_, 
			Flags_
			);
		AFTER_EXECUTE(this); 
	}

	bool QueryGetData::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "QueryGetData" );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::Value( "pData", pData_ ); // TODO: Fix it
		WriteStreamer::Value( "DataSize", DataSize_ );
		WriteStreamer::Value( "Flags", Flags_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool QueryGetData::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hQuery_ );
		ReadStreamer::Value( pData_ );
		ReadStreamer::Value( DataSize_ );
		ReadStreamer::Value( Flags_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY QueryGetData )(
							   D3D10DDI_HDEVICE  hDevice, 
							   D3D10DDI_HQUERY  hQuery, 
							   VOID*  pData, 
							   UINT  DataSize, 
							   UINT  Flags
							   )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::QueryGetData* command = new Commands::QueryGetData( hQuery, pData, DataSize, Flags );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnQueryGetData( D3D10_DEVICE, hQuery, pData, DataSize, Flags );
	}
#endif
}