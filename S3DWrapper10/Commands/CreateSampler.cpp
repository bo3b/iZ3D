#include "stdafx.h"
#include "CreateSampler.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateSampler::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateSampler(pWrapper->hDevice, 
			&SamplerDesc_, hSampler_, hRTSampler_);
		AFTER_EXECUTE(this); 
	}

	bool CreateSampler::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateSampler" );
		WriteStreamer::Value( "SamplerDesc", SamplerDesc_ );
		WriteStreamer::Reference( "hSampler", hSampler_ );
		WriteStreamer::Handle( "hRTSampler", hRTSampler_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateSampler::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( SamplerDesc_ );
		ReadStreamer::Reference( hSampler_ );
		ReadStreamer::Handle( hRTSampler_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateSampler::BuildCommand(CDumpState *ds)
	{
		output(glob,"DXSAMPSTATE *samp_state_%d;",EventID_);

		output(init,"desc.Filter = (DXFILTER)%s;",EnumToString(SamplerDesc_.Filter));
		output(init,"desc.AddressU = (DXTEXADDRMODE)%s;",EnumToString(SamplerDesc_.AddressU));
		output(init,"desc.AddressV = (DXTEXADDRMODE)%s;",EnumToString(SamplerDesc_.AddressV));
		output(init,"desc.AddressW = (DXTEXADDRMODE)%s;",EnumToString(SamplerDesc_.AddressW));

		output(init,"desc.MipLODBias = %f;",SamplerDesc_.MipLODBias);
		output(init,"desc.MaxAnisotropy = %u;",SamplerDesc_.MaxAnisotropy);

		output(init,"desc.ComparisonFunc = (DXCMPFUNC)%d; // %s",
			SamplerDesc_.ComparisonFunc,EnumToString(SamplerDesc_.ComparisonFunc));

		output(init,"desc.BorderColor[0] = %f;",SamplerDesc_.BorderColor[0]);
		output(init,"desc.BorderColor[1] = %f;",SamplerDesc_.BorderColor[1]);
		output(init,"desc.BorderColor[2] = %f;",SamplerDesc_.BorderColor[2]);
		output(init,"desc.BorderColor[3] = %f;",SamplerDesc_.BorderColor[3]);

		output(init,"desc.MinLOD = %f;",SamplerDesc_.MinLOD);
		output(init,"desc.MaxLOD = %f;",SamplerDesc_.MaxLOD);

		output(init,"V_(device->CreateSamplerState(&desc,&samp_state_%d));",EventID_);

		ds->AddHandleEventId(hSampler_,EventID_);
	}

}

VOID (APIENTRY  CreateSampler)(D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_SAMPLER_DESC*  pSamplerDesc, 
							   D3D10DDI_HSAMPLER  hSampler, D3D10DDI_HRTSAMPLER  hRTSampler)
{
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateSampler* command = new Commands::CreateSampler(pSamplerDesc, hSampler, hRTSampler);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateSampler(D3D10_DEVICE, pSamplerDesc, hSampler, hRTSampler);
	}
#endif
}