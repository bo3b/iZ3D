#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class VsSetShaderResources : public CommandWithAllocator<VsSetShaderResources, xxSetShaderResources> 
	{
	public:
		VsSetShaderResources()
		:	base_type(SP_VS)
		{
			CommandId = idVsSetShaderResources;
		}

		VsSetShaderResources( UINT									Offset, 
							  UINT									NumViews, 
							  CONST D3D10DDI_HSHADERRESOURCEVIEW*   phShaderResourceViews )
		:	base_type(SP_VS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idVsSetShaderResources;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const { return TSetShaderResourcesCmdPtr(new VsSetShaderResources(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute(D3DDeviceWrapper *pWrapper);
		virtual bool WriteToFile(D3DDeviceWrapper *pWrapper) const;
	};

}

VOID ( APIENTRY VsSetShaderResources )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumViews, 
									   CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews );
