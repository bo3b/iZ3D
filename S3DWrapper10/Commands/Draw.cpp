#include "stdafx.h"
#include "Draw.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void Draw::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDraw(pWrapper->hDevice, VertexCount_, StartVertexLocation_);
		AFTER_EXECUTE(this); 
	}

	bool Draw::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "Draw" );
		WriteStreamer::Value( "VertexCount", VertexCount_ );
		WriteStreamer::Value( "StartVertexLocation", StartVertexLocation_ );

		OnWrite(pWrapper);

		WriteStreamer::CmdEnd();
		return true;
	}

	bool Draw::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( VertexCount_);
		ReadStreamer::Value( StartVertexLocation_);

		OnRead();

		ReadStreamer::CmdEnd();
		return true;
	}

	void Draw::BuildCommand(CDumpState *ds)
	{
		DrawBase::BuildCommand(ds);
		output("ctx->Draw(%d,%d);",VertexCount_,StartVertexLocation_);
	}
}

VOID ( APIENTRY Draw )( D3D10DDI_HDEVICE  hDevice, UINT  VertexCount, UINT  StartVertexLocation )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::Draw* command = new Commands::Draw( VertexCount, StartVertexLocation );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDraw( D3D10_DEVICE, VertexCount, StartVertexLocation );
	}
#endif
}
