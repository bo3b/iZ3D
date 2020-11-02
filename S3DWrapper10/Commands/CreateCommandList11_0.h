#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateCommandList11_0 : public CommandWithAllocator<CreateCommandList11_0>
	{
	public:

		CreateCommandList11_0()		
		{
			CommandId = idCreateCommandList11_0;
		}

		CreateCommandList11_0(
			CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList, 
			D3D11DDI_HCOMMANDLIST  hCommandList, 
			D3D11DDI_HRTCOMMANDLIST  hRTCommandList)
		{
			CommandId = idCreateCommandList11_0;
			if (pCreateCommandList)
			{
				CreateCommandList_ = *pCreateCommandList;
				pCreateCommandList_ = &CreateCommandList_;
			}
			else
				pCreateCommandList_ = NULL;

			hCommandList_ = hCommandList;
			hRTCommandList_ = hRTCommandList;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

	public:

		D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList_;
		D3D11DDIARG_CREATECOMMANDLIST  CreateCommandList_;
		D3D11DDI_HCOMMANDLIST  hCommandList_;
		D3D11DDI_HRTCOMMANDLIST  hRTCommandList_;

		CREATES(hCommandList_);
	};

}

VOID (APIENTRY  CreateCommandList11_0)(D3D10DDI_HDEVICE  hDevice, 
									   CONST D3D11DDIARG_CREATECOMMANDLIST*  pCreateCommandList, 
									   D3D11DDI_HCOMMANDLIST  hCommandList, 
									   D3D11DDI_HRTCOMMANDLIST  hRTCommandList);
