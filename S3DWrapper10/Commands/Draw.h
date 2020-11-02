#pragma once

#include "Command.h"
#include "drawbase.h"

namespace Commands
{
	class Draw : public CommandWithAllocator<Draw,DrawBase>
	{
	public:
		Draw()
		{
			CommandId = idDraw;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
		}

		Draw( UINT VertexCount, UINT StartVertexLocation ) 
		{
			CommandId = idDraw;
			State_	= COMMAND_DEPENDED_ON_DEVICE_STATE;
			VertexCount_ = VertexCount;
			StartVertexLocation_= StartVertexLocation;
		}

		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;		
		virtual bool	ReadFromFile();

		void BuildCommand(CDumpState *ds);

		UINT VertexCount_;
		UINT StartVertexLocation_;
	};

}

extern VOID ( APIENTRY Draw )( D3D10DDI_HDEVICE hDevice, UINT VertexCount, UINT StartVertexLocation );
