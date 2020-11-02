#include "stdafx.h"
#include "CreateBlendState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateBlendState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateBlendState(pWrapper->hDevice, &BlendDesc_,
			hBlendState_, hRTBlendState_);
		AFTER_EXECUTE(this); 
	}

	bool CreateBlendState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateBlendState" );
		WriteStreamer::Value( "BlendDesc", BlendDesc_ );
		WriteStreamer::Reference( "hBlendState", hBlendState_ );
		WriteStreamer::Handle( "hRTBlendState", hRTBlendState_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateBlendState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value(  BlendDesc_ );
		ReadStreamer::Reference(  hBlendState_ );
		ReadStreamer::Handle( hRTBlendState_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateBlendState::BuildCommand(CDumpState *ds)
	{
		output(glob,"ID3D10BlendState *bstate_%d;",EventID_);

		output(init,"bld.AlphaToCoverageEnable = %d;",BlendDesc_.AlphaToCoverageEnable);
		output(init,"bld.SrcBlend = (D3D10_BLEND)%d; // %s",BlendDesc_.SrcBlend,EnumToString(BlendDesc_.SrcBlend));
		output(init,"bld.DestBlend = (D3D10_BLEND)%d; // %s",BlendDesc_.DestBlend,EnumToString(BlendDesc_.DestBlend));
		output(init,"bld.BlendOp = (D3D10_BLEND_OP)%d; // %s",BlendDesc_.BlendOp,EnumToString(BlendDesc_.BlendOp));
		output(init,"bld.SrcBlendAlpha = (D3D10_BLEND)%d; // %s",BlendDesc_.SrcBlendAlpha,EnumToString(BlendDesc_.SrcBlendAlpha));
		output(init,"bld.DestBlendAlpha = (D3D10_BLEND)%d; // %s",BlendDesc_.DestBlendAlpha,EnumToString(BlendDesc_.DestBlendAlpha));
		output(init,"bld.BlendOpAlpha = (D3D10_BLEND_OP)%d; // %s",BlendDesc_.BlendOpAlpha,EnumToString(BlendDesc_.BlendOpAlpha));

		_ASSERT(D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT == 8);
		for(int i = 0;i < 8;i++)
		{
			output(init,"bld.BlendEnable[%d] = %d;",i,BlendDesc_.BlendEnable[i]);
			output(init,"bld.RenderTargetWriteMask[%d] = 0x%x;",i,BlendDesc_.RenderTargetWriteMask[i]);
		}

		output(init,"V_(device->CreateBlendState(&bld,&bstate_%d));",EventID_);

		ds->AddHandleEventId(hBlendState_,EventID_);
	}
}

VOID ( APIENTRY CreateBlendState )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_BLEND_DESC*  pBlendDesc, 
								   D3D10DDI_HBLENDSTATE  hBlendState, D3D10DDI_HRTBLENDSTATE  hRTBlendState )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateBlendState* command = new Commands::CreateBlendState(pBlendDesc, hBlendState, hRTBlendState);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateBlendState(D3D10_DEVICE, pBlendDesc, hBlendState, hRTBlendState);
	}
#endif
}
