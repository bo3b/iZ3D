#pragma once

#include "xxSetShaderWithIfaces11_0.h"

namespace Commands
{

	class PsSetShaderWithIfaces11_0 : public CommandWithAllocator<PsSetShaderWithIfaces11_0, xxSetShaderWithIfaces11_0>
	{
	public:

		PsSetShaderWithIfaces11_0()
			: base_type( SP_PS )
		{
			CommandId = idPsSetShaderWithIfaces11_0;
		}

		PsSetShaderWithIfaces11_0( D3D10DDI_HSHADER  hShader, UINT  NumClassInstances, 
			const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData )
			: base_type( SP_PS, hShader, NumClassInstances, pIfaces,  pPointerData )
		{
			CommandId = idPsSetShaderWithIfaces11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual	void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY PsSetShaderWithIfaces11_0 )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader,
		UINT  NumClassInstances, const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData );
