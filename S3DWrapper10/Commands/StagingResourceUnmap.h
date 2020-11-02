#pragma once

#include "xxUnmap.h"

namespace Commands
{

	class StagingResourceUnmap : public CommandWithAllocator<StagingResourceUnmap, xxUnmap>
	{
	public:
		StagingResourceUnmap()		
		{
			CommandId		= idStagingResourceUnmap;
		}

		StagingResourceUnmap(
			D3D10DDI_HRESOURCE	hResource, 
			UINT				Subresource
			)
		{
			CommandId		= idStagingResourceUnmap;
			hResource_		= hResource; 
			Subresource_	= Subresource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );		
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);
	};

}

extern VOID ( APIENTRY StagingResourceUnmap )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hResource, 
	UINT  Subresource
	);
