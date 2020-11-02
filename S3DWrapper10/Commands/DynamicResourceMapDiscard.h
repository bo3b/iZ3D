#pragma once

#include "xxMap.h"

namespace Commands
{
	class DynamicResourceMapDiscard : public CommandWithAllocator<DynamicResourceMapDiscard, xxMap>
	{
	public:
		EMPTY_OUTPUT();

		DynamicResourceMapDiscard()		
		{
			CommandId = idDynamicResourceMapDiscard; 
		}

		DynamicResourceMapDiscard( D3D10DDI_HRESOURCE hResource,
			UINT Subresource,
			D3D10_DDI_MAP DDIMap,
			UINT Flags,
			D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
		{ 
			CommandId = idDynamicResourceMapDiscard; 
			hResource_ = hResource;
			Subresource_ = Subresource;
			DDIMap_ = DDIMap;
			Flags_ = Flags;
			pMappedSubResource_ = pMappedSubResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );

		virtual bool	WriteToFile		( D3DDeviceWrapper* pWrapper ) const;
		virtual bool	ReadFromFile	();
	};
}

VOID ( APIENTRY DynamicResourceMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource,
											D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource );
