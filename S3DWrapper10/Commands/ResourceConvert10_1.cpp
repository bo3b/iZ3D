#include "stdafx.h"
#include "ResourceConvert10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceConvert10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs10_1.pfnResourceConvert( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE_EX(hDstResource_), 
			UNWRAP_CV_RESOURCE_HANDLE_EX(hSrcResource_)
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceConvert10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceIsStagingBusy" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Reference( "hSrcResource", hSrcResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}


	bool ResourceConvert10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDstResource_ );
		ReadStreamer::Reference( hSrcResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceConvert10_1::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
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


	void ResourceConvert10_1::SetDestResourceType( TextureType type )
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

	void ResourceConvert10_1::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
	}

}

VOID (APIENTRY  ResourceConvert10_1)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hDstResource, 
									 D3D10DDI_HRESOURCE  hSrcResource)
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ResourceConvert10_1* command = new Commands::ResourceConvert10_1( hDstResource, hSrcResource );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_1_GET_ORIG().pfnResourceConvert(D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hDstResource), UNWRAP_CV_RESOURCE_HANDLE(hSrcResource));
	}
#endif
}
