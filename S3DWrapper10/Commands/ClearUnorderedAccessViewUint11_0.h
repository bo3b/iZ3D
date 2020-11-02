#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class ClearUnorderedAccessViewUint11_0 : public CommandWithAllocator<ClearUnorderedAccessViewUint11_0>
	{
	public:
		ClearUnorderedAccessViewUint11_0()		
		{
			CommandId = idClearUnorderedAccessViewUint11_0;
		}

		ClearUnorderedAccessViewUint11_0(D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, CONST UINT  pUints[4])
		{    
			CommandId = idClearUnorderedAccessViewUint11_0;
			hUnorderedAccessView_ = hUnorderedAccessView;
			memcpy(pUints_, pUints, sizeof(UINT) * 4);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView_;
		UINT  pUints_[4];
	};

}

VOID ( APIENTRY ClearUnorderedAccessViewUint11_0 )( D3D10DDI_HDEVICE  hDevice, D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
												   CONST UINT  pUints[4] );
