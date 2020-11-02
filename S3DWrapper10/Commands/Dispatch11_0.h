#pragma once

#include "DispatchBase11_0.h"

namespace Commands
{

	class Dispatch11_0 : public CommandWithAllocator<Dispatch11_0, DispatchBase11_0>
	{
	public:

		Dispatch11_0( )
		{
			CommandId = idDispatch11_0;
			State_	= COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE;
		}

		Dispatch11_0( UINT  ThreadGroupCountX, UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ )
		{
			CommandId = idDispatch11_0;
			State_	= COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE;
			ThreadGroupCountX_ = ThreadGroupCountX;
			ThreadGroupCountY_ = ThreadGroupCountY;
			ThreadGroupCountZ_ = ThreadGroupCountZ;
		}

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile();

		void BuildCommand(CDumpState *ds);

	public:
		UINT  ThreadGroupCountX_;
		UINT  ThreadGroupCountY_;
		UINT  ThreadGroupCountZ_;
	};

}

extern VOID ( APIENTRY Dispatch11_0 )( D3D10DDI_HDEVICE  hDevice, UINT  ThreadGroupCountX, 
									  UINT  ThreadGroupCountY, UINT  ThreadGroupCountZ );
