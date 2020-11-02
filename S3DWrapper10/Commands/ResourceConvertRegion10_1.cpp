#include "stdafx.h"
#include "ResourceConvertRegion10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceConvertRegion10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs10_1.pfnResourceConvertRegion( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource_),
			DstSubresource_,
			DstX_,
			DstY_,
			DstZ_,
			UNWRAP_CV_RESOURCE_HANDLE(hSrcResource_),
			SrcSubresource_, 
			pSrcBox_
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceConvertRegion10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceIsStagingBusy" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Value( "DstSubresource", DstSubresource_ );
		WriteStreamer::Value( "DstX", DstX_ );
		WriteStreamer::Value( "DstY", DstY_ );
		WriteStreamer::Value( "DstZ", DstZ_ );
		WriteStreamer::Reference( "hSrcResource", hSrcResource_ );
		WriteStreamer::Value( "SrcSubresource", SrcSubresource_ );
		WriteStreamer::ValueByRef( "SrcBox", pSrcBox_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceConvertRegion10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hDstResource_);
		ReadStreamer::Value( DstSubresource_);
		ReadStreamer::Value( DstX_);
		ReadStreamer::Value( DstY_);
		ReadStreamer::Value( DstZ_);
		ReadStreamer::Reference( hSrcResource_);
		ReadStreamer::Value( SrcSubresource_);
		ReadStreamer::ValueByRef( pSrcBox_);
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceConvertRegion10_1::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
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

	void ResourceConvertRegion10_1::SetDestResourceType( TextureType type )
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

	void ResourceConvertRegion10_1::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
}

}

VOID (APIENTRY  ResourceConvertRegion10_1)(D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRESOURCE  hDstResource, 
										   UINT  DstSubresource, UINT  DstX, UINT  DstY, UINT  DstZ, 
										   D3D10DDI_HRESOURCE  hSrcResource, UINT  SrcSubresource, 
										   CONST D3D10_DDI_BOX*  pSrcBox )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_10_1);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ResourceConvertRegion10_1* command = new Commands::ResourceConvertRegion10_1(hDstResource, DstSubresource, 
		DstX, DstY, DstZ, hSrcResource, SrcSubresource, pSrcBox);
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_1_GET_ORIG().pfnResourceConvertRegion(D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hDstResource), DstSubresource, 
			DstX, DstY, DstZ, UNWRAP_CV_RESOURCE_HANDLE(hSrcResource), SrcSubresource, pSrcBox);
	}
#endif
}