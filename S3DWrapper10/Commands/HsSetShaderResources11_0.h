#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class HsSetShaderResources11_0 : public CommandWithAllocator<HsSetShaderResources11_0, xxSetShaderResources> 
	{
	public:
		HsSetShaderResources11_0()
		:	base_type(SP_HS)
		{
			CommandId = idHsSetShaderResources11_0;
		}

		HsSetShaderResources11_0( UINT									Offset, 
								  UINT									NumViews, 
								  CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews)
		:	base_type(SP_HS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idHsSetShaderResources11_0;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const override { return TSetShaderResourcesCmdPtr(new HsSetShaderResources11_0(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY HsSetShaderResources11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  Offset, 
	UINT  NumViews, 
	CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews
	);
