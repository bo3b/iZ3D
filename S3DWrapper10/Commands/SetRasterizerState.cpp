#include "stdafx.h"
#include "SetRasterizerState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetRasterizerState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetRasterizerState( pWrapper->hDevice,
			hRasterizerState_	
			);
		AFTER_EXECUTE(this); 
	}

	void SetRasterizerState::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_RSState = this;
	}

	bool SetRasterizerState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetRasterizerState" );
		WriteStreamer::Reference( "hRasterizerState", hRasterizerState_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetRasterizerState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hRasterizerState_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void SetRasterizerState::BuildCommand(CDumpState *ds)
	{
		output("ctx->RSSetState(pRS_%d);",ds->GetHandleEventId(hRasterizerState_));
	}
}


VOID ( APIENTRY SetRasterizerState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRASTERIZERSTATE  hRasterizerState )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetRasterizerState* command = new Commands::SetRasterizerState( hRasterizerState );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetRasterizerState(D3D10_DEVICE, hRasterizerState);
	}
#endif
}
