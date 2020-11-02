#include "stdafx.h"
#include "CreateDepthStencilState.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateDepthStencilState::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateDepthStencilState(pWrapper->hDevice,
			&DepthStencilDesc_, hDepthStencilState_, hRTDepthStencilState_);
		AFTER_EXECUTE(this); 
	}

	bool CreateDepthStencilState::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateDepthStencilState" );
		WriteStreamer::Value( "DepthStencilDesc", DepthStencilDesc_ );
		WriteStreamer::Reference( "hDepthStencilState", hDepthStencilState_ );
		WriteStreamer::Handle( "hRTDepthStencilState", hRTDepthStencilState_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateDepthStencilState::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( DepthStencilDesc_ );
		ReadStreamer::Reference( hDepthStencilState_ );
		ReadStreamer::Handle( hRTDepthStencilState_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateDepthStencilState::BuildCommand(CDumpState *ds)
	{
		output(glob,"DXDEPTHSTATE *ds_state_%d;",EventID_);
		set_output_target(init);

		output("dsd.DepthEnable = %d;",DepthStencilDesc_.DepthEnable);
		output("dsd.DepthWriteMask = (DXWRITEMASK)%d;",DepthStencilDesc_.DepthWriteMask);
		output("dsd.DepthFunc = (DXCMPFUNC)%d; // %s",DepthStencilDesc_.DepthFunc,EnumToString(DepthStencilDesc_.DepthFunc));
		
		output("dsd.StencilEnable = %d;",DepthStencilDesc_.StencilEnable);
		output("dsd.StencilReadMask = 0x%x;",DepthStencilDesc_.StencilReadMask);
		output("dsd.StencilWriteMask = 0x%x;",DepthStencilDesc_.StencilWriteMask);
		
		output("dsd.FrontFace.StencilDepthFailOp = (DXSTENSILOP)%d;",DepthStencilDesc_.FrontFace.StencilDepthFailOp);
		output("dsd.FrontFace.StencilFailOp = (DXSTENSILOP)%d;",DepthStencilDesc_.FrontFace.StencilFailOp);
		output("dsd.FrontFace.StencilFunc = (DXCMPFUNC)%d;",DepthStencilDesc_.FrontFace.StencilFunc);
		output("dsd.FrontFace.StencilPassOp = (DXSTENSILOP)%d;",DepthStencilDesc_.FrontFace.StencilPassOp);

		output("dsd.BackFace.StencilDepthFailOp = (DXSTENSILOP)%d;",DepthStencilDesc_.BackFace.StencilDepthFailOp);
		output("dsd.BackFace.StencilFailOp = (DXSTENSILOP)%d;",DepthStencilDesc_.BackFace.StencilFailOp);
		output("dsd.BackFace.StencilFunc = (DXCMPFUNC)%d;",DepthStencilDesc_.BackFace.StencilFunc);
		output("dsd.BackFace.StencilPassOp = (DXSTENSILOP)%d;",DepthStencilDesc_.BackFace.StencilPassOp);

		output("V_(device->CreateDepthStencilState(&dsd,&ds_state_%d));",EventID_);

		ds->AddHandleEventId(hDepthStencilState_,EventID_);
	}
}

VOID ( APIENTRY CreateDepthStencilState )( D3D10DDI_HDEVICE  hDevice, 
										  CONST D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc, 
										  D3D10DDI_HDEPTHSTENCILSTATE  hDepthStencilState, 
										  D3D10DDI_HRTDEPTHSTENCILSTATE  hRTDepthStencilState )
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateDepthStencilState* command = new Commands::CreateDepthStencilState(pDepthStencilDesc,
		hDepthStencilState, hRTDepthStencilState);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateDepthStencilState(D3D10_DEVICE, pDepthStencilDesc,
			hDepthStencilState, hRTDepthStencilState);
	}
#endif
}