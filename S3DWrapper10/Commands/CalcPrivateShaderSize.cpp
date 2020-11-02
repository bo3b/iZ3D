#include "stdafx.h"
#include "CalcPrivateShaderSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CalcPrivateShaderSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateShaderSize(pWrapper->hDevice, pCode_, &Signatures_);
	}

	bool CalcPrivateShaderSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateShaderSize" );
		WriteStreamer::Value( "Signatures", Signatures_ );		
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateShaderSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Signatures_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateShaderSize )( D3D10DDI_HDEVICE  hDevice, 
										  CONST UINT*  pCode, CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateShaderSize* command = new Commands::CalcPrivateShaderSize(pCode, pSignatures);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateShaderSize(D3D10_DEVICE, pCode, pSignatures);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateShaderSize(D3D10_DEVICE, pCode, pSignatures);
	}
#endif
	size += ADDITIONAL_SHADER_SIZE;
	return size;
}