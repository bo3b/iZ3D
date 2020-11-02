#include "stdafx.h"
#include "SetDisplayMode.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetDisplayMode::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnSetDisplayMode(&DisplayModeData_);
	}

	bool SetDisplayMode::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetDisplayMode" );
		WriteStreamer::ValueByRef( "DisplayModeData", pDisplayModeData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );		
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetDisplayMode::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::ValueByRef( pDisplayModeData_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

HRESULT ( APIENTRY  SetDisplayMode )  (DXGI_DDI_ARG_SETDISPLAYMODE*  pDisplayModeData)
{
	DXGI_MODIFY_DEVICE(pDisplayModeData);	
	DXGI_DDI_HRESOURCE hResource = pDisplayModeData->hResource;
	pDisplayModeData->hResource = UNWRAP_CV_RESOURCE_HANDLE( pDisplayModeData->hResource );
#ifndef EXECUTE_IMMEDIATELY_G2
	DXGI_GET_WRAPPER()->ProcessCB( );

	Commands::SetDisplayMode* command = new Commands::SetDisplayMode( pDisplayModeData );
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnSetDisplayMode(pDisplayModeData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult= DXGI_GET_ORIG().pfnSetDisplayMode(pDisplayModeData);
	}
#endif

	pDisplayModeData->hResource = hResource;
	DXGI_RESTORE_DEVICE(pDisplayModeData);
	return hResult;
}