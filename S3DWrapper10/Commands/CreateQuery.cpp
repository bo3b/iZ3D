#include "stdafx.h"
#include "CreateQuery.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateQuery::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateQuery(pWrapper->hDevice, &CreateQuery_, hQuery_, hRTQuery_);
		AFTER_EXECUTE(this); 
	}

	bool CreateQuery::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateQuery" );
		WriteStreamer::Value( "CreateQuery", CreateQuery_ );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::Handle( "hRTQuery", hRTQuery_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateQuery::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( CreateQuery_);
		ReadStreamer::Skip();
		ReadStreamer::Skip();
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID (APIENTRY  CreateQuery)(D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATEQUERY*  pCreateQuery, 
							 D3D10DDI_HQUERY  hQuery, D3D10DDI_HRTQUERY  hRTQuery)
{
#ifndef EXECUTE_IMMEDIATELY_G2	
	Commands::CreateQuery* command = new Commands::CreateQuery(pCreateQuery, hQuery, hRTQuery);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateQuery(D3D10_DEVICE, pCreateQuery, hQuery, hRTQuery);
	}
#endif
}