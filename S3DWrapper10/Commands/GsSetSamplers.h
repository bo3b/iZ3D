#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class GsSetSamplers : public CommandWithAllocator<GsSetSamplers, xxSetSamplers>
	{
	public:
		GsSetSamplers()
			:	base_type(SP_GS)
		{
			CommandId = idGsSetSamplers;
		}

		GsSetSamplers( UINT  Offset, UINT  NumSamplers, CONST D3D10DDI_HSAMPLER*  phSamplers )
			:	base_type( SP_GS, Offset, NumSamplers, phSamplers )
		{
			CommandId = idGsSetSamplers;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper ) override;
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const override;

		virtual TSetSamplersCmdPtr Clone() const override { return TSetSamplersCmdPtr(new GsSetSamplers(Offset_, NumValues_, &Values_[0])); }
	};

}

extern VOID ( APIENTRY GsSetSamplers )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumSamplers, 
									   CONST D3D10DDI_HSAMPLER*  phSamplers );

