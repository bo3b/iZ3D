#pragma once

#include "xxSetShaderResources.h"

namespace Commands
{

	class GsSetShaderResources : public CommandWithAllocator<GsSetShaderResources, xxSetShaderResources>
	{
	public:
		GsSetShaderResources()
		:	base_type(SP_GS)
		{
			CommandId = idGsSetShaderResources;
		}

		GsSetShaderResources( UINT									Offset, 
							  UINT									NumViews, 
							  CONST D3D10DDI_HSHADERRESOURCEVIEW*	phShaderResourceViews )
		:	base_type(SP_GS, Offset, NumViews, phShaderResourceViews)
		{
			CommandId = idGsSetShaderResources;
		}
		
		// Override xxSetShaderResources
		virtual TSetShaderResourcesCmdPtr Clone() const { return TSetShaderResourcesCmdPtr(new GsSetShaderResources(Offset_, NumValues_, &Values_[0])); }

		// Override Command
		virtual void Execute( D3DDeviceWrapper *pWrapper );
		virtual bool WriteToFile( D3DDeviceWrapper *pWrapper ) const;
	};

}

extern VOID ( APIENTRY GsSetShaderResources )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumViews, 
											  CONST D3D10DDI_HSHADERRESOURCEVIEW*  phShaderResourceViews );
