#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class DsSetConstantBuffers11_0 : public CommandWithAllocator<DsSetConstantBuffers11_0, xxSetConstantBuffers>
	{
	public:
		DsSetConstantBuffers11_0()
		:	base_type(SP_DS)
		{
			CommandId = idDsSetConstantBuffers11_0;
		}

		DsSetConstantBuffers11_0( UINT						StartBuffer, 
								  UINT						NumBuffers, 
								  CONST D3D10DDI_HRESOURCE* phBuffers )
		:	base_type(SP_DS, StartBuffer, NumBuffers, phBuffers)
		{
			CommandId = idDsSetConstantBuffers11_0;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const override { return TSetConstantBuffersCmdPtr(new DsSetConstantBuffers11_0(Offset_, NumValues_, &Values_[0])); }
		
		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY DsSetConstantBuffers11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  StartBuffer, 
	UINT  NumBuffers, 
	CONST D3D10DDI_HRESOURCE*  phBuffers
	);
