#pragma once

#include "Command.h"

namespace Commands
{

	class ResourceConvertRegion10_1 : public CommandWithAllocator<ResourceConvertRegion10_1>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		ResourceConvertRegion10_1()
		:	pSrcBox_(0)
		{
			CommandId		= idResourceConvertRegion10_1;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ResourceConvertRegion10_1(
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
			CommandId		= idResourceConvertRegion10_1;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_	= hDstResource;
			DstX_			= DstX;
			DstY_			= DstY;
			DstZ_			= DstZ;
			hSrcResource_	= hSrcResource;
			SrcSubresource_	= SrcSubresource; 
			if(pSrcBox)
			{
				memcpy( &SrcBox_, pSrcBox, sizeof(D3D10_DDI_BOX) );
				pSrcBox_ = &SrcBox_;
			} else pSrcBox_ = NULL;
		}

		virtual bool IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void SetDestResourceType( TextureType type );
		virtual void GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HRESOURCE	hDstResource_; 
		UINT				DstSubresource_;
		UINT				DstX_;
		UINT				DstY_;
		UINT				DstZ_;
		D3D10DDI_HRESOURCE	hSrcResource_;
		UINT				SrcSubresource_; 
		D3D10_DDI_BOX* 		pSrcBox_;
		D3D10_DDI_BOX		SrcBox_;
	};

}

extern VOID (APIENTRY  ResourceConvertRegion10_1)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hDstResource, 
												  UINT  DstSubresource, UINT  DstX, UINT  DstY, UINT  DstZ, 
												  D3D10DDI_HRESOURCE  hSrcResource, UINT  SrcSubresource, 
												  CONST D3D10_DDI_BOX*  pSrcBox );
