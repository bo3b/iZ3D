#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateElementLayout : public CommandWithAllocator<CreateElementLayout>
	{
	public:
		CreateElementLayout()
		{
			CommandId = idCreateElementLayout;
		}

		CreateElementLayout(const D3D10DDIARG_CREATEELEMENTLAYOUT*  pCreateElementLayout, 
			D3D10DDI_HELEMENTLAYOUT  hElementLayout, 
			D3D10DDI_HRTELEMENTLAYOUT  hRTElementLayout)
		{    
			CommandId = idCreateElementLayout;

			memcpy(&CreateElementLayout_, pCreateElementLayout, sizeof(D3D10DDIARG_CREATEELEMENTLAYOUT));

			hElementLayout_    = hElementLayout;
			hRTElementLayout_  = hRTElementLayout;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDIARG_CREATEELEMENTLAYOUT  CreateElementLayout_; 
		D3D10DDI_HELEMENTLAYOUT  hElementLayout_; 
		D3D10DDI_HRTELEMENTLAYOUT  hRTElementLayout_;

		CREATES(hElementLayout_);
	};

}

VOID ( APIENTRY CreateElementLayout )( D3D10DDI_HDEVICE  hDevice, 
									  CONST D3D10DDIARG_CREATEELEMENTLAYOUT*  pCreateElementLayout, 
									  D3D10DDI_HELEMENTLAYOUT  hElementLayout, D3D10DDI_HRTELEMENTLAYOUT  hRTElementLayout );
