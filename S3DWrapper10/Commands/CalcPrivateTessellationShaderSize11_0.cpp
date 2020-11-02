#include "stdafx.h"
#include "CalcPrivateTessellationShaderSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CalcPrivateTessellationShaderSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateTessellationShaderSize(pWrapper->hDevice, pCode_, &Signatures_);
	}

	bool CalcPrivateTessellationShaderSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateTessellationShaderSize11_0" );
		//WriteStreamer::Value( "Signatures", Signatures_ );		
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateTessellationShaderSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( Signatures_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateTessellationShaderSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
										  CONST UINT*  pCode, CONST D3D11DDIARG_TESSELLATION_IO_SIGNATURES*  pSignatures )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateTessellationShaderSize11_0* command = new Commands::CalcPrivateTessellationShaderSize11_0(pCode, pSignatures);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateTessellationShaderSize(D3D10_DEVICE, pCode, pSignatures);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateTessellationShaderSize(D3D10_DEVICE, pCode, pSignatures);
	}
#endif
	size += ADDITIONAL_SHADER_SIZE;
	return size;
}