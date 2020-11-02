#include "stdafx.h"
#include "CalcPrivateShaderResourceViewSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void CalcPrivateShaderResourceViewSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		// pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateShaderResourceViewSize(pWrapper->hDevice, &CreateShaderResourceView_);
	}

	bool CalcPrivateShaderResourceViewSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateShaderResourceViewSize11_0" );
		//WriteStreamer::Value( "CreateShaderResourceView", CreateShaderResourceView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateShaderResourceViewSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateShaderResourceView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateShaderResourceViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
														  CONST D3D11DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateShaderResourceViewSize11_0* command = new Commands::CalcPrivateShaderResourceViewSize11_0(pCreateShaderResourceView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateShaderResourceViewSize(D3D10_DEVICE, pCreateShaderResourceView);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateShaderResourceViewSize(D3D10_DEVICE, pCreateShaderResourceView);
	}
#endif
	size += ADDITIONAL_SRV_SIZE;
	return size;
}