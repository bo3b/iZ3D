#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CalcPrivateRenderTargetViewSize : public CommandWithAllocator<CalcPrivateRenderTargetViewSize>
	{
	public:
		EMPTY_OUTPUT();

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		CalcPrivateRenderTargetViewSize ()
		{
			CommandId = idCalcPrivateRenderTargetViewSize;
		}

		CalcPrivateRenderTargetViewSize(const D3D10DDIARG_CREATERENDERTARGETVIEW* pCreateRenderTargetView)
		{
			CommandId = idCalcPrivateRenderTargetViewSize;
			memcpy(&CreateRenderTargetView_, pCreateRenderTargetView, sizeof(D3D10DDIARG_CREATERENDERTARGETVIEW));
		}

	public:

		D3D10DDIARG_CREATERENDERTARGETVIEW  CreateRenderTargetView_;
#ifndef FINAL_RELEASE
		SIZE_T	RetValue_;
#endif		

	};

}

SIZE_T ( APIENTRY CalcPrivateRenderTargetViewSize )( D3D10DDI_HDEVICE  hDevice, 
													CONST D3D10DDIARG_CREATERENDERTARGETVIEW*  pCreateRenderTargetView );
