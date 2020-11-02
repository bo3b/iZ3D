#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class GsSetShader : public CommandWithAllocator<GsSetShader, xxSetShader>
	{
	public:
		GsSetShader()
			: base_type( SP_GS )
		{
			CommandId = idGsSetShader;
		}

		GsSetShader( D3D10DDI_HSHADER  hShader )
			: base_type( SP_GS, hShader )
		{
			CommandId = idGsSetShader;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY GsSetShader )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader );
