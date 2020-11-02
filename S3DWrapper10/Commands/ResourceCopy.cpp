#include "stdafx.h"
#include "ResourceCopy.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceCopy::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceCopy( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource_),
			UNWRAP_CV_RESOURCE_HANDLE_EX(hSrcResource_)
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceCopy::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceCopy" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Reference( "hSrcResource", hSrcResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceCopy::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDstResource_ );
		ReadStreamer::Reference( hSrcResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceCopy::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
	{
		ResourceWrapper* pDstRes;
		InitWrapper(hDstResource_, pDstRes);
		if (pDstRes->IsStereo())
			return true;
		ResourceWrapper* pSrcRes;
		InitWrapper(hSrcResource_, pSrcRes);
		if (pSrcRes->IsStereo())
		{
			DEBUG_TRACE1(_T("Resource converted to stereo\n"));
			pDstRes->CreateRightResource(pWrapper);
			return true;
		}
		return false;
	}

	void ResourceCopy::SetDestResourceType( TextureType type )
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

	void ResourceCopy::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
	}

	void ResourceCopy::BuildCommand(CDumpState *ds)
	{
		int eid1 = ds->GetHandleEventId(hDstResource_);
		int eid2 = ds->GetHandleEventId(hSrcResource_);

		if (ds->IsUsedAsRenderTarget(hSrcResource_))
		{
			output("ctx->CopyResource(buffer_%d,buffer_%d);",eid1,eid2);

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

VOID ( APIENTRY ResourceCopy )(
							   D3D10DDI_HDEVICE  hDevice, 
							   D3D10DDI_HRESOURCE  hDstResource, 
							   D3D10DDI_HRESOURCE  hSrcResource
							   )
{
#ifndef EXECUTE_IMMEDIATELY_G1	
	ResourceWrapper* pWrpDst = NULL;
	ResourceWrapper* pWrpSrc = NULL;
	InitWrapper( hDstResource, pWrpDst );
	InitWrapper( hSrcResource, pWrpSrc );
	if ( pWrpSrc->IsAQBSSurface() || pWrpDst->IsAQBSSurface() )
	{
		// we need call this command immediate, in other case aqbs isn't created
		D3D10_GET_ORIG().pfnResourceCopy( D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hDstResource), UNWRAP_CV_RESOURCE_HANDLE(hSrcResource) );
	}
	else
	{
		Commands::ResourceCopy* command = new Commands::ResourceCopy( hDstResource, hSrcResource );
		D3D10_GET_WRAPPER()->AddCommand( command ); 
	}
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceCopy( D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hDstResource), UNWRAP_CV_RESOURCE_HANDLE_EX(hSrcResource) );
	}
#endif
}