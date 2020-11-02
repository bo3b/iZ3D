#include "stdafx.h"
#include "CalcPrivateUnorderedAccessViewSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void CalcPrivateUnorderedAccessViewSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateUnorderedAccessViewSize(pWrapper->hDevice, &CreateUnorderedAccessView_);
	}

	bool CalcPrivateUnorderedAccessViewSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateUnorderedAccessViewSize11_0" );
		//WriteStreamer::Value( "CreateUnorderedAccessView", CreateUnorderedAccessView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateUnorderedAccessViewSize11_0::ReadFromFile	()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateUnorderedAccessView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateUnorderedAccessViewSize11_0 )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW* pCreateUnorderedAccessView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateUnorderedAccessViewSize11_0* command = new Commands::CalcPrivateUnorderedAccessViewSize11_0(pCreateUnorderedAccessView);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateUnorderedAccessViewSize(D3D10_DEVICE, pCreateUnorderedAccessView);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D11_GET_ORIG().pfnCalcPrivateUnorderedAccessViewSize(D3D10_DEVICE, pCreateUnorderedAccessView);
	}
#endif
	size += ADDITIONAL_RTV_SIZE;
	return size;
}
