#pragma once

#include "Command.h"

namespace Commands
{

	class SetDisplayMode : public CommandWithAllocator<SetDisplayMode>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper * pWrapper ) const;
		virtual bool	ReadFromFile	();

		SetDisplayMode()
		{
			CommandId		= idSetDisplayMode;
		}

		SetDisplayMode( DXGI_DDI_ARG_SETDISPLAYMODE*  pDisplayModeData )
		{
			CommandId		= idSetDisplayMode;
			pDisplayModeData_ = pDisplayModeData;
			//MEMCOPY(&DisplayModeData_, pDisplayModeData, sizeof(DXGI_DDI_ARG_SETDISPLAYMODE));
			//DisplayModeData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_SETDISPLAYMODE*	pDisplayModeData_;
		//DXGI_DDI_ARG_SETDISPLAYMODE  DisplayModeData_;
		HRESULT							RetValue_;
	};

}

extern HRESULT ( APIENTRY  SetDisplayMode )  (DXGI_DDI_ARG_SETDISPLAYMODE*  pDisplayModeData);
