#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class HsSetSamplers11_0 : public CommandWithAllocator<HsSetSamplers11_0, xxSetSamplers> 
	{
	public:
		HsSetSamplers11_0()
			:	base_type(SP_HS)
		{
			CommandId = idHsSetSamplers11_0;
		}

		HsSetSamplers11_0(
			UINT			Offset, 
			UINT			NumSamplers, 
			CONST D3D10DDI_HSAMPLER* phSamplers
			)
			:	base_type( SP_HS, Offset, NumSamplers, phSamplers )
		{
			CommandId = idHsSetSamplers11_0;
		}

		virtual		void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual		bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;

		virtual TSetSamplersCmdPtr Clone() const { return TSetSamplersCmdPtr(new HsSetSamplers11_0(Offset_, NumValues_, &Values_[0])); }
	};

}

extern VOID ( APIENTRY HsSetSamplers11_0 )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  Offset, 
									   UINT  NumSamplers, 
									   CONST D3D10DDI_HSAMPLER*  phSamplers
									   );
