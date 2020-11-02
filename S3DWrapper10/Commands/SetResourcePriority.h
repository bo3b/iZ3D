#pragma once

#include "Command.h"

namespace Commands
{

	class SetResourcePriority : public CommandWithAllocator<SetResourcePriority>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		SetResourcePriority()
		{
			CommandId		= idSetResourcePriority;
		}

		SetResourcePriority( DXGI_DDI_ARG_SETRESOURCEPRIORITY*  pPriorityData )
		{
			CommandId		= idSetResourcePriority;
			pPriorityData_ = pPriorityData;
			//MEMCOPY(&PriorityData_, pPriorityData, sizeof(DXGI_DDI_ARG_SETRESOURCEPRIORITY));
			//PriorityData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_SETRESOURCEPRIORITY*	pPriorityData_;
		//DXGI_DDI_ARG_SETRESOURCEPRIORITY  PriorityData_;
		HRESULT								RetValue_;
	};

}

extern HRESULT ( APIENTRY  SetResourcePriority )  (DXGI_DDI_ARG_SETRESOURCEPRIORITY*  pPriorityData);
