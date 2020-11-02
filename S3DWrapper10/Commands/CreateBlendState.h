#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateBlendState : public CommandWithAllocator<CreateBlendState>
	{
	public:
		CreateBlendState()		
		{
			CommandId	  = idCreateBlendState;
		}

		CreateBlendState(const D3D10_DDI_BLEND_DESC*  pBlendDesc, D3D10DDI_HBLENDSTATE  hBlendState, D3D10DDI_HRTBLENDSTATE  hRTBlendState)
		{
			CommandId	  = idCreateBlendState;
			memcpy(&BlendDesc_, pBlendDesc, sizeof(D3D10_DDI_BLEND_DESC));

			hBlendState_ = hBlendState;
			hRTBlendState_ = hRTBlendState;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const {	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10_DDI_BLEND_DESC  BlendDesc_;
		D3D10DDI_HBLENDSTATE  hBlendState_;
		D3D10DDI_HRTBLENDSTATE  hRTBlendState_;

		CREATES(hBlendState_);
	};

}

VOID ( APIENTRY CreateBlendState )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_BLEND_DESC*  pBlendDesc, 
								   D3D10DDI_HBLENDSTATE  hBlendState, D3D10DDI_HRTBLENDSTATE  hRTBlendState );
