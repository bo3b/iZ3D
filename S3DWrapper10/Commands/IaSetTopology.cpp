#include "stdafx.h"
#include "IaSetTopology.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void IaSetTopology::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnIaSetTopology(pWrapper->hDevice, PrimitiveTopology_);
		AFTER_EXECUTE(this); 
	}

	void IaSetTopology::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_IaTopology = this;
	}

	bool IaSetTopology::WriteToFile( D3DDeviceWrapper *pWrapper ) const	
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "IaSetTopology" );
		WriteStreamer::Value( "PrimitiveTopology", PrimitiveTopology_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool IaSetTopology::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);			
		ReadStreamer::Value( PrimitiveTopology_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void IaSetTopology::BuildCommand(CDumpState *ds)
	{
		output("ctx->IASetPrimitiveTopology((DXTOPOLOGY)%d);",PrimitiveTopology_);
	}

}

VOID ( APIENTRY IaSetTopology )( D3D10DDI_HDEVICE  hDevice, D3D10_DDI_PRIMITIVE_TOPOLOGY  PrimitiveTopology )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::IaSetTopology* command = new Commands::IaSetTopology(PrimitiveTopology);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnIaSetTopology(D3D10_DEVICE, PrimitiveTopology);
	}
#endif
}
