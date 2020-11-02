#include "stdafx.h"
#include "DrawIndexedInstanced.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DrawIndexedInstanced::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDrawIndexedInstanced(pWrapper->hDevice, IndexCountPerInstance_,
			InstanceCount_, StartIndexLocation_, BaseVertexLocation_, StartInstanceLocation_);
		AFTER_EXECUTE(this); 
	}

	bool DrawIndexedInstanced::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DrawIndexedInstanced" );
		WriteStreamer::Value( "IndexCountPerInstance", IndexCountPerInstance_ );
		WriteStreamer::Value( "InstanceCount", InstanceCount_ );
		WriteStreamer::Value( "StartIndexLocation", StartIndexLocation_ );
		WriteStreamer::Value( "BaseVertexLocation", BaseVertexLocation_ );
		WriteStreamer::Value( "StartInstanceLocation", StartInstanceLocation_ );

		OnWrite(pWrapper);

		WriteStreamer::CmdEnd();

		return true;
	}

	bool DrawIndexedInstanced::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);	
		ReadStreamer::Value( IndexCountPerInstance_);
		ReadStreamer::Value( InstanceCount_);
		ReadStreamer::Value( StartIndexLocation_);
		ReadStreamer::Value( BaseVertexLocation_);
		ReadStreamer::Value( StartInstanceLocation_);

		OnRead();

		ReadStreamer::CmdEnd();
		return true;
	}

	void DrawIndexedInstanced::BuildCommand(CDumpState *ds)
	{
		DrawBase::BuildCommand(ds);
		output("ctx->DrawIndexedInstanced(%u,%u,%u,%d,%u);",
			IndexCountPerInstance_,InstanceCount_,StartIndexLocation_,BaseVertexLocation_,StartInstanceLocation_);
	}

}

VOID ( APIENTRY DrawIndexedInstanced )( D3D10DDI_HDEVICE hDevice, UINT IndexCountPerInstance, UINT InstanceCount, 
									   UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DrawIndexedInstanced* command = new Commands::DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, 
		BaseVertexLocation, StartInstanceLocation );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDrawIndexedInstanced( D3D10_DEVICE, IndexCountPerInstance, InstanceCount, StartIndexLocation, 
			BaseVertexLocation, StartInstanceLocation );
	}
#endif
}
