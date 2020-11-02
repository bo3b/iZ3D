#pragma once

#include "Command.h"

namespace Commands
{

	class ShaderResourceViewReadAfterWriteHazard : public CommandWithAllocator<ShaderResourceViewReadAfterWriteHazard>
	{
	public:
		EMPTY_OUTPUT();

		ShaderResourceViewReadAfterWriteHazard()
		{
			CommandId = idShaderResourceViewReadAfterWriteHazard;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ShaderResourceViewReadAfterWriteHazard(    
			D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView,
			D3D10DDI_HRESOURCE  hResource
			)
		{
			CommandId = idShaderResourceViewReadAfterWriteHazard;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hShaderResourceView_ = hShaderResourceView;
			hResource_ = hResource;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView_;
		D3D10DDI_HRESOURCE            hResource_;
	};

}

VOID ( APIENTRY ShaderResourceViewReadAfterWriteHazard )( D3D10DDI_HDEVICE  hDevice, 
														 D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRESOURCE  hResource );
