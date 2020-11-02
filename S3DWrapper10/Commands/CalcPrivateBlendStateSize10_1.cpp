#include "stdafx.h"
#include "CalcPrivateBlendStateSize10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateBlendStateSize10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs10_1.pfnCalcPrivateBlendStateSize(pWrapper->hDevice, &BlendDesc_);
	}

	bool CalcPrivateBlendStateSize10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateBlendStateSize10_1" );
		WriteStreamer::Value( "BlendDesc", BlendDesc_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateBlendStateSize10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( BlendDesc_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateBlendStateSize10_1 )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_1_DDI_BLEND_DESC*  pBlendDesc )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateBlendStateSize10_1* command = new Commands::CalcPrivateBlendStateSize10_1(pBlendDesc);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_1_GET_ORIG().pfnCalcPrivateBlendStateSize(D3D10_DEVICE, pBlendDesc);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_1_GET_ORIG().pfnCalcPrivateBlendStateSize(D3D10_DEVICE, pBlendDesc);
	}
#endif
	return size;
}
