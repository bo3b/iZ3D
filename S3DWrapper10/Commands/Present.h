#pragma once

#include "Command.h"

namespace Commands
{

	class Present : public CommandWithAllocator<Present>
	{
	public:
		static DXGI_DDI_HRESOURCE hRealSurfaceToPresent;
		static DXGI_DDI_HRESOURCE hLastKMSurfaceToPresent;
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		Present()
		{
			CommandId = idPresent;
			pPresentData_ = NULL;
		}

		Present( DXGI_DDI_ARG_PRESENT*  pPresentData )
		{
			CommandId = idPresent;
			pPresentData_ = pPresentData;
			//MEMCOPY(&PresentData_, pPresentData, sizeof(DXGI_DDI_ARG_PRESENT));
			//PresentData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_PRESENT*	pPresentData_;
		//DXGI_DDI_ARG_PRESENT  PresentData_;
		HRESULT					RetValue_;
	};

}

extern HRESULT ( APIENTRY  Present )  (DXGI_DDI_ARG_PRESENT* pPresentData);
