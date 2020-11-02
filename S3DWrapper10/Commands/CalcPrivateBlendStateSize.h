#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateBlendStateSize : public CommandWithAllocator<CalcPrivateBlendStateSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void Execute		( D3DDeviceWrapper *pWrapper );
		virtual	bool WriteToFile	( D3DDeviceWrapper *pWrapper ) const;
		virtual bool WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const {	return WriteToFile(pWrapper);	}
		virtual bool ReadFromFile	();


		CalcPrivateBlendStateSize ()		
		{
			CommandId		= idCalcPrivateBlendStateSize;
		}

		CalcPrivateBlendStateSize( CONST D3D10_DDI_BLEND_DESC*  pBlendDesc )
		{
			CommandId		= idCalcPrivateBlendStateSize;
			memcpy(&BlendDesc_, pBlendDesc, sizeof(D3D10_DDI_BLEND_DESC));
		}

	public:

		D3D10_DDI_BLEND_DESC	BlendDesc_;
		SIZE_T					RetValue_;
	};

}

SIZE_T ( APIENTRY CalcPrivateBlendStateSize )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_DDI_BLEND_DESC*  pBlendDesc );

