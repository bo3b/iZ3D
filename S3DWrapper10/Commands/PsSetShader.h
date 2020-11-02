#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class PsSetShader : public CommandWithAllocator<PsSetShader, xxSetShader>
	{
	public:
		PsSetShader()
			: base_type( SP_PS )
		{
			CommandId = idPsSetShader;
		}

		PsSetShader( D3D10DDI_HSHADER	hShader )
			: base_type( SP_PS, hShader )
		{
			CommandId = idPsSetShader;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY PsSetShader )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HSHADER  hShader
									 );
