#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class PsSetShaderResources : public CommandWithAllocator<PsSetShaderResources, xxSetShaderResources>
	{
	public:
		PsSetShaderResources()
		:	base_type(SP_PS)
		{
			CommandId = idPsSetShaderResources;
		}

		PsSetShaderResources(UINT									Offset, 
							 UINT									NumViews, 
							 CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews)
		:	base_type(SP_PS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idPsSetShaderResources;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const { return TSetShaderResourcesCmdPtr(new PsSetShaderResources(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
	};

}

extern VOID ( APIENTRY PsSetShaderResources )(
	D3D10DDI_HDEVICE  hDevice, 
	UINT  Offset, 
	UINT  NumViews, 
	CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews
	);
