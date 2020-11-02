#pragma once

#include "Command.h"

namespace Commands
{

	class DestroySampler : public CommandWithAllocator<DestroySampler>
	{
	public:
		EMPTY_OUTPUT();

		DestroySampler()		
		{
			CommandId = idDestroySampler;
		}

		DestroySampler( D3D10DDI_HSAMPLER hSampler )
		{
			CommandId = idDestroySampler;
			hSampler_ = hSampler;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HSAMPLER hSampler_;
	};

}

extern VOID ( APIENTRY DestroySampler )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSAMPLER hSampler );
