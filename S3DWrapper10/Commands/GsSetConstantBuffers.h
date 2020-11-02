#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class GsSetConstantBuffers : public CommandWithAllocator<GsSetConstantBuffers, xxSetConstantBuffers> 
	{
	public:
		GsSetConstantBuffers()
		:	base_type(SP_GS)
		{
			CommandId = idGsSetConstantBuffers;
		}

		GsSetConstantBuffers( UINT						StartBuffer, 
							  UINT						NumBuffers, 
							  CONST D3D10DDI_HRESOURCE* phBuffers )
		:	base_type(SP_GS, StartBuffer, NumBuffers, phBuffers)
		{
			CommandId = idGsSetConstantBuffers;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const override { return TSetConstantBuffersCmdPtr(new GsSetConstantBuffers(Offset_, NumValues_, &Values_[0])); }
		
		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY GsSetConstantBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
											  CONST D3D10DDI_HRESOURCE*  phBuffers );
