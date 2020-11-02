#include "stdafx.h"
#include "DispatchIndirect11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DispatchIndirect11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDispatchIndirect(pWrapper->hDevice, 
			GET_RESOURCE_HANDLE(hBufferForArgs_), AlignedByteOffsetForArgs_);
		AFTER_EXECUTE(this); 
	}

	bool DispatchIndirect11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DispatchIndirect11_0" );
		WriteStreamer::Reference( "hBufferForArgs", hBufferForArgs_ );
		WriteStreamer::Value( "AlignedByteOffsetForArgs", AlignedByteOffsetForArgs_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DispatchIndirect11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hBufferForArgs_ );
		ReadStreamer::Value( AlignedByteOffsetForArgs_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void DispatchIndirect11_0::BuildCommand(CDumpState *ds)
	{
		if (hBufferForArgs_.pDrvPrivate)
		{
			output("ctx->DispatchIndirect(buffer_%d,%d);",
				ds->GetHandleEventId(hBufferForArgs_),AlignedByteOffsetForArgs_);
		}
		else
		{
			output("ctx->DispatchIndirect(0,%d);",AlignedByteOffsetForArgs_);
		}
	}

}

VOID ( APIENTRY DispatchIndirect11_0 )( D3D10DDI_HDEVICE  hDevice, 
									   D3D10DDI_HRESOURCE  hBufferForArgs, 
									   UINT  AlignedByteOffsetForArgs )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DispatchIndirect11_0* command = new Commands::DispatchIndirect11_0(hBufferForArgs, AlignedByteOffsetForArgs);
	D3D10_GET_WRAPPER()->AddCommand(command, false );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	D3D11_GET_ORIG().pfnDispatchIndirect(D3D10_DEVICE, GET_RESOURCE_HANDLE(hBufferForArgs), AlignedByteOffsetForArgs);
#endif
}