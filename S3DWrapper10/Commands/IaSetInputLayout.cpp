#include "stdafx.h"
#include "IaSetInputLayout.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void IaSetInputLayout::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnIaSetInputLayout(pWrapper->hDevice, hInputLayout_);
		AFTER_EXECUTE(this); 
	}

	void IaSetInputLayout::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_IaInputLayout = this;
	}

	bool IaSetInputLayout::WriteToFile( D3DDeviceWrapper *pWrapper ) const	
	{		
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "IaSetInputLayout" );
		WriteStreamer::Reference( "hInputLayout", hInputLayout_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool  IaSetInputLayout::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hInputLayout_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void IaSetInputLayout::BuildCommand(CDumpState *ds)
	{
		if (hInputLayout_.pDrvPrivate)
		{
			int eid = ds->GetHandleEventId(hInputLayout_);
			if (eid < 0) output_("//");
			output("ctx->IASetInputLayout(vertexLayout_%d);",eid);
		}
		else
		{
			output("ctx->IASetInputLayout(0);");
		}
	}

}

VOID ( APIENTRY IaSetInputLayout )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HELEMENTLAYOUT  hInputLayout )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::IaSetInputLayout* command = new Commands::IaSetInputLayout(hInputLayout);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnIaSetInputLayout(D3D10_DEVICE, hInputLayout);
	}
#endif
}
