#include "stdafx.h"
#include "DrawIndexed.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

#include "VsSetShaderResources.h"
#include "PsSetShaderResources.h"
#include "GsSetShaderResources.h"
#include "xxSetShaderResources.h"

namespace Commands
{

	void DrawIndexed::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDrawIndexed(pWrapper->hDevice, IndexCount_, StartIndexLocation_, BaseVertexLocation_);
		AFTER_EXECUTE(this); 
	}

	bool DrawIndexed::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DrawIndexed" );
		WriteStreamer::Value( "IndexCount", IndexCount_ );
		WriteStreamer::Value( "StartIndexLocation", StartIndexLocation_ );
		WriteStreamer::Value( "BaseVertexLocation", BaseVertexLocation_ );

		OnWrite(pWrapper);

		WriteStreamer::CmdEnd();
		return true;
	}

	bool DrawIndexed::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( IndexCount_);
		ReadStreamer::Value( StartIndexLocation_);
		ReadStreamer::Value( BaseVertexLocation_);

		OnRead();

		ReadStreamer::CmdEnd();
		return true;
	}

	void DrawIndexed::BuildCommand(CDumpState *ds)
	{
		DrawBase::BuildCommand(ds);
		output("ctx->DrawIndexed(%d,%d,%d);",IndexCount_,StartIndexLocation_,BaseVertexLocation_);
	}


}

VOID ( APIENTRY DrawIndexed )( D3D10DDI_HDEVICE hDevice, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DrawIndexed* command = new Commands::DrawIndexed( IndexCount, StartIndexLocation, BaseVertexLocation );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDrawIndexed( D3D10_DEVICE, IndexCount, StartIndexLocation, BaseVertexLocation );
	}
#endif
}

