#include "stdafx.h"
#include "CalcPrivateShaderResourceViewSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void CalcPrivateShaderResourceViewSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateShaderResourceViewSize(pWrapper->hDevice, &CreateShaderResourceView_);
	}

	bool CalcPrivateShaderResourceViewSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateShaderResourceViewSize" );
		WriteStreamer::Value( "CreateShaderResourceView", CreateShaderResourceView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateShaderResourceViewSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateShaderResourceView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize )( D3D10DDI_HDEVICE  hDevice, 
													  CONST D3D10DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateShaderResourceViewSize* command = new Commands::CalcPrivateShaderResourceViewSize(pCreateShaderResourceView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateShaderResourceViewSize(D3D10_DEVICE, pCreateShaderResourceView);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateShaderResourceViewSize(D3D10_DEVICE, pCreateShaderResourceView);
	}
#endif
	size += ADDITIONAL_SRV_SIZE;
	return size;
}