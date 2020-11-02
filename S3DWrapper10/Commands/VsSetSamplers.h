#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class VsSetSamplers : public CommandWithAllocator<VsSetSamplers, xxSetSamplers>
	{
	public:
		VsSetSamplers()
			:	base_type(SP_VS)
		{
			CommandId = idVsSetSamplers;
		}

		VsSetSamplers( UINT  Offset, UINT  NumSamplers, CONST D3D10DDI_HSAMPLER*  phSamplers )
			:	base_type( SP_VS, Offset, NumSamplers, phSamplers )
		{
			CommandId = idVsSetSamplers;
		}

		virtual void Execute( D3DDeviceWrapper *pWrapper );

		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;

		virtual TSetSamplersCmdPtr Clone() const { return TSetSamplersCmdPtr(new VsSetSamplers(Offset_, NumValues_, &Values_[0])); }
	};

}

VOID ( APIENTRY VsSetSamplers )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumSamplers, 
								CONST D3D10DDI_HSAMPLER*  phSamplers );
