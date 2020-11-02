#pragma once

#include "xxSetShaderWithIfaces11_0.h"

namespace Commands
{

	class DsSetShaderWithIfaces11_0 : public CommandWithAllocator<DsSetShaderWithIfaces11_0, xxSetShaderWithIfaces11_0> 
	{
	public:

		DsSetShaderWithIfaces11_0()
			: base_type( SP_DS )
		{
			CommandId = idDsSetShaderWithIfaces11_0;
		}

		DsSetShaderWithIfaces11_0( D3D10DDI_HSHADER  hShader, UINT  NumClassInstances, 
			const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData )
			: base_type( SP_DS, hShader, NumClassInstances, pIfaces,  pPointerData )
		{
			CommandId = idDsSetShaderWithIfaces11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY DsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader,
		UINT  NumClassInstances, const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData );
