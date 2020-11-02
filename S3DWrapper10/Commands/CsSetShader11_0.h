#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class CsSetShader11_0 : public CommandWithAllocator<CsSetShader11_0, xxSetShader>
	{
	public:

		CsSetShader11_0()
			: base_type( SP_CS )
		{
			CommandId = idCsSetShader11_0;
		}

		CsSetShader11_0( D3D10DDI_HSHADER	hShader )
			: base_type( SP_CS, hShader )
		{
			CommandId = idCsSetShader11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY CsSetShader11_0 )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HSHADER  hShader
									 );
