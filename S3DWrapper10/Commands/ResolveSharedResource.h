#pragma once

#include "Command.h"

namespace Commands
{

	class ResolveSharedResource : public CommandWithAllocator<ResolveSharedResource>
	{
	public:
		virtual void	Execute( D3DDeviceWrapper *pWrapper );
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const;
		virtual bool	ReadFromFile();		

		ResolveSharedResource ()		
			:	pResolveData_ ( NULL )
		{
			CommandId = idResolveSharedResource;
		}

		ResolveSharedResource( DXGI_DDI_ARG_RESOLVESHAREDRESOURCE*  pResolveData )
		{
			CommandId = idResolveSharedResource;
			pResolveData_ = pResolveData;
			//MEMCOPY(&pResolveData_, pResolveData, sizeof(DXGI_DDI_ARG_RESOLVESHAREDRESOURCE));
		}

		virtual bool IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const;

	public:
		DXGI_DDI_ARG_RESOLVESHAREDRESOURCE*		pResolveData_;
		//DXGI_DDI_ARG_RESOLVESHAREDRESOURCE	ResolveData_;
		HRESULT									RetValue_;
	};

}

extern HRESULT ( APIENTRY  ResolveSharedResource )  (DXGI_DDI_ARG_RESOLVESHAREDRESOURCE* pResolveData);
