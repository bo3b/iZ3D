#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyDepthStencilView : public CommandWithAllocator<DestroyDepthStencilView>
	{
	public:
		EMPTY_OUTPUT();

		DestroyDepthStencilView()		
		{
			CommandId = idDestroyDepthStencilView;
		}

		DestroyDepthStencilView( D3D10DDI_HDEPTHSTENCILVIEW hDepthStencilView )
		{ 
			CommandId = idDestroyDepthStencilView;
			hDepthStencilView_ = hDepthStencilView;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HDEPTHSTENCILVIEW	hDepthStencilView_;
	};

}

extern VOID ( APIENTRY DestroyDepthStencilView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView );
