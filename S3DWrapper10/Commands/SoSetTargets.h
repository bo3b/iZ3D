#pragma once

#include "Command.h"

namespace Commands
{

	class SoSetTargets : public CommandWithAllocator<SoSetTargets>
	{
	public:
		typedef fast_vector<D3D10DDI_HRESOURCE>::type	handle_vector;
		typedef fast_vector<UINT>::type					uint_vector;

		static const D3D10DDI_HRESOURCE			NULL_RESOURCE;
		static handle_vector::allocator_type	handleAllocator;
		static uint_vector::allocator_type		uintAllocator;

	public:

		SoSetTargets()
		:	phResource_(handleAllocator)
		,	pOffsets_(uintAllocator)
		{
			CommandId = idSoSetTargets;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SoSetTargets(UINT						SOTargets, 
					 UINT						ClearTargets, 
					 CONST D3D10DDI_HRESOURCE*	phResource, 
					 CONST UINT*				pOffsets)
		:	SOTargets_(SOTargets)
		,	ClearTargets_(ClearTargets)
		,	phResource_(phResource, phResource + SOTargets, handleAllocator)
		,	phTempResource_(SOTargets, NULL_RESOURCE, handleAllocator)
		,	pOffsets_(pOffsets, pOffsets + SOTargets, uintAllocator)
		{
			CommandId = idSoSetTargets;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		virtual void Init()
		{
			ClearTargets_ = 0;
			SOTargets_ = 0;
			phResource_.clear();
			pOffsets_.clear();
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:
		UINT			SOTargets_; 
		UINT			ClearTargets_; 
		handle_vector	phResource_;
		handle_vector	phTempResource_;
		uint_vector		pOffsets_;
	};

}

VOID ( APIENTRY SoSetTargets )( D3D10DDI_HDEVICE  hDevice, UINT  SOTargets, UINT  ClearTargets, 
							   CONST D3D10DDI_HRESOURCE*  phResource, CONST UINT*  pOffsets );
