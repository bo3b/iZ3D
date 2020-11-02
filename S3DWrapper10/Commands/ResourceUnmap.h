#pragma once

#include "xxUnmap.h"

namespace Commands
{

	class ResourceUnmap : public CommandWithAllocator<ResourceUnmap, xxUnmap>
	{
	public:
		ResourceUnmap()
		:	mem(0)
		,	memlen(0)
		,	databin_pos(0)
		{
			CommandId		= idResourceUnmap;
		}

		ResourceUnmap(
			D3D10DDI_HRESOURCE	hResource, 
			UINT				Subresource
			)
		:	mem(0)
		,	memlen(0)
		,	databin_pos(0)
		{
			CommandId		= idResourceUnmap;
			hResource_	= hResource; 
			Subresource_	= Subresource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual	bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		mutable char *mem;
		mutable DWORD memlen;
		int databin_pos;
	};

}

extern VOID ( APIENTRY ResourceUnmap )(
									   D3D10DDI_HDEVICE  hDevice, 
									   D3D10DDI_HRESOURCE  hResource, 
									   UINT  Subresource
									   );
