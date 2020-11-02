#include "stdafx.h"
#include "DefaultConstantBufferUpdateSubresourceUP.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	void DefaultConstantBufferUpdateSubresourceUP::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);

		DEBUG_TRACE3(L"\t\t CB [%p]: pDstBox = %p, RowPitch = %d, DepthPitch = %d\n", hDstResource_.pDrvPrivate, pDstBox_, RowPitch_, DepthPitch_);

		CONST VOID*	pSysMemUP = pSysMemUP_;
		D3D10DDI_HRESOURCE hResource = hDstResource_;
		ResourceWrapper* pWrp;
		InitWrapper(hResource, pWrp);
		if (pWrp->m_pCB)
			pWrp->m_pCB->UpdateSubresourceUP(pWrapper, pSysMemUP);
		pWrapper->OriginalDeviceFuncs.pfnDefaultConstantBufferUpdateSubresourceUP( pWrapper->hDevice, 
			GET_RESOURCE_HANDLE(hDstResource_), DstSubresource_, pDstBox_, pSysMemUP, RowPitch_, DepthPitch_ );

		AFTER_EXECUTE(this); 
	}

	bool DefaultConstantBufferUpdateSubresourceUP::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DefaultConstantBufferUpdateSubresourceUP" );
		WriteStreamer::Reference( "hDstResource", hDstResource_ );
		WriteStreamer::Value( "DstSubresource", DstSubresource_ );
		WriteStreamer::ValueByRef( "DstBox", pDstBox_ );
		WriteStreamer::Value( "RowPitch", RowPitch_ );
		WriteStreamer::Value( "DepthPitch", DepthPitch_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DefaultConstantBufferUpdateSubresourceUP::ReadFromFile()
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

	bool DefaultConstantBufferUpdateSubresourceUP::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		return true;
		//ResourceWrapper* pRes;
		//InitWrapper(hDstResource_, pRes);
		//return pRes->m_pCB;
	}
}

VOID ( APIENTRY DefaultConstantBufferUpdateSubresourceUP )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hDstResource, 
														   UINT DstSubresource, CONST D3D10_DDI_BOX* pDstBox, CONST VOID* pSysMemUP, UINT RowPitch, UINT DepthPitch )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::DefaultConstantBufferUpdateSubresourceUP* command = new Commands::DefaultConstantBufferUpdateSubresourceUP( hDstResource, DstSubresource, pDstBox, pSysMemUP, RowPitch, DepthPitch );
	D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();

	D3D10DDI_HRESOURCE hResource = hDstResource;
	ResourceWrapper* pWrp;
	InitWrapper(hResource, pWrp);
	if (pWrp->m_pCB)
		pWrp->m_pCB->UpdateSubresourceUP(D3D10_GET_WRAPPER(), pSysMemUP );
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDefaultConstantBufferUpdateSubresourceUP( D3D10_DEVICE, 
			GET_RESOURCE_HANDLE(hDstResource), DstSubresource, pDstBox, pSysMemUP, RowPitch, DepthPitch );
	}
#endif
}
