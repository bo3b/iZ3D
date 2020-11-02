#pragma once

#include "Command.h"

namespace Commands
{

	class RotateResourceIdentities : public CommandWithAllocator<RotateResourceIdentities>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper * pWrapper ) const;
		virtual bool	ReadFromFile	();

		RotateResourceIdentities()
		{
			CommandId		= idRotateResourceIdentities;
		}

		RotateResourceIdentities( DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData )
		{
			CommandId		= idRotateResourceIdentities;
			pRotateData_ = pRotateData;
			//MEMCOPY(&RotateData_, pRotateData, sizeof(DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES));
			//RotateData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*	pRotateData_;
		//DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES  RotateData_;
		HRESULT										RetValue_;
	};

}

extern HRESULT ( APIENTRY  RotateResourceIdentities )  (DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData);
