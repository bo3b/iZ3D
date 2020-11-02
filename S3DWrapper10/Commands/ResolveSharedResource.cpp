#include "stdafx.h"
#include "ResolveSharedResource.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "..\D3DSwapChain.h"

namespace Commands
{

	void ResolveSharedResource::Execute( D3DDeviceWrapper *pWrapper )
	{
		// pWrapper->OriginalDXGIDDIBaseFunctions2.pfnResolveSharedResource(&ResolveData_);
	}

	bool ResolveSharedResource::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResolveSharedResource" );
		WriteStreamer::ValueByRef( "ResolveData", pResolveData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResolveSharedResource::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::ValueByRef( pResolveData_);
		ReadStreamer::Value( RetValue_);
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResolveSharedResource::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		D3D10DDI_HRESOURCE hResource;
		hResource.pDrvPrivate = (void*)pResolveData_->hResource;
		if (IS_STEREO_RESOURCE_HANDLE(hResource))
			return true;
		return false;
	}
}

HRESULT ( APIENTRY  ResolveSharedResource )  (DXGI_DDI_ARG_RESOLVESHAREDRESOURCE* pResolveData)
{
	DXGI_MODIFY_DEVICE(pResolveData);

	DXGI_GET_WRAPPER()->ProcessCB( );

	DXGI_DDI_HRESOURCE hResource = pResolveData->hResource;
	pResolveData->hResource = UNWRAP_CV_RESOURCE_HANDLE( hResource );

#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::ResolveSharedResource* command = new Commands::ResolveSharedResource( pResolveData );
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG2().pfnResolveSharedResource(pResolveData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG2().pfnResolveSharedResource(pResolveData);
	}
#endif

	pResolveData->hResource = hResource;

	DXGI_RESTORE_DEVICE(pResolveData);
	return hResult;
}
