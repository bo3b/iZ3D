#pragma once

#include "Command.h"

namespace Commands
{

	class SetScissorRects : public CommandWithAllocator<SetScissorRects>
	{
	public:
		typedef fast_vector<D3D10_DDI_RECT>::type	rect_vector;

		static rect_vector::allocator_type	rectAllocator;

	public:
		SetScissorRects()
		:	pRects_(rectAllocator)
		{
			CommandId = idSetScissorRects;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetScissorRects( UINT					NumScissorRects, 
						 UINT					ClearScissorRects, 
						 CONST D3D10_DDI_RECT*  pRects )
		:	NumScissorRects_(NumScissorRects)
		,	ClearScissorRects_(ClearScissorRects)
		,	pRects_(pRects, pRects + NumScissorRects, rectAllocator)
		{
			CommandId = idSetScissorRects;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		virtual void Init()
		{
			NumScissorRects_ = 0;
			ClearScissorRects_ = 0;
			pRects_.clear();
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;  
		virtual bool	ReadFromFile();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:
		UINT		NumScissorRects_; 
		UINT		ClearScissorRects_; 
		rect_vector	pRects_;
	};

}

VOID ( APIENTRY SetScissorRects )( D3D10DDI_HDEVICE  hDevice, UINT  NumScissorRects, UINT  ClearScissorRects, 
								  CONST D3D10_DDI_RECT*  pRects );
