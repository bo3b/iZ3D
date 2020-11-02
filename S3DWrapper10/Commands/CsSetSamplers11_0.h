#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class CsSetSamplers11_0 : public CommandWithAllocator<CsSetSamplers11_0, xxSetSamplers>
	{
	public:
		CsSetSamplers11_0()
			:	base_type(SP_CS)
		{
			CommandId = idCsSetSamplers11_0;
		}

		CsSetSamplers11_0(
			UINT			Offset, 
			UINT			NumSamplers, 
			CONST D3D10DDI_HSAMPLER* phSamplers
			)
			:	base_type( SP_CS, Offset, NumSamplers, phSamplers )
		{
			CommandId = idCsSetSamplers11_0;
		}

		virtual		void	Execute		( D3DDeviceWrapper *pWrapper );
		virtual		bool	WriteToFile	( D3DDeviceWrapper *pWrapper ) const;

		virtual TSetSamplersCmdPtr Clone() const override { return TSetSamplersCmdPtr(new CsSetSamplers11_0(Offset_, NumValues_, &Values_[0])); }
	};

}

extern VOID ( APIENTRY CsSetSamplers11_0 )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  Offset, 
									   UINT  NumSamplers, 
									   CONST D3D10DDI_HSAMPLER*  phSamplers
									   );
