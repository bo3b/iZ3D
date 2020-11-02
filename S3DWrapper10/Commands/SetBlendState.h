#pragma once

#include "Command.h"

namespace Commands
{

	class SetBlendState : public CommandWithAllocator<SetBlendState>
	{
	public:
		SetBlendState()
		{
			CommandId = idSetBlendState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetBlendState(     
			D3D10DDI_HBLENDSTATE  hState, 
			const FLOAT  pBlendFactor[4], 
			UINT  SampleMask
			)
		{
			CommandId = idSetBlendState;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hState_ = hState;
			memcpy(pBlendFactor_, pBlendFactor, 4 * sizeof(FLOAT));
			SampleMask_ = SampleMask;
		}

		virtual void Init()
		{
			hState_ = MAKE_D3D10DDI_HBLENDSTATE(NULL);
			pBlendFactor_[0] = 0.0f;
			pBlendFactor_[1] = 0.0f;
			pBlendFactor_[2] = 0.0f;
			pBlendFactor_[3] = 0.0f;
			SampleMask_ = 0xffffffff;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HBLENDSTATE  hState_; 
		FLOAT  pBlendFactor_[4];
		UINT  SampleMask_;

		DEPENDS_ON(hState_);
	};

}

extern VOID ( APIENTRY SetBlendState )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HBLENDSTATE  hState, 
									   const FLOAT pBlendFactor[4], UINT  SampleMask );
