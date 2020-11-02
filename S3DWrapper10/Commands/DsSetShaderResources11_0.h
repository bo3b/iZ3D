#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class DsSetShaderResources11_0 : public CommandWithAllocator<DsSetShaderResources11_0, xxSetShaderResources>
	{
	public:
		DsSetShaderResources11_0()
		:	base_type(SP_DS)
		{
			CommandId = idDsSetShaderResources11_0;
		}

		DsSetShaderResources11_0( UINT									Offset, 
								  UINT									NumViews, 
								  CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews)
		:	base_type(SP_DS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idDsSetShaderResources11_0;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const override { return TSetShaderResourcesCmdPtr(new DsSetShaderResources11_0(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY DsSetShaderResources11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  Offset, 
	UINT  NumViews, 
	CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews
	);
