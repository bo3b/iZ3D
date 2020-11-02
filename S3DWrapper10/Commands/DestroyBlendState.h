#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyBlendState : public CommandWithAllocator<DestroyBlendState>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		DestroyBlendState()		
		{
			CommandId = idDestroyBlendState;
		}

		DestroyBlendState( D3D10DDI_HBLENDSTATE hBlendState ) 
		{ 
			CommandId = idDestroyBlendState;
			hBlendState_ = hBlendState;
		}

		D3D10DDI_HBLENDSTATE hBlendState_;
	};

}

extern VOID ( APIENTRY DestroyBlendState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HBLENDSTATE hBlendState );
