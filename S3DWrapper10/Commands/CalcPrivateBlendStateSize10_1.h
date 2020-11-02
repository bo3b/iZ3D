#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateBlendStateSize10_1 : public CommandWithAllocator<CalcPrivateBlendStateSize10_1>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const {	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateBlendStateSize10_1 ()			
		{
			CommandId		= idCalcPrivateBlendStateSize10_1;
		}

		CalcPrivateBlendStateSize10_1( CONST D3D10_1_DDI_BLEND_DESC*  pBlendDesc)
		{
			CommandId		= idCalcPrivateBlendStateSize10_1;
			memcpy(&BlendDesc_, pBlendDesc, sizeof(D3D10_1_DDI_BLEND_DESC));
		}

	public:

		D3D10_1_DDI_BLEND_DESC  BlendDesc_;
		SIZE_T					RetValue_;
	};

}

extern SIZE_T ( APIENTRY CalcPrivateBlendStateSize10_1 )( D3D10DDI_HDEVICE  hDevice, CONST D3D10_1_DDI_BLEND_DESC*  pBlendDesc );
