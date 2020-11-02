#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceResolveSubresource : public CommandWithAllocator<ResourceResolveSubresource>
	{
	public:
		ResourceResolveSubresource()		
		{
			CommandId		= idResourceResolveSubresource;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceResolveSubresource(
			D3D10DDI_HRESOURCE	hDstResource, 
			UINT				DstSubresource, 
			D3D10DDI_HRESOURCE	hSrcResource, 
			UINT				SrcSubresource, 
			DXGI_FORMAT			ResolveFormat
			)
		{
			CommandId		= idResourceResolveSubresource;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_	= hDstResource; 
			DstSubresource_	= DstSubresource; 
			hSrcResource_	= hSrcResource; 
			SrcSubresource_	= SrcSubresource;
			ResolveFormat_	= ResolveFormat;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HRESOURCE	hDstResource_; 
		UINT				DstSubresource_; 
		D3D10DDI_HRESOURCE	hSrcResource_; 
		UINT				SrcSubresource_; 
		DXGI_FORMAT			ResolveFormat_;

		DEPENDS_ON2(hDstResource_,hSrcResource_);
	};

}

extern VOID ( APIENTRY ResourceResolveSubresource )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hDstResource, 
	UINT  DstSubresource, 
	D3D10DDI_HRESOURCE  hSrcResource, 
	UINT  SrcSubresource, 
	DXGI_FORMAT  ResolveFormat
	);
