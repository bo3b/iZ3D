#include "stdafx.h"
#include "SetBlendState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetBlendState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetBlendState( pWrapper->hDevice, 
			hState_, pBlendFactor_, SampleMask_
			);
		AFTER_EXECUTE(this); 
	}

	void SetBlendState::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_OMBlendState = this;
	}

	bool SetBlendState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetBlendState" );
		WriteStreamer::Reference( "hState", hState_ );
		WriteStreamer::ArrayValues( "BlendFactor", 4, pBlendFactor_ );
		WriteStreamer::Value( "SampleMask", SampleMask_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetBlendState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hState_);
		ReadStreamer::ArrayValues( pBlendFactor_, 4  );
		ReadStreamer::Value( SampleMask_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void SetBlendState::BuildCommand(CDumpState *ds)
	{
		int eid = ds->GetHandleEventId(hState_);
		output(decl,"extern float bfactor_%d[4];",EventID_);
		output(glob,"float bfactor_%d[4] = { %ff,%ff,%ff,%ff };",EventID_,
			pBlendFactor_[0],pBlendFactor_[1],pBlendFactor_[2],pBlendFactor_[3]);
		output("ctx->OMSetBlendState(bstate_%d,bfactor_%d,0x%x);",eid,EventID_,SampleMask_);
	}
}


VOID ( APIENTRY SetBlendState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HBLENDSTATE  hState, 
								const FLOAT pBlendFactor[4], UINT  SampleMask )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetBlendState* command = new Commands::SetBlendState( hState, pBlendFactor, SampleMask );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetBlendState(D3D10_DEVICE, hState, pBlendFactor, SampleMask);
	}
#endif
}
