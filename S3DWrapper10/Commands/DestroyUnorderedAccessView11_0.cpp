#include "stdafx.h"
#include "DestroyUnorderedAccessView11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{

	void DestroyUnorderedAccessView11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDestroyUnorderedAccessView(pWrapper->hDevice, GET_UAV_HANDLE(hUnorderedAccessView_));
		AFTER_EXECUTE(this); 
	}

	bool DestroyUnorderedAccessView11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyUnorderedAccessView11_0" );
		WriteStreamer::Reference( "hUnorderedAccessView", hUnorderedAccessView_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hUnorderedAccessView_);
		return true;
	}

	bool DestroyUnorderedAccessView11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hUnorderedAccessView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyUnorderedAccessView11_0 )( D3D10DDI_HDEVICE hDevice, D3D11DDI_HUNORDEREDACCESSVIEW hUnorderedAccessView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	UnorderedAccessViewWrapper* pWrp;
	InitWrapper(hUnorderedAccessView, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyUnorderedAccessView11_0* command = new Commands::DestroyUnorderedAccessView11_0( hUnorderedAccessView );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnDestroyUnorderedAccessView( D3D10_DEVICE, GET_UAV_HANDLE(hUnorderedAccessView) );
	}
#endif
	DESTROY_UAV_WRAPPER();
}

