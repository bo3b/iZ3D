#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class ClearDepthStencilView : public CommandWithAllocator<ClearDepthStencilView>
	{
	public:
		ClearDepthStencilView()
		{
			CommandId = idClearDepthStencilView;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
		}

		ClearDepthStencilView(D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, unsigned int Flags, float Depth, unsigned char Stencil)
		{    
			CommandId = idClearDepthStencilView;
			State_	= COMMAND_MAY_USE_STEREO_RESOURCES;
			hDepthStencilView_ = hDepthStencilView;
			Flags_ = Flags;
			Depth_ = Depth;
			Stencil_ = Stencil;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView_;
		UINT Flags_;
		FLOAT Depth_;
		UINT8 Stencil_;

		DEPENDS_ON(hDepthStencilView_);
	};

}

VOID ( APIENTRY ClearDepthStencilView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, 
										UINT  Flags, FLOAT  Depth, UINT8  Stencil );
