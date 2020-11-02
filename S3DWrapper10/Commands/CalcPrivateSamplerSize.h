#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateSamplerSize : public CommandWithAllocator<CalcPrivateSamplerSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile(); 

		CalcPrivateSamplerSize ()		
		{
			CommandId = idCalcPrivateSamplerSize;
		}

		CalcPrivateSamplerSize( CONST D3D10_DDI_SAMPLER_DESC* pSamplerDesc)
		{
			CommandId = idCalcPrivateSamplerSize;
			memcpy(&SamplerDesc_, pSamplerDesc, sizeof(D3D10_DDI_SAMPLER_DESC));
		}

	public:

		D3D10_DDI_SAMPLER_DESC  SamplerDesc_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

extern SIZE_T ( APIENTRY CalcPrivateSamplerSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_SAMPLER_DESC*  pSamplerDesc );
