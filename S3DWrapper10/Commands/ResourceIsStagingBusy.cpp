#include "stdafx.h"
#include "ResourceIsStagingBusy.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceIsStagingBusy::Execute( D3DDeviceWrapper *pWrapper )
	{
		// Do nothing because it's a BOOL type, so we have to call it in the wrapper function
		//
		// pWrapper->OriginalDeviceFuncs.pfnResourceIsStagingBusy( pWrapper->hDevice,
		//		hResource_		
		// );
	}

	bool ResourceIsStagingBusy::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceIsStagingBusy" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceIsStagingBusy::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

BOOL ( APIENTRY ResourceIsStagingBusy )(
										D3D10DDI_HDEVICE  hDevice, 
										D3D10DDI_HRESOURCE  hResource
										)
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::ResourceIsStagingBusy* command = new Commands::ResourceIsStagingBusy( hResource );
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_D3D10();
		hResult = D3D10_GET_ORIG().pfnResourceIsStagingBusy( D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hResource) );
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand( command, true );
	return hResult;
#else
	{
		THREAD_GUARD_D3D10();
		return D3D10_GET_ORIG().pfnResourceIsStagingBusy( D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hResource) );
	}
#endif
}
