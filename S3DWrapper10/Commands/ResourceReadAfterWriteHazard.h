#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceReadAfterWriteHazard : public CommandWithAllocator<ResourceReadAfterWriteHazard>
	{
	public:
		EMPTY_OUTPUT();

		ResourceReadAfterWriteHazard()
		{
			CommandId	= idResourceReadAfterWriteHazard;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceReadAfterWriteHazard( D3D10DDI_HRESOURCE	hResource )
		{
			CommandId	= idResourceReadAfterWriteHazard;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hResource_	= hResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HRESOURCE	hResource_;
	};

}

extern VOID ( APIENTRY ResourceReadAfterWriteHazard )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hResource
	);
