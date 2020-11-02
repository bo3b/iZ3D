#include "stdafx.h"
#include "ResourceUpdateSubresourceUP.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	void ResourceUpdateSubresourceUP::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceUpdateSubresourceUP( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource_),
			DstSubresource_,
			pDstBox_,
			pSysMemUP_,
			RowPitch_,
			DepthPitch_
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceUpdateSubresourceUP::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceUpdateSubresourceUP" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Value( "DstSubresource", DstSubresource_ );
		WriteStreamer::ValueByRef( "DstBox", pDstBox_ );
		WriteStreamer::Value( "RowPitch" , RowPitch_ );
		WriteStreamer::Value( "DepthPitch", DepthPitch_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceUpdateSubresourceUP::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hDstResource_);
		ReadStreamer::Value( DstSubresource_);
		ReadStreamer::ValueByRef( pDstBox_);
		ReadStreamer::Value( RowPitch_);
		ReadStreamer::Value( DepthPitch_);
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceUpdateSubresourceUP::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const 
	{
		return true;
	}

	void ResourceUpdateSubresourceUP::BuildCommand(CDumpState *ds)
	{
		ds->SetUsedAsRenderTarget(hDstResource_,FALSE);
		output("// unlock: buffer_%d",ds->GetHandleEventId(hDstResource_));
	}

}

VOID ( APIENTRY ResourceUpdateSubresourceUP )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hDstResource, 
	UINT  DstSubresource, 
	CONST D3D10_DDI_BOX*  pDstBox, 
	CONST VOID*  pSysMemUP, 
	UINT  RowPitch, 
	UINT  DepthPitch
	)
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::ResourceUpdateSubresourceUP* command = new Commands::ResourceUpdateSubresourceUP(
		hDstResource, 
		DstSubresource, 
		pDstBox, 
		pSysMemUP, 
		RowPitch, 
		DepthPitch
		);
	D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceUpdateSubresourceUP( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource), 
			DstSubresource, 
			pDstBox, 
			pSysMemUP, 
			RowPitch, 
			DepthPitch
			);
	}
#endif
}
