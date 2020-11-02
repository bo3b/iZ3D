#pragma once

#include "Command.h"

namespace Commands
{

	class SetViewports : public CommandWithAllocator<SetViewports>
	{
	public:
		typedef fast_vector<D3D10_DDI_VIEWPORT>::type	viewport_vector;

		static viewport_vector::allocator_type viewportAllocator;

	public:
		SetViewports()
		:	pViewports_(viewportAllocator)
		{
			CommandId = idSetViewports;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
		}

		SetViewports( UINT						NumViewports, 
					  UINT						ClearViewports, 
					  CONST D3D10_DDI_VIEWPORT*	pViewports )
		:	NumViewports_(NumViewports)
		,	pViewports_(pViewports, pViewports + NumViewports, viewportAllocator)
		{ 
			CommandId = idSetViewports;
			State_	= COMMAND_CHANGE_DEVICE_STATE;
			ClearViewports_ = ClearViewports;
		}

		virtual void Init()
		{
			NumViewports_ = 0;
			ClearViewports_ = 0;
			pViewports_.clear();
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile();
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );

		void BuildCommand(CDumpState *ds);

	public:
		UINT			NumViewports_; 
		UINT			ClearViewports_; 
		viewport_vector pViewports_;
	};

}

VOID ( APIENTRY SetViewports )( D3D10DDI_HDEVICE  hDevice, UINT  NumViewports, UINT  ClearViewports, 
							   CONST D3D10_DDI_VIEWPORT*  pViewports );

inline bool operator==(const D3D10_DDI_VIEWPORT& _Left, const D3D10_DDI_VIEWPORT& _Right)
{
	return _Left.TopLeftX == _Right.TopLeftX &&
		_Left.TopLeftY == _Right.TopLeftY &&
		_Left.Width == _Right.Width &&
		_Left.Height == _Right.Height &&
		_Left.MinDepth == _Right.MinDepth &&
		_Left.MaxDepth == _Right.MaxDepth;
}
