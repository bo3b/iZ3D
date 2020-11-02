#include "stdafx.h"
#include "DestroyElementLayout.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyElementLayout::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyElementLayout( pWrapper->hDevice, hElementLayout_ );
		AFTER_EXECUTE(this); 
	}

	bool DestroyElementLayout::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyElementLayout" );
		WriteStreamer::Reference( "hElementLayout", hElementLayout_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hElementLayout_);
		return true;
	}

	bool DestroyElementLayout::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hElementLayout_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyElementLayout )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HELEMENTLAYOUT hElementLayout )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyElementLayout* command = new Commands::DestroyElementLayout( hElementLayout );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyElementLayout( D3D10_DEVICE, hElementLayout );
	}
#endif
}
