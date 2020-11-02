#pragma once

#include "xxUnmap.h"

namespace Commands
{
	class DynamicIABufferUnmap : public CommandWithAllocator<DynamicIABufferUnmap, xxUnmap>
	{
	public:
		DynamicIABufferUnmap()		
		{
			CommandId = idDynamicIABufferUnmap; 
		}

		DynamicIABufferUnmap( D3D10DDI_HRESOURCE hResource, UINT Subresource )
		{ 
			CommandId = idDynamicIABufferUnmap; 
			hResource_ = hResource;
			Subresource_ = Subresource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand	(CDumpState *ds);
	};
}

VOID ( APIENTRY DynamicIABufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource );
