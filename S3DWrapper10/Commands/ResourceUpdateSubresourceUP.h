#pragma once

#include "xxUpdateSubresourceUP.h"
#include <memory.h>

namespace Commands
{

	class ResourceUpdateSubresourceUP : public CommandWithAllocator<ResourceUpdateSubresourceUP, xxUpdateSubresourceUP>
	{
	public:
		ResourceUpdateSubresourceUP()
		{
			CommandId		= idResourceUpdateSubresourceUP;
		}

		ResourceUpdateSubresourceUP(
			D3D10DDI_HRESOURCE	hDstResource, 
			UINT				DstSubresource, 
			CONST D3D10_DDI_BOX* pDstBox, 
			CONST VOID*			pSysMemUP, 
			UINT				RowPitch, 
			UINT				DepthPitch
			)
		{
			CommandId		= idResourceUpdateSubresourceUP;
			hDstResource_	= hDstResource;
			DstSubresource_	= DstSubresource;
			pSysMemUP_		= pSysMemUP;
			RowPitch_		= RowPitch;
			DepthPitch_		= DepthPitch;
			if(pDstBox)
			{
				memcpy(&DstBox_, pDstBox, sizeof(D3D10_DDI_BOX));
				pDstBox_ = &DstBox_;
			}
			else pDstBox_ = NULL;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );

		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;

		DEPENDS_ON(hDstResource_);
	};

}

extern VOID ( APIENTRY ResourceUpdateSubresourceUP )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hDstResource, 
	UINT  DstSubresource, 
	CONST D3D10_DDI_BOX*  pDstBox, 
	CONST VOID*  pSysMemUP, 
	UINT  RowPitch, 
	UINT  DepthPitch
	);
