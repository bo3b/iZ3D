#include "stdafx.h"
#include "SetPredication.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void SetPredication::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetPredication( pWrapper->hDevice,
			hQuery_, PredicateValue_
			);
		AFTER_EXECUTE(this); 
	}

	void SetPredication::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_Predication = this;
	}

	bool SetPredication::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetPredication" );
		WriteStreamer::Reference( "hQuery", hQuery_ );
		WriteStreamer::Value( "PredicateValue", PredicateValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetPredication::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hQuery_ );
		ReadStreamer::Value( PredicateValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY SetPredication )(
								 D3D10DDI_HDEVICE  hDevice, 
								 D3D10DDI_HQUERY  hQuery, 
								 BOOL  PredicateValue
								 )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetPredication* command = new Commands::SetPredication( hQuery, PredicateValue );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetPredication( D3D10_DEVICE, hQuery, PredicateValue );
	}
#endif
}