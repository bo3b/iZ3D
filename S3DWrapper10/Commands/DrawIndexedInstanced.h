#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{

	class DrawIndexedInstanced : public CommandWithAllocator<DrawIndexedInstanced,DrawBase>
	{
	public:
		DrawIndexedInstanced()		
		{
			CommandId = idDrawIndexedInstanced;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		DrawIndexedInstanced( UINT IndexCountPerInstance,
			UINT InstanceCount,
			UINT StartIndexLocation,
			INT BaseVertexLocation,
			UINT StartInstanceLocation)
		{
			CommandId = idDrawIndexedInstanced;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
			IndexCountPerInstance_ = IndexCountPerInstance;
			InstanceCount_ = InstanceCount;
			StartIndexLocation_ = StartIndexLocation;
			BaseVertexLocation_ = BaseVertexLocation;
			StartInstanceLocation_ = StartInstanceLocation;
		};

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool	ReadFromFile();

		void BuildCommand(CDumpState *ds);

		UINT	IndexCountPerInstance_;
		UINT	InstanceCount_;
		UINT	StartIndexLocation_;
		INT		BaseVertexLocation_;
		UINT	StartInstanceLocation_;
	};

}

extern VOID ( APIENTRY DrawIndexedInstanced )( D3D10DDI_HDEVICE hDevice, UINT IndexCountPerInstance, UINT InstanceCount,
											  UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation );
