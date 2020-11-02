#include "stdafx.h"
#include "CalcPrivateGeometryShaderWithStreamOutput.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace Commands
{

	void CalcPrivateGeometryShaderWithStreamOutput::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateGeometryShaderWithStreamOutput(pWrapper->hDevice,
		//  &CreateGeometryShaderWithStreamOutput_, &Signatures_);
	}

	bool CalcPrivateGeometryShaderWithStreamOutput::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateGeometryShaderWithStreamOutput" );
		WriteStreamer::Value( "CreateGeometryShaderWithStreamOutput", CreateGeometryShaderWithStreamOutput_ );
		WriteStreamer::Value( "Signatures", Signatures_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateGeometryShaderWithStreamOutput::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateGeometryShaderWithStreamOutput_ );
		ReadStreamer::Value( Signatures_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T (APIENTRY  CalcPrivateGeometryShaderWithStreamOutput)(D3D10DDI_HDEVICE  hDevice, 
															 CONST D3D10DDIARG_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT*  pCreateGeometryShaderWithStreamOutput, 
															 CONST D3D10DDIARG_STAGE_IO_SIGNATURES*  pSignatures)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateGeometryShaderWithStreamOutput* command = new Commands::CalcPrivateGeometryShaderWithStreamOutput(pCreateGeometryShaderWithStreamOutput, pSignatures);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput(D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateGeometryShaderWithStreamOutput(D3D10_DEVICE, pCreateGeometryShaderWithStreamOutput, pSignatures);
	}
#endif
	size += ADDITIONAL_SHADER_SIZE;
	return size;
}
