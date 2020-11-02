#pragma once

#include "xxSetConstantBuffers.h"

namespace Commands
{

	class VsSetConstantBuffers : public CommandWithAllocator<VsSetConstantBuffers, xxSetConstantBuffers>
	{
	public:
		VsSetConstantBuffers()
		:	base_type(SP_VS)
		{
			CommandId = idVsSetConstantBuffers;
		}

		VsSetConstantBuffers( UINT						StartBuffer, 
							  UINT						NumBuffers, 
							  CONST D3D10DDI_HRESOURCE* phBuffers )
		:	base_type(SP_VS, StartBuffer, NumBuffers, phBuffers)
		{			
			CommandId = idVsSetConstantBuffers;
		}
		
		// Override xxSetConstantBuffers
		virtual TSetConstantBuffersCmdPtr Clone() const { return TSetConstantBuffersCmdPtr(new VsSetConstantBuffers(Offset_, NumValues_, &Values_[0])); }
		
		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
	};

}

VOID ( APIENTRY VsSetConstantBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
									   CONST D3D10DDI_HRESOURCE*  phBuffers );
