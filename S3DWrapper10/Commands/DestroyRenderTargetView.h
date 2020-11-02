#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyRenderTargetView : public CommandWithAllocator<DestroyRenderTargetView>
	{
	public:
		EMPTY_OUTPUT();

		DestroyRenderTargetView()		
		{
			CommandId = idDestroyRenderTargetView;
		}

		DestroyRenderTargetView( D3D10DDI_HRENDERTARGETVIEW hRenderTargetView )
		{
			CommandId = idDestroyRenderTargetView;
			hRenderTargetView_ = hRenderTargetView;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HRENDERTARGETVIEW hRenderTargetView_;
	};

}

extern VOID ( APIENTRY DestroyRenderTargetView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRENDERTARGETVIEW hRenderTargetView );
