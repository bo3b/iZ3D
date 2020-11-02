#include "stdafx.h"
#include "ResourceUnmap.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	
	void ResourceUnmap::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceUnmap( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hResource_),
			Subresource_
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceUnmap::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceUnmap" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceUnmap::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hResource_);
		ReadStreamer::Value( Subresource_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void ResourceUnmap::BuildCommand(CDumpState *ds)
	{
		ds->SetUsedAsRenderTarget(hResource_,FALSE);
		output("// unlock: buffer_%d",ds->GetHandleEventId(hResource_));
	}
}

VOID ( APIENTRY ResourceUnmap )(
								D3D10DDI_HDEVICE  hDevice, 
								D3D10DDI_HRESOURCE  hResource, 
								UINT  Subresource
								)
{	
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::ResourceUnmap* command = new Commands::ResourceUnmap(
		hResource, 
		Subresource
		);
	if (!ENABLE_BUFFERING)
	D3D10_GET_WRAPPER()->AddCommand( command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceUnmap( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hResource), 
			Subresource
			);
	}
#endif
}