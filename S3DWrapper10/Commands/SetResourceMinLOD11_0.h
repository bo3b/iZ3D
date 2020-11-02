#pragma once

#include "Command.h"

namespace Commands
{

	class SetResourceMinLOD11_0 : public CommandWithAllocator<SetResourceMinLOD11_0>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		SetResourceMinLOD11_0()		
		{
			CommandId = idSetResourceMinLOD11_0;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		SetResourceMinLOD11_0( D3D10DDI_HRESOURCE  hResource, FLOAT  MinLOD )
		{
			CommandId = idSetResourceMinLOD11_0;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hResource_ = hResource;
			MinLOD_ = MinLOD;
		}

		virtual bool IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;

	public:

		D3D10DDI_HRESOURCE  hResource_;
		FLOAT  MinLOD_;
	};

}

extern VOID ( APIENTRY SetResourceMinLOD11_0 )( D3D10DDI_HDEVICE  hDevice, 
											   D3D10DDI_HRESOURCE  hResource, FLOAT  MinLOD );
