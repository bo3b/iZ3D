#include "stdafx.h"
#include "CreateRasterizerState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateRasterizerState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateRasterizerState(pWrapper->hDevice,
			&RasterizerDesc_, hRasterizerState_, hRTRasterizerState_);
		AFTER_EXECUTE(this); 
	}

	bool CreateRasterizerState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateRasterizerState" );
		WriteStreamer::Value( "RasterizerDesc", RasterizerDesc_ );
		WriteStreamer::Reference( "hRasterizerState", hRasterizerState_ );
		WriteStreamer::Handle( "hRTRasterizerState", hRTRasterizerState_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateRasterizerState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( RasterizerDesc_ );
		ReadStreamer::Reference( hRasterizerState_ );
		ReadStreamer::Handle( hRTRasterizerState_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateRasterizerState::BuildCommand(CDumpState *ds)
	{
		set_output_target(init);
		output("rasterizerState.CullMode = (DXCULLMODE)%d;",RasterizerDesc_.CullMode);
		output("rasterizerState.FillMode = (DXFILLMODE)%d;",RasterizerDesc_.FillMode);

		output("rasterizerState.FrontCounterClockwise = %d;",RasterizerDesc_.FrontCounterClockwise);
		output("rasterizerState.DepthBias = %d;",RasterizerDesc_.DepthBias); 
		output("rasterizerState.DepthBiasClamp = %ff;",RasterizerDesc_.DepthBiasClamp);
		output("rasterizerState.SlopeScaledDepthBias = %ff;",RasterizerDesc_.SlopeScaledDepthBias);
		output("rasterizerState.DepthClipEnable = %d;",RasterizerDesc_.DepthClipEnable);
		output("rasterizerState.ScissorEnable = %d;",RasterizerDesc_.ScissorEnable); 
		output("rasterizerState.MultisampleEnable = %d;",RasterizerDesc_.MultisampleEnable); 
		output("rasterizerState.AntialiasedLineEnable = %d;",RasterizerDesc_.AntialiasedLineEnable);

		output(glob,"DXRASTSTATE* pRS_%d;",EventID_);  
		output("V_(device->CreateRasterizerState( &rasterizerState, &pRS_%d));",EventID_);

		ds->AddHandleEventId(hRasterizerState_,EventID_);
	}
}

VOID ( APIENTRY CreateRasterizerState )( D3D10DDI_HDEVICE  hDevice, 
										CONST D3D10_DDI_RASTERIZER_DESC*  pRasterizerDesc, 
										D3D10DDI_HRASTERIZERSTATE  hRasterizerState, 
										D3D10DDI_HRTRASTERIZERSTATE  hRTRasterizerState )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateRasterizerState* command = new Commands::CreateRasterizerState(pRasterizerDesc,
		hRasterizerState, hRTRasterizerState);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateRasterizerState(D3D10_DEVICE, pRasterizerDesc,
			hRasterizerState, hRTRasterizerState);
	}
#endif
}