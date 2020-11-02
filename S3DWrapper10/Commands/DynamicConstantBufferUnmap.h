#pragma once

#include "xxUnmap.h"

namespace Commands
{
	class DynamicConstantBufferUnmap : public CommandWithAllocator<DynamicConstantBufferUnmap, xxUnmap>
	{
	public:
		EMPTY_OUTPUT();

		DynamicConstantBufferUnmap()		
		{
			CommandId = idDynamicConstantBufferUnmap; 
		}

		DynamicConstantBufferUnmap( D3D10DDI_HRESOURCE  hResource, UINT  Subresource )
		{ 
			CommandId = idDynamicConstantBufferUnmap; 
			hResource_ = hResource;
			Subresource_ = Subresource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		mutable char *mem;
		mutable DWORD memlen;
		int databin_pos;
	};
}

extern VOID ( APIENTRY DynamicConstantBufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource );
