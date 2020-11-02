#include "stdafx.h"
#include "DynamicIABufferUnmap.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DynamicIABufferUnmap::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		// only mono
		pWrapper->OriginalDeviceFuncs.pfnDynamicIABufferUnmap( pWrapper->hDevice, GET_RESOURCE_HANDLE(hResource_), Subresource_ );
		AFTER_EXECUTE(this); 
	}

	bool DynamicIABufferUnmap::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DynamicIABufferUnmap" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DynamicIABufferUnmap::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hResource_);
		ReadStreamer::Value( Subresource_);
		ReadStreamer::CmdEnd();
		return true;
	}
	
	void DynamicIABufferUnmap::BuildCommand(CDumpState *ds)
	{
		ds->SetUsedAsRenderTarget(hResource_,FALSE);
		output("// unlock: buffer_%d",ds->GetHandleEventId(hResource_));
	}

}

VOID ( APIENTRY DynamicIABufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::DynamicIABufferUnmap* command = new Commands::DynamicIABufferUnmap( hResource, Subresource );
	if (!ENABLE_BUFFERING)
	D3D10_GET_WRAPPER()->AddCommand( command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	// only mono
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDynamicIABufferUnmap( D3D10_DEVICE, GET_RESOURCE_HANDLE(hResource), Subresource );
	}
#endif
}
