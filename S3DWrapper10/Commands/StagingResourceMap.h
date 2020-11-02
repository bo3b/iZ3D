#pragma once

#include "xxMap.h"

namespace Commands
{

	class StagingResourceMap : public CommandWithAllocator<StagingResourceMap, xxMap>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		StagingResourceMap()
		{
			CommandId		= idStagingResourceMap;
		}

		StagingResourceMap(
			D3D10DDI_HRESOURCE	hResource, 
			UINT				Subresource, 
			D3D10_DDI_MAP		DDIMap, 
			UINT				Flags, 
			D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource
			)
		{
			CommandId		= idStagingResourceMap;
			hResource_		= hResource; 
			Subresource_	= Subresource; 
			DDIMap_			= DDIMap; 
			Flags_			= Flags; 
			pMappedSubResource_ = pMappedSubResource;			
		}	
	};

}

extern VOID ( APIENTRY StagingResourceMap )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hResource, 
	UINT  Subresource, 
	D3D10_DDI_MAP  DDIMap, 
	UINT  Flags, 
	D3D10DDI_MAPPED_SUBRESOURCE*  pMappedSubResource
	);
