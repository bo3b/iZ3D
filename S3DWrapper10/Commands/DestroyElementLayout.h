#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyElementLayout : public CommandWithAllocator<DestroyElementLayout>
	{
	public:
		EMPTY_OUTPUT();

		DestroyElementLayout()		
		{
			CommandId = idDestroyElementLayout;
		}

		DestroyElementLayout( D3D10DDI_HELEMENTLAYOUT hElementLayout ) 
		{
			CommandId = idDestroyElementLayout;
			hElementLayout_ = hElementLayout;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HELEMENTLAYOUT	hElementLayout_;
	};

}

extern VOID ( APIENTRY DestroyElementLayout )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HELEMENTLAYOUT hElementLayout );
