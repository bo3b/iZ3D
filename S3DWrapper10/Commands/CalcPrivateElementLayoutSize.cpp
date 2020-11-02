#include "stdafx.h"
#include "CalcPrivateElementLayoutSize.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CalcPrivateElementLayoutSize::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDeviceFuncs.pfnCalcPrivateElementLayoutSize(pWrapper->hDevice, &CreateElementLayout_);
	}

	bool CalcPrivateElementLayoutSize::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CalcPrivateElementLayoutSize" );
		WriteStreamer::Value( "CreateElementLayout", CreateElementLayout_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CalcPrivateElementLayoutSize::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateElementLayout_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

SIZE_T ( APIENTRY CalcPrivateElementLayoutSize )( D3D10DDI_HDEVICE  hDevice, 
												 CONST D3D10DDIARG_CREATEELEMENTLAYOUT*  pCreateElementLayout )
{
	SIZE_T size;
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CalcPrivateElementLayoutSize* command = new Commands::CalcPrivateElementLayoutSize(pCreateElementLayout);
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateElementLayoutSize(D3D10_DEVICE, pCreateElementLayout);
	}
	command->RetValue_= size;
	AFTER_EXECUTE(command); 
	D3D10_GET_WRAPPER()->AddCommand(command, true);
#else
	{
		THREAD_GUARD_D3D10();
		size = D3D10_GET_ORIG().pfnCalcPrivateElementLayoutSize(D3D10_DEVICE, pCreateElementLayout);
	}
#endif
	return size;
}
