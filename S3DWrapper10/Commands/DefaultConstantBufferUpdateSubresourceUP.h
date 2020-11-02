#pragma once

#include "xxUpdateSubresourceUP.h"

namespace Commands
{

	class DefaultConstantBufferUpdateSubresourceUP : public CommandWithAllocator<DefaultConstantBufferUpdateSubresourceUP, xxUpdateSubresourceUP>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		DefaultConstantBufferUpdateSubresourceUP()
		{
			CommandId = idDefaultConstantBufferUpdateSubresourceUP;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		DefaultConstantBufferUpdateSubresourceUP( D3D10DDI_HRESOURCE	hDstResource,
			UINT DstSubresource,
			CONST D3D10_DDI_BOX* pDstBox,
			CONST VOID* pSysMemUP,
			UINT RowPitch,
			UINT DepthPitch )
		{
			CommandId       = idDefaultConstantBufferUpdateSubresourceUP;
			State_	        = COMMAND_MAY_USE_STEREO_RESOURCES;
			hDstResource_   = hDstResource;
			DstSubresource_ = DstSubresource;
			pSysMemUP_      = pSysMemUP;
			RowPitch_       = RowPitch;
			DepthPitch_     = DepthPitch;
			if(pDstBox)
			{
				memcpy(&DstBox_, pDstBox, sizeof(D3D10_DDI_BOX));
				pDstBox_ = &DstBox_;
			}
			else pDstBox_ = NULL;
		}

		bool IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
	};

}

extern VOID ( APIENTRY DefaultConstantBufferUpdateSubresourceUP )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, 
																  UINT DstSubresource, CONST D3D10_DDI_BOX* pDstBox, CONST VOID* pSysMemUP, UINT RowPitch, UINT DepthPitch );

