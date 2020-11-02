#include "stdafx.h"
#include "DestroyResource.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyResource::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyResource(pWrapper->hDevice, GET_RESOURCE_HANDLE(hResource_));
		AFTER_EXECUTE(this);
	}

	bool DestroyResource::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyResource" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hResource_);
		return true;
	}

	bool DestroyResource::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyResource )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource )
{
	ResourceWrapper* pWrp;
	InitWrapper(hResource, pWrp);
	DESTROY_RESOURCE_WRAPPER();
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyResource* command = new Commands::DestroyResource( hResource );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();	
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyResource( D3D10_DEVICE, GET_RESOURCE_HANDLE(hResource) );
	}
#endif
}

