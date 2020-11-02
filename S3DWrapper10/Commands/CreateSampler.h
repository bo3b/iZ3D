#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateSampler : public CommandWithAllocator<CreateSampler>
	{
	public:
		CreateSampler()		
		{
			CommandId = idCreateSampler;
		}

		CreateSampler(const D3D10_DDI_SAMPLER_DESC*  pSamplerDesc, 
			D3D10DDI_HSAMPLER  hSampler, 
			D3D10DDI_HRTSAMPLER  hRTSampler)
		{
			CommandId = idCreateSampler;
			memcpy(&SamplerDesc_, pSamplerDesc, sizeof( D3D10_DDI_SAMPLER_DESC ));

			hSampler_ = hSampler;
			hRTSampler_ = hRTSampler;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10_DDI_SAMPLER_DESC		SamplerDesc_; 
		D3D10DDI_HSAMPLER			hSampler_;
		D3D10DDI_HRTSAMPLER			hRTSampler_;

		CREATES(hSampler_);
	};

}

VOID (APIENTRY  CreateSampler)(D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_SAMPLER_DESC*  pSamplerDesc, 
							   D3D10DDI_HSAMPLER  hSampler, D3D10DDI_HRTSAMPLER  hRTSampler);
