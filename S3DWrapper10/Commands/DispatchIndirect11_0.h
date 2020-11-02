#pragma once

#include "DispatchBase11_0.h"

namespace Commands
{

	class DispatchIndirect11_0 : public CommandWithAllocator<DispatchIndirect11_0, DispatchBase11_0>
	{
	public:

		DispatchIndirect11_0( )
		{
			CommandId = idDispatchIndirect11_0;
			State_	= COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE;
		}

		DispatchIndirect11_0( D3D10DDI_HRESOURCE  hBufferForArgs, 
			UINT  AlignedByteOffsetForArgs)
		{
			CommandId = idDispatchIndirect11_0;
			State_	= COMMAND_DEPENDED_ON_COMPUTE_DEVICE_STATE;
			hBufferForArgs_ = hBufferForArgs;
			AlignedByteOffsetForArgs_ = AlignedByteOffsetForArgs;
		}

		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool ReadFromFile();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10DDI_HRESOURCE  hBufferForArgs_; 
		UINT  AlignedByteOffsetForArgs_;
	};

}

extern VOID ( APIENTRY DispatchIndirect11_0 )( D3D10DDI_HDEVICE  hDevice, 
											  D3D10DDI_HRESOURCE  hBufferForArgs, 
											  UINT  AlignedByteOffsetForArgs );
