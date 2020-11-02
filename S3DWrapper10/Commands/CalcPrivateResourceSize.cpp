#include "stdafx.h"
#include "CalcPrivateResourceSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateResourceSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateResourceSize(pWrapper->hDevice, &CreateResource_);
	}

	bool CalcPrivateResourceSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateResourceSize" );
		WriteStreamer::Value( "CreateResource", CreateResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateResourceSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateResourceSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATERESOURCE*  pCreateResource )
{  
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	BEFORE_EXECUTE(command);
	THREAD_GUARD_D3D10();
	SIZE_T size = D3D10_GET_ORIG().pfnCalcPrivateResourceSize(D3D10_DEVICE, pCreateResource) + ADDITIONAL_RESOURCE_SIZE;
	AFTER_EXECUTE(command); 
	return size;
}
