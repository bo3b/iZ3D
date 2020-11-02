#include "stdafx.h"
#include "CreateBlendState10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateBlendState10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs10_1.pfnCreateBlendState(pWrapper->hDevice, &BlendDesc_,
			hBlendState_, hRTBlendState_);
		AFTER_EXECUTE(this); 
	}

	bool CreateBlendState10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateBlendState10_1" );
		WriteStreamer::Value( "BlendDesc", BlendDesc_ );
		WriteStreamer::Reference( "hBlendState", hBlendState_ );
		WriteStreamer::Handle( "hRTBlendState", hRTBlendState_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateBlendState10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( BlendDesc_ );
		ReadStreamer::Reference( hBlendState_ );
		ReadStreamer::Handle( hRTBlendState_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateBlendState10_1::BuildCommand(CDumpState *ds)
	{
		output(glob,"DXBLENDSTATE1 *bstate_%d;",EventID_);
		output(init,"bld1.AlphaToCoverageEnable = %d;",BlendDesc_.AlphaToCoverageEnable);
		output(init,"bld1.IndependentBlendEnable = %d;",BlendDesc_.IndependentBlendEnable);
		for(int i = 0;i < 8;i++)
		{
			output(init,"bld1.RenderTarget[%d].BlendEnable = %d;",i,BlendDesc_.RenderTarget[i].BlendEnable);

			output(init,"bld1.RenderTarget[%d].SrcBlend = (DXBLEND)%d; // %s",i,BlendDesc_.RenderTarget[i].SrcBlend,EnumToString(BlendDesc_.RenderTarget[i].SrcBlend));
			output(init,"bld1.RenderTarget[%d].DestBlend = (DXBLEND)%d; // %s",i,BlendDesc_.RenderTarget[i].DestBlend,EnumToString(BlendDesc_.RenderTarget[i].DestBlend));
			output(init,"bld1.RenderTarget[%d].BlendOp = (DXBLENDOP)%d; // %s",i,BlendDesc_.RenderTarget[i].BlendOp,EnumToString(BlendDesc_.RenderTarget[i].BlendOp));
			output(init,"bld1.RenderTarget[%d].SrcBlendAlpha = (DXBLEND)%d; // %s",i,BlendDesc_.RenderTarget[i].SrcBlendAlpha,EnumToString(BlendDesc_.RenderTarget[i].SrcBlendAlpha));
			output(init,"bld1.RenderTarget[%d].DestBlendAlpha = (DXBLEND)%d; // %s",i,BlendDesc_.RenderTarget[i].DestBlendAlpha,EnumToString(BlendDesc_.RenderTarget[i].DestBlendAlpha));
			output(init,"bld1.RenderTarget[%d].BlendOpAlpha = (DXBLENDOP)%d; // %s",i,BlendDesc_.RenderTarget[i].BlendOpAlpha,EnumToString(BlendDesc_.RenderTarget[i].BlendOpAlpha));

			output(init,"bld1.RenderTarget[%d].RenderTargetWriteMask = 0x%x;",i,BlendDesc_.RenderTarget[i].RenderTargetWriteMask);
		}

		output(init,"V_(device->CREATEBLENDSTATE(&bld1,&bstate_%d));",EventID_);
		
		ds->AddHandleEventId(hBlendState_,EventID_);
	}
}

VOID ( APIENTRY CreateBlendState10_1 )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_1_DDI_BLEND_DESC*  pBlendDesc, 
									   D3D10DDI_HBLENDSTATE  hBlendState, D3D10DDI_HRTBLENDSTATE  hRTBlendState )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateBlendState10_1* command = new Commands::CreateBlendState10_1(pBlendDesc, hBlendState, hRTBlendState);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_1_GET_ORIG().pfnCreateBlendState(D3D10_DEVICE, pBlendDesc, hBlendState, hRTBlendState);
	}
#endif
}