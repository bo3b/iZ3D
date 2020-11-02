#include "stdafx.h"
#include "QueryResourceResidency.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void QueryResourceResidency::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnQueryResourceResidency( &ResidencyData_ );
	}

	bool QueryResourceResidency::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "QueryResourceResidency" );
		WriteStreamer::ValueByRef( "ResidencyData", pResidencyData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool QueryResourceResidency::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::ValueByRef( pResidencyData_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

HRESULT ( APIENTRY QueryResourceResidency )( DXGI_DDI_ARG_QUERYRESOURCERESIDENCY*  pResidencyData )
{
	DXGI_MODIFY_DEVICE(pResidencyData);

	Commands::fast_vector<DXGI_DDI_HRESOURCE>::type	hRESOURCEBuffer(pResidencyData->pResources, pResidencyData->pResources + pResidencyData->Resources);
	for(UINT i = 0; i < pResidencyData->Resources; i++)
		hRESOURCEBuffer[ i ] = UNWRAP_CV_RESOURCE_HANDLE(((DXGI_DDI_HRESOURCE *)pResidencyData->pResources)[i]);
	const DXGI_DDI_HRESOURCE* pResources = pResidencyData->pResources;
	pResidencyData->pResources = GetPointerToVector( hRESOURCEBuffer );

#ifndef EXECUTE_IMMEDIATELY_G2
	DXGI_GET_WRAPPER()->ProcessCB();

	Commands::QueryResourceResidency* command = new Commands::QueryResourceResidency(pResidencyData);
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnQueryResourceResidency(pResidencyData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 

	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnQueryResourceResidency(pResidencyData);
	}
#endif

	pResidencyData->pResources = pResources;
	DXGI_RESTORE_DEVICE(pResidencyData);
	return hResult;
}
