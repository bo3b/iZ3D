#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class CsSetConstantBuffers11_0 : public CommandWithAllocator<CsSetConstantBuffers11_0, xxSetConstantBuffers>
	{
	public:
		CsSetConstantBuffers11_0()
		:	base_type(SP_CS)
		{
			CommandId	= idCsSetConstantBuffers11_0;
		}

		CsSetConstantBuffers11_0( UINT						StartBuffer, 
								  UINT						NumBuffers, 
								  CONST D3D10DDI_HRESOURCE* phBuffers )
		:	base_type(SP_CS, StartBuffer, NumBuffers, phBuffers)
		{
			CommandId = idCsSetConstantBuffers11_0;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const override { return TSetConstantBuffersCmdPtr(new CsSetConstantBuffers11_0(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY CsSetConstantBuffers11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  StartBuffer, 
	UINT  NumBuffers, 
	CONST D3D10DDI_HRESOURCE*  phBuffers
	);
