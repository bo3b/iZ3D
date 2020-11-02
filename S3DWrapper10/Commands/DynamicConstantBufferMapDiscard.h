#pragma once

#include "xxMap.h"

namespace Commands
{
	class DynamicConstantBufferMapDiscard : public CommandWithAllocator<DynamicConstantBufferMapDiscard, xxMap>
	{
	public:
		EMPTY_OUTPUT();
		DynamicConstantBufferMapDiscard();

		DynamicConstantBufferMapDiscard( D3D10DDI_HRESOURCE hResource, 
			UINT Subresource, 
			D3D10_DDI_MAP DDIMap, 
			UINT  Flags, 
			D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource );

		virtual TUpdateSubresourceUpCmdPtr CreateUpdateCommand( D3DDeviceWrapper *pWrapper, const xxUpdateSubresourceUP* pOriginalCmd );

		virtual void	Execute			( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile		( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile	();
	};
}

extern VOID ( APIENTRY DynamicConstantBufferMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource, 
														 D3D10_DDI_MAP DDIMap, UINT  Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource );
