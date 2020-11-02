#include "stdafx.h"
#include "CreateDeferredContext11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CreateDeferredContext11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCreateDeferredContext(pWrapper->hDevice, &CreateDeferredContext_);		
		AFTER_EXECUTE(this); 
	}

	bool CreateDeferredContext11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateDeferredContext11_0" );
		//WriteStreamer::Value( "CreateDeferredContext", CreateDeferredContext_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool  CreateDeferredContext11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		//ReadStreamer::Value( CreateDeferredContext_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY CreateDeferredContext11_0 )( D3D10DDI_HDEVICE  hDevice, 
									  CONST D3D11DDIARG_CREATEDEFERREDCONTEXT*  pCreateDeferredContext )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	DEBUG_MESSAGE(_T("\tWarning: CreateDeferredContext called!\n"));
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateDeferredContext11_0* command = new Commands::CreateDeferredContext11_0(pCreateDeferredContext);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCreateDeferredContext(D3D10_DEVICE, pCreateDeferredContext);
	}
#endif
}