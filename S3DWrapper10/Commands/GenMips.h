#pragma once

#include "Command.h"

namespace Commands
{

	class GenMips : public CommandWithAllocator<GenMips>
	{
	public:
		GenMips()		
		{
			CommandId = idGenMips;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		GenMips( D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView )
		{
			CommandId = idGenMips;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hShaderResourceView_ = hShaderResourceView;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand	(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView_;

		DEPENDS_ON(hShaderResourceView_);
	};

}

extern VOID ( APIENTRY GenMips )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView );
