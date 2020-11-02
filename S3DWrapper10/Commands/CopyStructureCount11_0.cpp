#include "stdafx.h"
#include "CopyStructureCount11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{

	void CopyStructureCount11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCopyStructureCount( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstBuffer_), 
			DstAlignedByteOffset_, 
			UNWRAP_CV_UAV_HANDLE(hSrcView_)
			);
		AFTER_EXECUTE(this); 
	}

	bool CopyStructureCount11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CopyStructureCount11_0" );
		WriteStreamer::Reference( "hDstBuffer", hDstBuffer_ );;
		WriteStreamer::Value( "DstAlignedByteOffset", DstAlignedByteOffset_ );
		WriteStreamer::Reference( "hSrcView", hSrcView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CopyStructureCount11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDstBuffer_ );
		ReadStreamer::Value( DstAlignedByteOffset_ );
		ReadStreamer::Reference( hSrcView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool CopyStructureCount11_0::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
	{
		ResourceWrapper* pDstRes;
		InitWrapper(hDstBuffer_, pDstRes);
		if (pDstRes->IsStereo())
			return true;
		if (IS_STEREO_UAV_HANDLE(hSrcView_))
		{
			DEBUG_TRACE1(_T("Resource converted to stereo\n"));
			pDstRes->CreateRightResource(pWrapper);
			return true;
		}
		return false;
	}

	void CopyStructureCount11_0::SetDestResourceType( TextureType type )
	{
		if (type == TT_SETLIKESRC)
		{
			UnorderedAccessViewWrapper* pSrcView;
			InitWrapper(hSrcView_, pSrcView);
			type = pSrcView->GetResourceWrapper()->m_Type;
		}
		ResourceWrapper* pRes;
		InitWrapper(hDstBuffer_, pRes);
		pRes->m_Type = type;
	}

	void CopyStructureCount11_0::GetDestResources( DestResourcesSet &res )
	{
		res.insert(hDstBuffer_);
	}
}

VOID ( APIENTRY CopyStructureCount11_0 )(
								D3D10DDI_HDEVICE  hDevice, 
								D3D10DDI_HRESOURCE  hDstBuffer, 
								UINT  DstAlignedByteOffset,
								D3D11DDI_HUNORDEREDACCESSVIEW  hSrcView 
							   )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::CopyStructureCount11_0* command = new Commands::CopyStructureCount11_0( hDstBuffer, DstAlignedByteOffset, hSrcView );
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCopyStructureCount( D3D10_DEVICE, UNWRAP_CV_RESOURCE_HANDLE(hDstBuffer), DstAlignedByteOffset, 
			UNWRAP_CV_UAV_HANDLE(hSrcView) );
	}
#endif
}