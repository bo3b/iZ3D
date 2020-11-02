#include "stdafx.h"
#include "RelocateDeviceFuncs11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void RelocateDeviceFuncs11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnRelocateDeviceFuncs( pWrapper->hDevice,
			pDeviceFunctions_
			);
		AFTER_EXECUTE(this); 
	}

	bool RelocateDeviceFuncs11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "RelocateDeviceFuncs11_0" );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool RelocateDeviceFuncs11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY RelocateDeviceFuncs11_0 )( D3D10DDI_HDEVICE  hDevice,  struct D3D11DDI_DEVICEFUNCS*  pDeviceFunctions )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_11_0);
	D3D10_GET_WRAPPER()->ProcessCB();
	memcpy(pDeviceFunctions, &D3D10_GET_WRAPPER()->OriginalDeviceFuncs11, sizeof D3D11DDI_DEVICEFUNCS); // restore
	D3D11_GET_ORIG().pfnRelocateDeviceFuncs(D3D10_DEVICE, pDeviceFunctions);
	D3D10_GET_WRAPPER()->HookDeviceFuncs( pDeviceFunctions );
}