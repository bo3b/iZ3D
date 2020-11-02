#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceCopy : public CommandWithAllocator<ResourceCopy>
	{
	public:
		ResourceCopy()		
		{
			CommandId = idResourceCopy;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceCopy( D3D10DDI_HRESOURCE	hDstResource, D3D10DDI_HRESOURCE	hSrcResource )
		{
			CommandId = idResourceCopy;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_	= hDstResource; 
			hSrcResource_	= hSrcResource;
		}

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

		D3D10DDI_HRESOURCE	hDstResource_; 
		D3D10DDI_HRESOURCE	hSrcResource_;

		DEPENDS_ON2(hDstResource_,hSrcResource_);
	};

}

extern VOID ( APIENTRY ResourceCopy )(
									  D3D10DDI_HDEVICE  hDevice, 
									  D3D10DDI_HRESOURCE  hDstResource, 
									  D3D10DDI_HRESOURCE  hSrcResource
									  );
