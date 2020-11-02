#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class CreateRasterizerState : public CommandWithAllocator<CreateRasterizerState>
	{
	public:
		CreateRasterizerState()		
		{
			CommandId = idCreateRasterizerState;
		}

		CreateRasterizerState(const D3D10_DDI_RASTERIZER_DESC*  pRasterizerDesc, 
			D3D10DDI_HRASTERIZERSTATE  hRasterizerState, 
			D3D10DDI_HRTRASTERIZERSTATE  hRTRasterizerState)
		{
			CommandId = idCreateRasterizerState;
			memcpy(&RasterizerDesc_, pRasterizerDesc, sizeof(D3D10_DDI_RASTERIZER_DESC));

			hRasterizerState_ = hRasterizerState;
			hRTRasterizerState_ = hRTRasterizerState;   
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	WriteToFileSimple( D3DDeviceWrapper *pWrapper ) const
		{	return WriteToFile(pWrapper);	}
		virtual bool	ReadFromFile	();

		void BuildCommand(CDumpState *ds);

	public:

		D3D10_DDI_RASTERIZER_DESC	  RasterizerDesc_; 
		D3D10DDI_HRASTERIZERSTATE     hRasterizerState_; 
		D3D10DDI_HRTRASTERIZERSTATE   hRTRasterizerState_;

		CREATES(hRasterizerState_);
	};

}

VOID ( APIENTRY CreateRasterizerState )( D3D10DDI_HDEVICE  hDevice, 
										CONST D3D10_DDI_RASTERIZER_DESC*  pRasterizerDesc, 
										D3D10DDI_HRASTERIZERSTATE  hRasterizerState, 
										D3D10DDI_HRTRASTERIZERSTATE  hRTRasterizerState );
