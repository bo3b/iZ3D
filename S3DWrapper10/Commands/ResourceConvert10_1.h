#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceConvert10_1 : public CommandWithAllocator<ResourceConvert10_1>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		ResourceConvert10_1()
		{
			CommandId		= idResourceConvert10_1;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceConvert10_1(
			D3D10DDI_HRESOURCE  hDstResource, 
			D3D10DDI_HRESOURCE  hSrcResource
			)
		{
			CommandId		= idResourceConvert10_1;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_	= hDstResource; 
			hSrcResource_	= hSrcResource;
		}

		virtual bool IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void SetDestResourceType( TextureType type );
		virtual void GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HRESOURCE  hDstResource_; 
		D3D10DDI_HRESOURCE  hSrcResource_;
	};

}

extern VOID (APIENTRY  ResourceConvert10_1)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hDstResource, 
											D3D10DDI_HRESOURCE  hSrcResource);
