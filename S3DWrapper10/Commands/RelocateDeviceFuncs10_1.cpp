#include "stdafx.h"
#include "RelocateDeviceFuncs10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void RelocateDeviceFuncs10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs10_1.pfnRelocateDeviceFuncs( pWrapper->hDevice,
			pDeviceFunctions_
			);
		AFTER_EXECUTE(this); 
	}

	bool RelocateDeviceFuncs10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "RelocateDeviceFuncs10_1" );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool RelocateDeviceFuncs10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY RelocateDeviceFuncs10_1 )( D3D10DDI_HDEVICE  hDevice,  struct D3D10_1DDI_DEVICEFUNCS*  pDeviceFunctions )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	D3D10_GET_WRAPPER()->ProcessCB();
	memcpy(pDeviceFunctions, &D3D10_GET_WRAPPER()->OriginalDeviceFuncs10_1, sizeof D3D10_1DDI_DEVICEFUNCS); // restore
	D3D10_1_GET_ORIG().pfnRelocateDeviceFuncs(D3D10_DEVICE, pDeviceFunctions);
	D3D10_GET_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
}