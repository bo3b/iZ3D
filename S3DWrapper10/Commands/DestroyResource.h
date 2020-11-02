#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyResource : public CommandWithAllocator<DestroyResource>
	{
	public:
		EMPTY_OUTPUT();

		DestroyResource()		
		{
			CommandId = idDestroyResource;
		}

		DestroyResource( D3D10DDI_HRESOURCE hResource )
		{
			CommandId = idDestroyResource;
			hResource_ = hResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		//void BuildCommand(CDumpState *ds);

		D3D10DDI_HRESOURCE hResource_;
	};

}

extern VOID ( APIENTRY DestroyResource )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource );
