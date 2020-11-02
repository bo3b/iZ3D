#include "stdafx.h"
#include "CalcPrivateGeometryShaderWithStreamOutput11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CalcPrivateGeometryShaderWithStreamOutput11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateGeometryShaderWithStreamOutput(pWrapper->hDevice,
		//  &CreateGeometryShaderWithStreamOutput_, &Signatures_);
	}

	bool CalcPrivateGeometryShaderWithStreamOutput11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateGeometryShaderWithStreamOutput11_0" );
		//WriteStreamer::Value( "CreateGeometryShaderWithStreamOutput", CreateGeometryShaderWithStreamOutput_ );
		WriteStreamer::Value( "Signatures", Signatures_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateGeometryShaderWithStreamOutput11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateGeometryShaderWithStreamOutput_ );
		ReadStreamer::Value( Signatures_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T (APIENTRY  CalcPrivateGeometryShaderWithStreamOutput11_0)(D3D10DDI_HDEVICE  hDevice, 
															 CONST D3D11DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryShaderWithStreamOutput, 
															 CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateGeometryShaderWithStreamOutput11_0* command = new Commands::CalcPrivateGeometryShaderWithStreamOutput11_0(pCreateGeometryShaderWithStreamOutput, pSignatures);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput(D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput(D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures);
	}
#endif
	size += ADDITIONAL_SHADER_SIZE;
	return size;
}
