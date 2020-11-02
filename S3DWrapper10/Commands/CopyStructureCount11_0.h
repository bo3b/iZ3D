#pragma once

#include "Command.h"

namespace Commands
{

	class CopyStructureCount11_0 : public CommandWithAllocator<CopyStructureCount11_0>
	{
	public:
		CopyStructureCount11_0()		
		{
			CommandId = idCopyStructureCount11_0;
		}

		CopyStructureCount11_0( D3D10DDI_HRESOURCE  hDstBuffer, UINT  DstAlignedByteOffset, 
			D3D11DDI_HUNORDEREDACCESSVIEW  hSrcView  )
		{
			CommandId = idCopyStructureCount11_0;
			hDstBuffer_	= hDstBuffer; 
			DstAlignedByteOffset_	= DstAlignedByteOffset;
			hSrcView_	= hSrcView;
		}

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HRESOURCE  hDstBuffer_;
		UINT  DstAlignedByteOffset_;
		D3D11DDI_HUNORDEREDACCESSVIEW  hSrcView_;

		DEPENDS_ON2(hDstBuffer_,hSrcView_);
	};

}

extern VOID ( APIENTRY CopyStructureCount11_0 )(
										D3D10DDI_HDEVICE  hDevice, 
										D3D10DDI_HRESOURCE  hDstBuffer, 
										UINT  DstAlignedByteOffset,
										D3D11DDI_HUNORDEREDACCESSVIEW  hSrcView 
									  );
