#pragma once

#include "Command.h"

namespace Commands
{

	class DestroyRasterizerState : public CommandWithAllocator<DestroyRasterizerState>
	{
	public:
		EMPTY_OUTPUT();

		DestroyRasterizerState()		
		{
			CommandId = idDestroyRasterizerState;
		}

		DestroyRasterizerState( D3D10DDI_HRASTERIZERSTATE hRasterizerState )
		{
			CommandId = idDestroyRasterizerState;
			hRasterizerState_ = hRasterizerState;
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		D3D10DDI_HRASTERIZERSTATE hRasterizerState_;
	};

}

extern VOID ( APIENTRY DestroyRasterizerState )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRASTERIZERSTATE hRasterizerState );
