#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class HsSetShader11_0 : public CommandWithAllocator<HsSetShader11_0, xxSetShader> 
	{
	public:

		HsSetShader11_0()
			: base_type( SP_HS )
		{
			CommandId = idHsSetShader11_0;
		}

		HsSetShader11_0( D3D10DDI_HSHADER	hShader )
			: base_type( SP_HS, hShader )
		{
			CommandId = idHsSetShader11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY HsSetShader11_0 )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HSHADER  hShader
									 );
