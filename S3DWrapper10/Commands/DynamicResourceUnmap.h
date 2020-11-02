#pragma once

#include "xxUnmap.h"

namespace Commands
{
	class DynamicResourceUnmap : public CommandWithAllocator<DynamicResourceUnmap, xxUnmap>
	{
	public:
		DynamicResourceUnmap()		
		{
			CommandId = idDynamicResourceUnmap;
		}

		DynamicResourceUnmap( D3D10DDI_HRESOURCE hResource, UINT Subresource )
		{ 
			CommandId = idDynamicResourceUnmap; 
			hResource_ = hResource;
			Subresource_ = Subresource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);
	};
}

VOID ( APIENTRY DynamicResourceUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource );
