#include "stdafx.h"
#include "SetDepthStencilState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetDepthStencilState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetDepthStencilState( pWrapper->hDevice,
			hState_, StencilRef_
			);
		AFTER_EXECUTE(this); 
	}

	void SetDepthStencilState::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_OMDepthStencilState = this;
	}

	bool SetDepthStencilState::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetDepthStencilState" );
		WriteStreamer::Reference( "hState", hState_ );
		WriteStreamer::Value( "StencilRef", StencilRef_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetDepthStencilState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hState_);
		ReadStreamer::Value( StencilRef_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void SetDepthStencilState::BuildCommand(CDumpState *ds)
	{
		if (hState_.pDrvPrivate > 0)
		{
			output("ctx->OMSetDepthStencilState(ds_state_%d,%u);",ds->GetHandleEventId(hState_),StencilRef_);
		}
		else
		{
			output("ctx->OMSetDepthStencilState(0,%u);",StencilRef_);
		}
	}
}

VOID ( APIENTRY SetDepthStencilState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILSTATE  hState, 
									   UINT  StencilRef )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetDepthStencilState* command = new Commands::SetDepthStencilState( hState, StencilRef );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetDepthStencilState(D3D10_DEVICE, hState, StencilRef);
	}
#endif
}
