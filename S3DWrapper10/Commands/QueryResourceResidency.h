#pragma once

#include "Command.h"

namespace Commands
{

	class QueryResourceResidency : public CommandWithAllocator<QueryResourceResidency>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		QueryResourceResidency()		
		{
			CommandId = idQueryResourceResidency;
		}

		QueryResourceResidency( DXGI_DDI_ARG_QUERYRESOURCERESIDENCY*  pResidencyData )
		{
			CommandId = idQueryResourceResidency;
			pResidencyData_ = pResidencyData;
			//MEMCOPY(&ResidencyData_, pResidencyData, sizeof(DXGI_DDI_ARG_QUERYRESOURCERESIDENCY));
			//ResidencyData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_QUERYRESOURCERESIDENCY*	pResidencyData_;
		//DXGI_DDI_ARG_QUERYRESOURCERESIDENCY  ResidencyData_;
		HRESULT									RetValue_;
	};
}

extern HRESULT ( __stdcall QueryResourceResidency )(
	DXGI_DDI_ARG_QUERYRESOURCERESIDENCY*  pResidencyData
	);
