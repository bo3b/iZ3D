#pragma once

#include "xxSetShaderWithIfaces11_0.h"

namespace Commands
{

	class CsSetShaderWithIfaces11_0 : public CommandWithAllocator<CsSetShaderWithIfaces11_0, xxSetShaderWithIfaces11_0>
	{
	public:
		
		CsSetShaderWithIfaces11_0()
			: base_type( SP_CS )
		{
			CommandId = idCsSetShaderWithIfaces11_0;
		}

		CsSetShaderWithIfaces11_0( D3D10DDI_HSHADER  hShader, UINT  NumClassInstances, 
			const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData )
			: base_type( SP_CS, hShader, NumClassInstances, pIfaces, pPointerData )
		{
			CommandId = idCsSetShaderWithIfaces11_0;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState );
	};

}

extern VOID ( APIENTRY CsSetShaderWithIfaces11_0 )(
									 D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADER  hShader, UINT  NumClassInstances, 
									 const UINT*  pIfaces, const D3D11DDIARG_POINTERDATA*  pPointerData
									 );
