#include "stdafx.h"
#include "DrawAuto.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

BOOL bWaitingForQueryData = FALSE;

namespace Commands
{

	void DrawAuto::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);

		// we want to know vertices count
		D3D10DDIARG_CREATEQUERY cq;
		cq.Query = D3D10DDI_QUERY_PIPELINESTATS;
		cq.MiscFlags = 0;

		SIZE_T size = pWrapper->OriginalDeviceFuncs.pfnCalcPrivateQuerySize(pWrapper->hDevice,&cq);

		D3D10DDI_HQUERY hq;
		D3D10DDI_HRTQUERY hrtq = { 0 };
		hq.pDrvPrivate = new char[size];

		pWrapper->OriginalDeviceFuncs.pfnCreateQuery(pWrapper->hDevice,&cq,hq,hrtq);
		pWrapper->OriginalDeviceFuncs.pfnQueryBegin(pWrapper->hDevice,hq);

		pWrapper->OriginalDeviceFuncs.pfnDrawAuto(pWrapper->hDevice);

		pWrapper->OriginalDeviceFuncs.pfnQueryEnd(pWrapper->hDevice,hq);

		D3D10_QUERY_DATA_PIPELINE_STATISTICS SOStats;
		SOStats.IAVertices = -1;

		bWaitingForQueryData = TRUE;
		for(;;)
		{
			pWrapper->OriginalDeviceFuncs.pfnQueryGetData(pWrapper->hDevice,hq,&SOStats,sizeof(SOStats),0);
			if (SOStats.IAVertices != -1) break;
			Sleep(1);
		}
		bWaitingForQueryData = FALSE;

		vertexCount = SOStats.IAVertices;

		delete (char *)hq.pDrvPrivate;

		AFTER_EXECUTE(this); 
	}

	bool DrawAuto::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DrawAuto" );
		WriteStreamer::Value("vertexCount",vertexCount);

		OnWrite(pWrapper);

		WriteStreamer::CmdEnd();
		return true;
	}

	bool DrawAuto::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value(vertexCount);

		OnRead();

		ReadStreamer::CmdEnd();
		return true;
	}

	void DrawAuto::BuildCommand(CDumpState *ds)
	{
		DrawBase::BuildCommand(ds);
		output("ctx->Draw(%d,0);",vertexCount);
	}

}

VOID ( APIENTRY DrawAuto )( D3D10DDI_HDEVICE hDevice )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DrawAuto* command = new Commands::DrawAuto();
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDrawAuto( D3D10_DEVICE );
	}
#endif
}


