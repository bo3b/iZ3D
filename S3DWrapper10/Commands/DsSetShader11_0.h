#pragma once

#include "xxSetShader.h"

namespace Commands
{

	class DsSetShader11_0 : public CommandWithAllocator<DsSetShader11_0, xxSetShader>
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;

		DsSetShader11_0()
			: base_type( SP_DS )
		{
			CommandId = idDsSetShader11_0;
		}

		DsSetShader11_0( D3D10DDI_HSHADER	hShader )
			: base_type( SP_DS, hShader )
		{
			CommandId = idDsSetShader11_0;
		}

		virtual void UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY DsSetShader11_0 )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HSHADER  hShader
									 );
