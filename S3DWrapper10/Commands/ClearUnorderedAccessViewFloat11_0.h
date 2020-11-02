#pragma once

#include "Command.h"
#include <memory.h>

namespace Commands
{

	class ClearUnorderedAccessViewFloat11_0 : public CommandWithAllocator<ClearUnorderedAccessViewFloat11_0>
	{
	public:
		ClearUnorderedAccessViewFloat11_0()		
		{
			CommandId = idClearUnorderedAccessViewFloat11_0;
		}

		ClearUnorderedAccessViewFloat11_0(D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, const float  pFloats[4])
		{    
			CommandId = idClearUnorderedAccessViewFloat11_0;
			hUnorderedAccessView_ = hUnorderedAccessView;
			memcpy(pFloats_, pFloats, sizeof(float) * 4);
		}

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();

		virtual bool	IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;
		virtual void	SetDestResourceType( TextureType type );
		virtual void	GetDestResources(DestResourcesSet &res);

	public:

		D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView_;
		float  pFloats_[4];
	};

}

VOID ( APIENTRY ClearUnorderedAccessViewFloat11_0 )( D3D10DDI_HDEVICE  hDevice, D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
													CONST float  pFloats[4] );
