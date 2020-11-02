#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{

	class DrawInstanced : public CommandWithAllocator<DrawInstanced,DrawBase>
	{
	public:
		DrawInstanced()		
		{
			CommandId = idDrawIndexedInstanced;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		DrawInstanced(UINT VertexCountPerInstance,
			UINT InstanceCount,
			UINT StartVertexLocation,
			UINT StartInstanceLocation)
		{
			CommandId = idDrawIndexedInstanced;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
			VertexCountPerInstance_ = VertexCountPerInstance;
			InstanceCount_ = InstanceCount;
			StartVertexLocation_ = StartVertexLocation;
			StartInstanceLocation_ = StartInstanceLocation;
		}

		virtual void Execute( D3DDeviceWrapper *pWrapper );

		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool ReadFromFile();

		UINT VertexCountPerInstance_;
		UINT InstanceCount_;
		UINT StartVertexLocation_;
		UINT StartInstanceLocation_;
	};

}

extern VOID ( APIENTRY DrawInstanced )( D3D10DDI_HDEVICE hDevice, UINT VertexCountPerInstance, UINT InstanceCount, 
									   UINT StartVertexLocation, UINT StartInstanceLocation );
