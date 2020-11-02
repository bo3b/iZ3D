#include "stdafx.h"
#include "DrawIndexedInstancedIndirect11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DrawIndexedInstancedIndirect11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDrawIndexedInstancedIndirect(pWrapper->hDevice, 
			GET_RESOURCE_HANDLE(hBufferForArgs_), AlignedByteOffsetForArgs_);
		AFTER_EXECUTE(this); 
	}

	bool DrawIndexedInstancedIndirect11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DrawIndexedInstancedIndirect11_0" );
		WriteStreamer::Reference( "hBufferForArgs", hBufferForArgs_ );
		WriteStreamer::Value( "AlignedByteOffsetForArgs", AlignedByteOffsetForArgs_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DrawIndexedInstancedIndirect11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hBufferForArgs_);
		ReadStreamer::Value( AlignedByteOffsetForArgs_);
		ReadStreamer::CmdEnd();

		__debugbreak(); // implement BuildCommand!

		return true;
	}

}

VOID ( APIENTRY DrawIndexedInstancedIndirect11_0 )( D3D10DDI_HDEVICE hDevice,
												   D3D10DDI_HRESOURCE  hBufferForArgs, UINT  AlignedByteOffsetForArgs  )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::DrawIndexedInstancedIndirect11_0* command = new Commands::DrawIndexedInstancedIndirect11_0(
		hBufferForArgs, AlignedByteOffsetForArgs  );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnDrawIndexedInstancedIndirect( D3D10_DEVICE, GET_RESOURCE_HANDLE(hBufferForArgs), AlignedByteOffsetForArgs );
	}
#endif
}
