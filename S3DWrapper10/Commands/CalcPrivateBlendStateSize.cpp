#include "stdafx.h"
#include "CalcPrivateBlendStateSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateBlendStateSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateBlendStateSize(pWrapper->hDevice, &BlendDesc_);
	}

	bool CalcPrivateBlendStateSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this, "CalcPrivateBlendStateSize" );
		WriteStreamer::Value( "BlendDesc", BlendDesc_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateBlendStateSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( BlendDesc_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateBlendStateSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_BLEND_DESC*  pBlendDesc )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateBlendStateSize* command = new Commands::CalcPrivateBlendStateSize(pBlendDesc);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateBlendStateSize(D3D10_DEVICE, pBlendDesc);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateBlendStateSize(D3D10_DEVICE, pBlendDesc);
	}
#endif
	return size;
}
