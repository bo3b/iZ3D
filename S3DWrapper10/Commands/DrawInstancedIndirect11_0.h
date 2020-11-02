#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{

	class DrawInstancedIndirect11_0 : public CommandWithAllocator<DrawInstancedIndirect11_0,DrawBase>
	{
	public:
		DrawInstancedIndirect11_0()		
		{
			CommandId = idDrawIndexedInstancedIndirect11_0;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		DrawInstancedIndirect11_0(D3D10DDI_HRESOURCE  hBufferForArgs, 
			UINT  AlignedByteOffsetForArgs)
		{
			CommandId = idDrawIndexedInstancedIndirect11_0;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
			hBufferForArgs_ = hBufferForArgs;
			AlignedByteOffsetForArgs_ = AlignedByteOffsetForArgs;
		}

		virtual void Execute( D3DDeviceWrapper *pWrapper );

		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool ReadFromFile();

		D3D10DDI_HRESOURCE	hBufferForArgs_;
		UINT				AlignedByteOffsetForArgs_;
	};

}

extern VOID ( APIENTRY DrawInstancedIndirect11_0 )( D3D10DDI_HDEVICE hDevice, 
	D3D10DDI_HRESOURCE	hBufferForArgs, UINT AlignedByteOffsetForArgs );
