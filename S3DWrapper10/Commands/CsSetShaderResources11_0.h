#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class CsSetShaderResources11_0 : public CommandWithAllocator<CsSetShaderResources11_0, xxSetShaderResources>
	{
	public:
		CsSetShaderResources11_0()
		:	base_type(SP_CS)
		{
			CommandId = idCsSetShaderResources11_0;
		}

		CsSetShaderResources11_0( UINT									Offset, 
								  UINT									NumViews, 
								  CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews )
		:	base_type(SP_CS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idCsSetShaderResources11_0;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const override { return TSetShaderResourcesCmdPtr(new CsSetShaderResources11_0(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
	};

}

extern VOID ( APIENTRY CsSetShaderResources11_0 )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  Offset, 
	UINT  NumViews, 
	CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews
	);
