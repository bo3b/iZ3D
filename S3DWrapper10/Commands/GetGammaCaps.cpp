#include "stdafx.h"
#include "GetGammaCaps.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void GetGammaCaps::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnGetGammaCaps(&GammaData_);
	}

	bool GetGammaCaps::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "GetGammaCaps" );
		WriteStreamer::ValueByRef( "GammaData", pGammaData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );		
		WriteStreamer::CmdEnd();
		return true;
	}

	bool GetGammaCaps::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::ValueByRef( pGammaData_);
		ReadStreamer::Value( RetValue_);
		ReadStreamer::CmdEnd();
		return true;
	}

}

HRESULT ( APIENTRY  GetGammaCaps )  (DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS*  pGammaData)
{
	DXGI_MODIFY_DEVICE(pGammaData);
#ifndef EXECUTE_IMMEDIATELY_G2
	DXGI_GET_WRAPPER()->ProcessCB();

	Commands::GetGammaCaps* command = new Commands::GetGammaCaps( pGammaData );
	BEFORE_EXECUTE(command);
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnGetGammaCaps(pGammaData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnGetGammaCaps(pGammaData);
	}
#endif
	DXGI_RESTORE_DEVICE(pGammaData);
	return hResult;
}
