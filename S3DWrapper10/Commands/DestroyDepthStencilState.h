#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyDepthStencilState : public CommandWithAllocator<DestroyDepthStencilState>
	{
	public:
		EMPTY_OUTPUT();

		DestroyDepthStencilState()		
		{
			CommandId = idDestroyDepthStencilState;
		}

		DestroyDepthStencilState( D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState )
		{ 
			CommandId = idDestroyDepthStencilState;
			hDepthStencilState_ = hDepthStencilState;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HDEPTHSTENCILSTATE	hDepthStencilState_;
	};

}

extern VOID ( APIENTRY DestroyDepthStencilState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HDEPTHSTENCILSTATE hDepthStencilState );
