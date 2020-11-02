#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateUnorderedAccessView11_0 : public CommandWithAllocator<CreateUnorderedAccessView11_0>
	{
	public:
		CreateUnorderedAccessView11_0()		
		{
			CommandId = idCreateUnorderedAccessView11_0;
		}

		CreateUnorderedAccessView11_0(
			CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*  pCreateUnorderedAccessView, 
			D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
			D3D11DDI_HRTUNORDEREDACCESSVIEW  hRTUnorderedAccessView)
		{    
			CommandId = idCreateUnorderedAccessView11_0;
			memcpy(&CreateUnorderedAccessView_, pCreateUnorderedAccessView, sizeof(D3D11DDIARG_CREATEUNORDEREDACCESSVIEW));

			hUnorderedAccessView_ = hUnorderedAccessView;
			hRTUnorderedAccessView_ = hRTUnorderedAccessView;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D11DDIARG_CREATEUNORDEREDACCESSVIEW  CreateUnorderedAccessView_; 
		D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView_; 
		D3D11DDI_HRTUNORDEREDACCESSVIEW  hRTUnorderedAccessView_;

		CREATES(hUnorderedAccessView_);
		DEPENDS_ON(CreateUnorderedAccessView_.hDrvResource);
	};

}

VOID ( APIENTRY CreateUnorderedAccessView11_0 )( D3D10DDI_HDEVICE  hDevice, 
												__in CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*  pCreateUnorderedAccessView, 
												D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
												D3D11DDI_HRTUNORDEREDACCESSVIEW  hRTUnorderedAccessView );
