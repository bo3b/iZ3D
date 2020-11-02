#include "stdafx.h"
#include "SetResourceMinLOD11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetResourceMinLOD11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnSetResourceMinLOD(pWrapper->hDevice, 
			GET_RESOURCE_HANDLE(hResource_), MinLOD_);
		AFTER_EXECUTE(this); 
	}

	bool SetResourceMinLOD11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetResourceMinLOD11_0" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "MinLOD", MinLOD_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetResourceMinLOD11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::Value( MinLOD_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool SetResourceMinLOD11_0::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_RESOURCE_HANDLE(hResource_))
			return true;
		return false;
	}
}

VOID ( APIENTRY SetResourceMinLOD11_0 )( D3D10DDI_HDEVICE  hDevice, 
										D3D10DDI_HRESOURCE  hResource, FLOAT  MinLOD )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetResourceMinLOD11_0* command = new Commands::SetResourceMinLOD11_0(hResource, MinLOD);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	D3D11_GET_ORIG().pfnSetResourceMinLOD(D3D10_DEVICE, GET_RESOURCE_HANDLE(hResource), MinLOD);
#endif
}