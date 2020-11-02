#include "stdafx.h"
#include "Dispatch11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void Dispatch11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnDispatch(pWrapper->hDevice, ThreadGroupCountX_, 
			ThreadGroupCountY_, ThreadGroupCountZ_);
		AFTER_EXECUTE(this); 
	}

	bool Dispatch11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "Dispatch11_0" );
		WriteStreamer::Value( "ThreadGroupCountX", ThreadGroupCountX_ );
		WriteStreamer::Value( "ThreadGroupCountY", ThreadGroupCountY_ );
		WriteStreamer::Value( "ThreadGroupCountZ", ThreadGroupCountZ_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool Dispatch11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( ThreadGroupCountX_ );
		ReadStreamer::Value( ThreadGroupCountY_ );
		ReadStreamer::Value( ThreadGroupCountZ_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void Dispatch11_0::BuildCommand(CDumpState *ds)
	{
		output("ctx->Dispatch(%d,%d,%d);",ThreadGroupCountX_,ThreadGroupCountY_,ThreadGroupCountZ_);
	}

}

VOID ( APIENTRY Dispatch11_0 )( D3D10DDI_HDEVICE  hDevice, UINT  ThreadGroupCountX, 
							   UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ  )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::Dispatch11_0* command = new Commands::Dispatch11_0(ThreadGroupCountX, 
		ThreadGroupCountY, ThreadGroupCountZ );
	D3D10_GET_WRAPPER()->AddCommand(command, false );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	D3D11_GET_ORIG().pfnDispatch(D3D10_DEVICE, ThreadGroupCountX, 
		ThreadGroupCountY, ThreadGroupCountZ);
#endif
}