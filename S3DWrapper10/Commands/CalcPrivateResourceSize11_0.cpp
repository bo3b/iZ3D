#include "stdafx.h"
#include "CalcPrivateResourceSize11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateResourceSize11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs11.pfnCalcPrivateResourceSize(pWrapper->hDevice, &CreateResource_);
	}

	bool CalcPrivateResourceSize11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateResourceSize11_0" );
		//WriteStreamer::Value( "CreateResource", CreateResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateResourceSize11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateResourceSize11_0 )( D3D10DDI_HDEVICE  hDevice, CONST D3D11DDIARG_CREATERESOURCE*  pCreateResource )
{  
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	BEFORE_EXECUTE(command);
	THREAD_GUARD_D3D10();
	SIZE_T size = D3D11_GET_ORIG().pfnCalcPrivateResourceSize(D3D10_DEVICE, pCreateResource) + ADDITIONAL_RESOURCE_SIZE;
	AFTER_EXECUTE(command); 
	return size;
}
