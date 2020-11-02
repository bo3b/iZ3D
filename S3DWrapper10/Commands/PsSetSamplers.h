#pragma once

#include "xxSetSamplers.h"

namespace Commands
{

	class PsSetSamplers : public CommandWithAllocator<PsSetSamplers, xxSetSamplers>
	{
	public:
		PsSetSamplers()
			:	base_type(SP_PS)
		{
			CommandId = idPsSetSamplers;
		}

		PsSetSamplers(UINT						Offset, 
					  UINT						NumSamplers, 
					  CONST D3D10DDI_HSAMPLER*	phSamplers)
		:	base_type(SP_PS, Offset, NumSamplers, phSamplers)
		{
			CommandId = idPsSetSamplers;
		}

		virtual		void	Execute( D3DDeviceWrapper *pWrapper );
		virtual		bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;

		// Override xxSetSamplers
		TSetSamplersCmdPtr Clone() const { return TSetSamplersCmdPtr(new PsSetSamplers(Offset_, NumValues_, &Values_[0])); }
	};

}

extern VOID ( APIENTRY PsSetSamplers )(
									   D3D10DDI_HDEVICE  hDevice, 
									   UINT  Offset, 
									   UINT  NumSamplers, 
									   CONST D3D10DDI_HSAMPLER*  phSamplers
									   );
