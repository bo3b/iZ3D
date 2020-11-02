#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceIsStagingBusy : public CommandWithAllocator<ResourceIsStagingBusy>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		ResourceIsStagingBusy()		
		{
			CommandId = idResourceIsStagingBusy;
		}

		ResourceIsStagingBusy( D3D10DDI_HRESOURCE  hResource )
		{
			CommandId = idResourceIsStagingBusy;
			hResource_ = hResource;
		}

	public:

		D3D10DDI_HRESOURCE  hResource_;
		BOOL				RetValue_;
	};

}

extern BOOL ( APIENTRY ResourceIsStagingBusy )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hResource
	);
