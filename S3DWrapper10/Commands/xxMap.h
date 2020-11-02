#pragma once

#include "xxUpdateSubresourceUP.h"

namespace Commands
{
	class xxMap : public Command
	{
	public:
		xxMap();
		
		/** Create updated command which can be used instead map command. Function should store command into UpdateCommand_ field. */
		virtual TUpdateSubresourceUpCmdPtr CreateUpdateCommand( D3DDeviceWrapper*				/*pWrapper*/, 
																const xxUpdateSubresourceUP*	/*pOriginalCmd*/ ) 
		{ 
			return UpdateCommand_;
		}

		virtual void SetDestResourceType( TextureType type );
		virtual void GetDestResources(DestResourcesSet &res);
				
	public:
		D3D10DDI_HRESOURCE				hResource_;
		UINT							Subresource_;
		D3D10_DDI_MAP					DDIMap_;
		UINT							Flags_;
		D3D10DDI_MAPPED_SUBRESOURCE*	pMappedSubResource_;
		TUpdateSubresourceUpCmdPtr		UpdateCommand_;
	};

	typedef boost::intrusive_ptr<xxMap> TMapCmdPtr;
}
