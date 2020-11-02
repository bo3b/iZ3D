#include "stdafx.h"
#include "CalcPrivateSamplerSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateSamplerSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateSamplerSize(pWrapper->hDevice, &SamplerDesc_);
	}

	bool CalcPrivateSamplerSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateSamplerSize" );
		WriteStreamer::Value( "SamplerDesc", SamplerDesc_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateSamplerSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( SamplerDesc_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateSamplerSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_SAMPLER_DESC*  pSamplerDesc )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateSamplerSize* command = new Commands::CalcPrivateSamplerSize(pSamplerDesc);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateSamplerSize(D3D10_DEVICE, pSamplerDesc);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateSamplerSize(D3D10_DEVICE, pSamplerDesc);
	}
#endif;
	return size;
}