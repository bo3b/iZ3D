#pragma once

#include "xxSetShaderWithIfaces11_0.h"

namespace Commands
{

	class VsSetShaderWithIfaces11_0 : public CommandWithAllocator<VsSetShaderWithIfaces11_0, xxSetShaderWithIfaces11_0> 
	{
	public:

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;

		VsSetShaderWithIfaces11_0()
			: base_type( SP_VS )
		{
			CommandId = idVsSetShaderWithIfaces11_0;
		}

		VsSetShaderWithIfaces11_0( D3D10DDI_HSHADER  hShader, UINT  NumClassInstances, 
			const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData )
			: base_type( SP_VS, hShader, NumClassInstances, pIfaces,  pPointerData )
		{
			CommandId = idVsSetShaderWithIfaces11_0;
		}

		virtual void UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

VOID ( APIENTRY VsSetShaderWithIfaces11_0 )(  D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader,
		UINT  NumClassInstances, const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData  );
