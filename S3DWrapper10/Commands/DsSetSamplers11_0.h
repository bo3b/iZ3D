#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class DsSetSamplers11_0 : public CommandWithAllocator<DsSetSamplers11_0, xxSetSamplers>
	{
	public:
		DsSetSamplers11_0()
			:	base_type(SP_DS)
		{
			CommandId = idDsSetSamplers11_0;
		}

		DsSetSamplers11_0(
			UINT			Offset, 
			UINT			NumSamplers, 
			CONST D3D10DDI_HSAMPLER* phSamplers
			)
			:	base_type( SP_DS, Offset, NumSamplers, phSamplers )
		{
			CommandId = idDsSetSamplers11_0;
		}

		virtual		void	Execute		( D3DDeviceWrapper *pWrapper ) override;
		virtual		bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const override;

		virtual TSetSamplersCmdPtr Clone() const override { return TSetSamplersCmdPtr(new DsSetSamplers11_0(Offset_, NumValues_, &Values_[0])); }
	};

}

extern VOID ( APIENTRY DsSetSamplers11_0 )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  Offset, 
									   UINT  NumSamplers, 
									   CONST D3D10DDI_HSAMPLER*  phSamplers
									   );
