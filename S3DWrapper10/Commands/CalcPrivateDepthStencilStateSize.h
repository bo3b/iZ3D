#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateDepthStencilStateSize : public CommandWithAllocator<CalcPrivateDepthStencilStateSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const {	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateDepthStencilStateSize ()
		{
			CommandId = idCalcPrivateDepthStencilStateSize;
		}

		CalcPrivateDepthStencilStateSize( CONST D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc)
		{
			CommandId = idCalcPrivateDepthStencilStateSize;
			memcpy(&DepthStencilDesc_, pDepthStencilDesc, sizeof(D3D10_DDI_DEPTH_STENCIL_DESC));
		}

	public:

		D3D10_DDI_DEPTH_STENCIL_DESC	DepthStencilDesc_;
		SIZE_T							RetValue_;

	};

}

SIZE_T ( APIENTRY CalcPrivateDepthStencilStateSize )( D3D10DDI_HDEVICE  hDevice, 
													 CONST D3D10_DDI_DEPTH_STENCIL_DESC*  pDepthStencilDesc );
