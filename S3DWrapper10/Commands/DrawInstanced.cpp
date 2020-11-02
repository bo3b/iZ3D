#include "stdafx.h"
#include "DrawInstanced.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DrawInstanced::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDrawInstanced(pWrapper->hDevice, VertexCountPerInstance_,
			InstanceCount_, StartVertexLocation_, StartInstanceLocation_);
		AFTER_EXECUTE(this); 
	}

	bool DrawInstanced::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DrawInstanced" );
		WriteStreamer::Value( "VertexCountPerInstance", VertexCountPerInstance_ );
		WriteStreamer::Value( "InstanceCount", InstanceCount_ );
		WriteStreamer::Value( "StartVertexLocation", StartVertexLocation_ );
		WriteStreamer::Value( "StartInstanceLocation", StartInstanceLocation_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DrawInstanced::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( VertexCountPerInstance_);
		ReadStreamer::Value( InstanceCount_);
		ReadStreamer::Value( StartVertexLocation_);
		ReadStreamer::Value( StartInstanceLocation_);
		ReadStreamer::CmdEnd();

		__debugbreak(); // implement BuildCommand!

		return true;
	}
}

VOID ( APIENTRY DrawInstanced )( D3D10DDI_HDEVICE hDevice, UINT VertexCountPerInstance, UINT InstanceCount, 
								UINT StartVertexLocation, UINT StartInstanceLocation )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DrawInstanced* command = new Commands::DrawInstanced( VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDrawInstanced(D3D10_DEVICE, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation );
	}
#endif
}
