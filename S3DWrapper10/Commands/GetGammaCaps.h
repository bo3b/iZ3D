#pragma once

#include "Command.h"

namespace Commands
{

	class GetGammaCaps : public CommandWithAllocator<GetGammaCaps>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper * pWrapper ) const;
		virtual bool	ReadFromFile	();

		GetGammaCaps()		
		{
			CommandId = idGetGammaCaps;
		}

		GetGammaCaps( DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS*  pGammaData )
		{
			CommandId = idGetGammaCaps;
			pGammaData_ = pGammaData;
			//MEMCOPY(&GammaData_, pGammaData, sizeof(DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS));
			//GammaData_.hDevice = hDevice;
		}

	public:
		DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS*	pGammaData_;
		//DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS  GammaData_;
		HRESULT									RetValue_;
	};

}

extern HRESULT ( APIENTRY  GetGammaCaps )  (DXGI_DDI_ARG_GET_GAMMA_CONTROL_CAPS*  pGammaData);
