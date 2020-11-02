#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{

	class DrawIndexed : public CommandWithAllocator<DrawIndexed,DrawBase>
	{
	public:
		DrawIndexed()
		{
			CommandId = idDrawIndexed;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		DrawIndexed( UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation )
		{
			CommandId = idDrawIndexed;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
			IndexCount_ = IndexCount;
			StartIndexLocation_ = StartIndexLocation;
			BaseVertexLocation_ = BaseVertexLocation;
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool	ReadFromFile();

		void BuildCommand(CDumpState *ds);

		UINT	IndexCount_;
		UINT	StartIndexLocation_;
		INT		BaseVertexLocation_;
	};

}

extern VOID ( APIENTRY DrawIndexed )( D3D10DDI_HDEVICE hDevice, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation );
