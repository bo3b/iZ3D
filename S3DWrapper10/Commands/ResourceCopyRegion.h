#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceCopyRegion : public CommandWithAllocator<ResourceCopyRegion>
	{
	public:
		ResourceCopyRegion()		
		{
			CommandId = idResourceCopyRegion;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceCopyRegion(
			D3D10DDI_HRESOURCE	hDstResource, 
			UINT				DstSubresource, 
			UINT				DstX, 
			UINT				DstY, 
			UINT				DstZ, 
			D3D10DDI_HRESOURCE	hSrcResource, 
			UINT				SrcSubresource, 
			CONST D3D10_DDI_BOX* pSrcBox
			)
		{
			CommandId = idResourceCopyRegion;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_	= hDstResource; 
			DstSubresource_	= DstSubresource; 
			DstX_			= DstX; 
			DstY_			= DstY; 
			DstZ_			= DstZ; 
			hSrcResource_	= hSrcResource; 
			SrcSubresource_	= SrcSubresource;
			if( pSrcBox ) 
			{
				memcpy( &SrcBox_, pSrcBox, sizeof(D3D10_DDI_BOX) );
				pSrcBox_ = &SrcBox_;
			} else pSrcBox_ = NULL;
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
		UINT				DstX_; 
		UINT				DstY_; 
		UINT				DstZ_; 
		D3D10DDI_HRESOURCE	hSrcResource_; 
		UINT				SrcSubresource_; 
		D3D10_DDI_BOX		SrcBox_;
		D3D10_DDI_BOX*		pSrcBox_;

		DEPENDS_ON2(hDstResource_,hSrcResource_);
	};

}

extern VOID ( APIENTRY ResourceCopyRegion )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hDstResource, 
	UINT  DstSubresource, 
	UINT  DstX, 
	UINT  DstY, 
	UINT  DstZ, 
	D3D10DDI_HRESOURCE  hSrcResource, 
	UINT  SrcSubresource, 
	CONST D3D10_DDI_BOX*  pSrcBox
	);
