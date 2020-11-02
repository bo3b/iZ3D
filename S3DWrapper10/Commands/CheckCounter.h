#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CheckCounter : public CommandWithAllocator<CheckCounter> 
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		CheckCounter()		
		{
			CommandId		= idCheckCounter;
		}

		CheckCounter(D3D10DDI_QUERY  Query, 
			D3D10DDI_COUNTER_TYPE*  pCounterType, 
			UINT*  pActiveCounters, 
			LPSTR  pName, 
			UINT*  pNameLength, 
			LPSTR  pUnits, 
			UINT*  pUnitsLength, 
			LPSTR  pDescription, 
			UINT*  pDescriptionLength)
		{
			CommandId		= idCheckCounter;
			Query_ = Query;
			pCounterType_ = pCounterType;
			pActiveCounters_ = pActiveCounters;
			pName_ = pName;
			pNameLength_ = pNameLength;
			pUnits_ = pUnits;
			pUnitsLength_ = pUnitsLength;
			pDescription_ = pDescription;
			pDescriptionLength_ = pDescriptionLength;
		}

		~CheckCounter()
		{

		}

	public:
		D3D10DDI_QUERY  Query_;
		D3D10DDI_COUNTER_TYPE*  pCounterType_;
		UINT*  pActiveCounters_;
		LPSTR  pName_;
		UINT*  pNameLength_;
		LPSTR  pUnits_;
		UINT*  pUnitsLength_;
		LPSTR  pDescription_;
		UINT*  pDescriptionLength_;

	};

}

extern VOID (APIENTRY  CheckCounter)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_QUERY  Query, D3D10DDI_COUNTER_TYPE*  pCounterType, 
									 UINT*  pActiveCounters, LPSTR  pName, UINT*  pNameLength, 
									 LPSTR  pUnits, UINT*  pUnitsLength, LPSTR  pDescription, UINT*  pDescriptionLength );
