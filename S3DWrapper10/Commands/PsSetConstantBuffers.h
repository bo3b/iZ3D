#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class PsSetConstantBuffers : public CommandWithAllocator<PsSetConstantBuffers, xxSetConstantBuffers>
	{
	public:
		PsSetConstantBuffers()
		:	base_type(SP_PS)
		{
			CommandId	= idPsSetConstantBuffers;
		}

		PsSetConstantBuffers(UINT		StartBuffer, 
							 UINT		NumBuffers, 
							 CONST D3D10DDI_HRESOURCE* phBuffers)
		:	base_type(SP_PS, StartBuffer, NumBuffers, phBuffers)
		{
			CommandId = idPsSetConstantBuffers;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const override { return TSetConstantBuffersCmdPtr(new PsSetConstantBuffers(Offset_, NumValues_, &Values_[0])); }
		
		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY PsSetConstantBuffers )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  StartBuffer, 
	UINT  NumBuffers, 
	CONST D3D10DDI_HRESOURCE*  phBuffers
	);
