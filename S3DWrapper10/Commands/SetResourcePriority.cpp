#include "stdafx.h"
#include "SetResourcePriority.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetResourcePriority::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnSetResourcePriority(&PriorityData_);
	}

	bool SetResourcePriority::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetResourcePriority" );
		WriteStreamer::ValueByRef( "PriorityData", pPriorityData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );		
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetResourcePriority::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::ValueByRef( pPriorityData_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

HRESULT ( APIENTRY  SetResourcePriority )  (DXGI_DDI_ARG_SETRESOURCEPRIORITY*  pPriorityData)
{
	DXGI_MODIFY_DEVICE(pPriorityData);
	DXGI_DDI_HRESOURCE hResource = pPriorityData->hResource;
	pPriorityData->hResource = UNWRAP_CV_RESOURCE_HANDLE( pPriorityData->hResource );

#ifndef EXECUTE_IMMEDIATELY_G2
	DXGI_GET_WRAPPER()->ProcessCB( );

	Commands::SetResourcePriority* command = new Commands::SetResourcePriority( pPriorityData );
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnSetResourcePriority(pPriorityData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnSetResourcePriority(pPriorityData);
	}
#endif

	pPriorityData->hResource = hResource;

	DXGI_RESTORE_DEVICE(pPriorityData);
	return hResult;
}