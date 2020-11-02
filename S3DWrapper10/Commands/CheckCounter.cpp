#include "stdafx.h"
#include "CheckCounter.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void CheckCounter::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCheckCounter(pWrapper->hDevice, Query_, pCounterType_, pActiveCounters_, 
			pName_, pNameLength_, pUnits_, pUnitsLength_, pDescription_, pDescriptionLength_);
		AFTER_EXECUTE(this); 
	}

	bool CheckCounter::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CheckCounter" );
		WriteStreamer::Value( "Query", Query_ );
		//WriteStreamer::Value( "pCounterType", pCounterType_ );	// FIXME !
		//WriteStreamer::Value( "pActiveCounters", pActiveCounters_ );
		WriteStreamer::Value( "pName", pName_ );
		//WriteStreamer::Value( "pNameLength", pNameLength_ );
		WriteStreamer::Value( "pUnits", pUnits_ );
		//WriteStreamer::Value( "pUnitsLength", pUnitsLength_ );
		WriteStreamer::Value( "pDescription", pDescription_ );
		//WriteStreamer::Value( "pDescriptionLength", pDescriptionLength_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CheckCounter::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Query_ );
		//ReadStreamer::Value( pCounterType_ );	// FIXME !
		//ReadStreamer::Value( pActiveCounters_ );
		ReadStreamer::Value( pName_ );
		//ReadStreamer::Value( pNameLength_ );
		ReadStreamer::Value( pUnits_ );
		//ReadStreamer::Value( pUnitsLength_ );
		ReadStreamer::Value( pDescription_ );
		//ReadStreamer::Value( pDescriptionLength_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID (APIENTRY  CheckCounter)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_QUERY  Query, D3D10DDI_COUNTER_TYPE*  pCounterType, 
							  UINT*  pActiveCounters, LPSTR  pName, UINT*  pNameLength, 
							  LPSTR  pUnits, UINT*  pUnitsLength, LPSTR  pDescription, UINT*  pDescriptionLength )
{
#ifndef EXECUTE_IMMEDIATELY_G3
	Commands::CheckCounter* command = new Commands::CheckCounter(Query, pCounterType, pActiveCounters, pName, pNameLength, 
		pUnits, pUnitsLength, pDescription, pDescriptionLength);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCheckCounter(D3D10_DEVICE, Query, pCounterType, pActiveCounters, pName, pNameLength, 
			pUnits, pUnitsLength, pDescription, pDescriptionLength);
	}
#endif
}
