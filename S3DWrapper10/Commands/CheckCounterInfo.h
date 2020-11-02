#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CheckCounterInfo : public CommandWithAllocator<CheckCounterInfo> 
	{
	public:

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile	();		

		CheckCounterInfo()		
		{
			CommandId = idCheckCounterInfo;
		}

		CheckCounterInfo(D3D10DDI_COUNTER_INFO* pCounterInfo)
		{
			CommandId = idCheckCounterInfo;
			memcpy(&CounterInfo_, pCounterInfo, sizeof(D3D10DDI_COUNTER_INFO));
		}

	public:

		D3D10DDI_COUNTER_INFO  CounterInfo_;
	};

}

extern VOID (APIENTRY  CheckCounterInfo)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_COUNTER_INFO*  pCounterInfo);
