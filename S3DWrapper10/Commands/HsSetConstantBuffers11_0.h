#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class HsSetConstantBuffers11_0 : public CommandWithAllocator<HsSetConstantBuffers11_0, xxSetConstantBuffers>
	{
	public:
		HsSetConstantBuffers11_0()
		:	base_type(SP_HS)
		{
			CommandId	= idHsSetConstantBuffers11_0;
		}

		HsSetConstantBuffers11_0( UINT						StartBuffer, 
								  UINT						NumBuffers, 
								  CONST D3D10DDI_HRESOURCE* phBuffers )
		:	base_type( SP_HS, StartBuffer, NumBuffers, phBuffers )
		{
			CommandId = idHsSetConstantBuffers11_0;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const override { return TSetConstantBuffersCmdPtr(new HsSetConstantBuffers11_0(Offset_, NumValues_, &Values_[0])); }
		
		// Override Comman
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY HsSetConstantBuffers11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  StartBuffer, 
	UINT  NumBuffers, 
	CONST D3D10DDI_HRESOURCE*  phBuffers
	);
