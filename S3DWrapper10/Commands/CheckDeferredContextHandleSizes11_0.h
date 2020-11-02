#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CheckDeferredContextHandleSizes11_0 : public CommandWithAllocator<CheckDeferredContextHandleSizes11_0> 
	{
	public:

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile	();		

		CheckDeferredContextHandleSizes11_0()		
		{
			CommandId = idCheckDeferredContextHandleSizes11_0;
		}

		CheckDeferredContextHandleSizes11_0(UINT*  pHSizes, D3D11DDI_HANDLESIZE*  pHandleSize)
		{
			CommandId = idCheckDeferredContextHandleSizes11_0;
			pHSizes_ = pHSizes;
			pHandleSize_ = pHandleSize;
		}

	public:

		UINT*  pHSizes_;
		D3D11DDI_HANDLESIZE*  pHandleSize_;
	};

}

extern VOID (APIENTRY  CheckDeferredContextHandleSizes11_0)(D3D10DDI_HDEVICE  hDevice, 
										UINT*  pHSizes, D3D11DDI_HANDLESIZE*  pHandleSize);
