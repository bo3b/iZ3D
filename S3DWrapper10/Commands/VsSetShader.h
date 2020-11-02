#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class VsSetShader : public CommandWithAllocator<VsSetShader, xxSetShader>
	{
	public:
		VsSetShader()
			: base_type( SP_VS )
		{
			CommandId = idVsSetShader;
		}

		VsSetShader( D3D10DDI_HSHADER  hShader )
			: base_type( SP_VS, hShader )
		{
			CommandId = idVsSetShader;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

VOID ( APIENTRY VsSetShader )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader );
