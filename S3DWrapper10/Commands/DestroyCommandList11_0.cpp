#include "stdafx.h"
#include "DestroyCommandList11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyCommandList11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDestroyCommandList( pWrapper->hDevice, hCommandList_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroyCommandList11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyCommandList11_0" );
		WriteStreamer::Reference( "hCommandList", hCommandList_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hCommandList_);
		return true;
	}

	bool DestroyCommandList11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hCommandList_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyCommandList11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HCOMMANDLIST  hCommandList )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyCommandList11_0* command = new Commands::DestroyCommandList11_0( hCommandList );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnDestroyCommandList( D3D10_DEVICE, hCommandList );
	}
#endif
}
