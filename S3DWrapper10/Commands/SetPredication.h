#pragma once

#include "Command.h"

namespace Commands
{

	class SetPredication : public CommandWithAllocator<SetPredication>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool	ReadFromFile	();

		SetPredication()
		{
			CommandId	= idSetPredication;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetPredication( D3D10DDI_HQUERY  hQuery, BOOL  PredicateValue )
		{
			CommandId	= idSetPredication;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			hQuery_	= hQuery;
			PredicateValue_	= PredicateValue;
		}

		virtual void Init()
		{
			hQuery_ = MAKE_D3D10DDI_HQUERY(NULL);
			PredicateValue_ = FALSE;
		}

		virtual void		UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

	public:

		D3D10DDI_HQUERY  hQuery_;
		BOOL  PredicateValue_;
	};

}

extern VOID ( APIENTRY SetPredication )(
										D3D10DDI_HDEVICE  hDevice, 
										D3D10DDI_HQUERY  hQuery, 
										BOOL  PredicateValue
										);
