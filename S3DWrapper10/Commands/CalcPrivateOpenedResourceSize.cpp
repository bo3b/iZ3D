#include "stdafx.h"
#include "CalcPrivateOpenedResourceSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateOpenedResourceSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateOpenedResourceSize(pWrapper->hDevice, &OpenResource_);
	}

	bool CalcPrivateOpenedResourceSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateOpenedResourceSize" );
		WriteStreamer::Value( "OpenResource", OpenResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateOpenedResourceSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );		
		ReadStreamer::Value( OpenResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateOpenedResourceSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource )
{  
	SIZE_T size = 0;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateOpenedResourceSize* command = new Commands::CalcPrivateOpenedResourceSize(pOpenResource);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateOpenedResourceSize(D3D10_DEVICE, pOpenResource);
	}
#ifndef FINAL_RELEASE
	command->RetValue_= size;
#endif
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateOpenedResourceSize(D3D10_DEVICE, pOpenResource);
	}
#endif
	size += ADDITIONAL_RESOURCE_SIZE;
	return size;
}
