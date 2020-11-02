#include "stdafx.h"
#include "ResourceResolveSubresource.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceResolveSubresource::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceResolveSubresource( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource_),
			DstSubresource_, 
			UNWRAP_CV_RESOURCE_HANDLE(hSrcResource_), 
			SrcSubresource_,
			ResolveFormat_		
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceResolveSubresource::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceResolveSubresource" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Value( "DstSubresource", DstSubresource_ );
		WriteStreamer::Reference( "hSrcResource", hSrcResource_ );
		WriteStreamer::Value( "SrcSubresource", SrcSubresource_ );
		WriteStreamer::Value( "ResolveFormat", ResolveFormat_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceResolveSubresource::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDstResource_ );
		ReadStreamer::Value( DstSubresource_ );
		ReadStreamer::Reference( hSrcResource_ );
		ReadStreamer::Value( SrcSubresource_ );
		ReadStreamer::Value( ResolveFormat_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceResolveSubresource::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
	{
		ResourceWrapper* pDstRes;
		InitWrapper(hDstResource_, pDstRes);
		if (pDstRes->IsStereo())
			return true;
		if (IS_STEREO_RESOURCE_HANDLE(hSrcResource_))
		{
			DEBUG_TRACE1(_T("Resource converted to stereo\n"));
			pDstRes->CreateRightResource(pWrapper);
			return true;
		}
		return false;
	}

	void ResourceResolveSubresource::SetDestResourceType( TextureType type )
	{
		if (type == TT_SETLIKESRC)
		{
			ResourceWrapper* pSrcRes;
			InitWrapper(hSrcResource_, pSrcRes);
			type = pSrcRes->m_Type;
		}
		ResourceWrapper* pRes;
		InitWrapper(hDstResource_, pRes);
		pRes->m_Type = type;
	}

	void ResourceResolveSubresource::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
	}

	void ResourceResolveSubresource::BuildCommand(CDumpState *ds)
	{
		int eid1 = ds->GetHandleEventId(hDstResource_);
		int eid2 = ds->GetHandleEventId(hSrcResource_);

		// src locked?   dst locked?    copy?
		//     0            0			  0
		//     0            1			  0, unlock dst
		//     1            0             1, lock dst
		//     1			1             1
		if (ds->IsUsedAsRenderTarget(hSrcResource_))
		{
			output("buffer_%d->AddRef(); // PIX \"feature\" support",eid1);
			output("buffer_%d->AddRef();",eid2);
			output("ctx->ResolveSubresource(buffer_%d,%u,buffer_%d,%u,%s);",
				eid1,DstSubresource_,eid2,SrcSubresource_,EnumToString(ResolveFormat_));
			output("buffer_%d->Release();",eid1);
			output("buffer_%d->Release();",eid2);

			ds->SetUsedAsRenderTarget(hDstResource_);
			output("// lock: buffer_%d",eid1);
		}
		else 
		{
			ds->SetUsedAsRenderTarget(hDstResource_,FALSE);
			output("// unlock: buffer_%d",eid1);
		}
	}

}

VOID ( APIENTRY ResourceResolveSubresource )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hDstResource, 
	UINT  DstSubresource, 
	D3D10DDI_HRESOURCE  hSrcResource, 
	UINT  SrcSubresource, 
	DXGI_FORMAT  ResolveFormat
	)
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ResourceResolveSubresource* command = new Commands::ResourceResolveSubresource(
		hDstResource, 
		DstSubresource, 
		hSrcResource, 
		SrcSubresource, 
		ResolveFormat
		);
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceResolveSubresource( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource), 
			DstSubresource, 
			UNWRAP_CV_RESOURCE_HANDLE(hSrcResource), 
			SrcSubresource, 
			ResolveFormat
			);
	}
#endif
}