#include "stdafx.h"
#include "RelocateDeviceFuncs.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void RelocateDeviceFuncs::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnRelocateDeviceFuncs( pWrapper->hDevice,
			pDeviceFunctions_		
			);
		AFTER_EXECUTE(this); 
	}

	bool RelocateDeviceFuncs::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "RelocateDeviceFuncs" );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool RelocateDeviceFuncs::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY RelocateDeviceFuncs )( D3D10DDI_HDEVICE  hDevice,  struct D3D10DDI_DEVICEFUNCS*  pDeviceFunctions )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
	D3D10_GET_WRAPPER()->ProcessCB();
	memcpy(pDeviceFunctions, &D3D10_GET_WRAPPER()->OriginalDeviceFuncs, sizeof D3D10DDI_DEVICEFUNCS); // restore
	D3D10_GET_ORIG().pfnRelocateDeviceFuncs(D3D10_DEVICE, pDeviceFunctions);
	D3D10_GET_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
}