#include "stdafx.h"
#include "ResourceCopyRegion.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"
#include "Presenter.h"

namespace Commands
{

	void ResourceCopyRegion::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		D3DSwapChain*		pSwapChain	= pWrapper->GetD3DSwapChain();

		pWrapper->OriginalDeviceFuncs.pfnResourceCopyRegion( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource_),
			DstSubresource_, 
			DstX_, 
			DstY_, 
			DstZ_, 
			UNWRAP_CV_RESOURCE_HANDLE_EX(hSrcResource_),
			SrcSubresource_, 
			pSrcBox_		
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceCopyRegion::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceCopyRegion" );
		WriteStreamer::Value( "hDstResource", hDstResource_.pDrvPrivate ); 
		WriteStreamer::Value( "DstSubresource", DstSubresource_ ); 
		WriteStreamer::Value( "DstX", DstX_ ); 
		WriteStreamer::Value( "DstY", DstY_ ); 
		WriteStreamer::Value( "DstZ", DstZ_ ); 
		WriteStreamer::Value( "hSrcResource", hSrcResource_.pDrvPrivate ); 
		WriteStreamer::Value( "SrcSubresource", SrcSubresource_ );
		WriteStreamer::ValueByRef( "SrcBox", pSrcBox_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceCopyRegion::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( hDstResource_.pDrvPrivate); 
		ReadStreamer::Value( DstSubresource_); 
		ReadStreamer::Value( DstX_); 
		ReadStreamer::Value( DstY_); 
		ReadStreamer::Value( DstZ_); 
		ReadStreamer::Value( hSrcResource_.pDrvPrivate); 
		ReadStreamer::Value( SrcSubresource_);
		ReadStreamer::ValueByRef( pSrcBox_);
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceCopyRegion::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
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

	void ResourceCopyRegion::SetDestResourceType( TextureType type )
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

	void ResourceCopyRegion::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
	}

	void ResourceCopyRegion::BuildCommand(CDumpState *ds)
	{
		int eid1 = ds->GetHandleEventId(hDstResource_);
		int eid2 = ds->GetHandleEventId(hSrcResource_);

		if (ds->IsUsedAsRenderTarget(hSrcResource_))
		{
			if (pSrcBox_)
			{
				output("box.back = %u;",pSrcBox_->back);
				output("box.bottom = %u;",pSrcBox_->bottom);
				output("box.front = %u;",pSrcBox_->front);
				output("box.left = %u;",pSrcBox_->left);
				output("box.right = %u;",pSrcBox_->right);
				output("box.top = %u;",pSrcBox_->top);
			}

			output("buffer_%d->AddRef(); // PIX \"feature\" support",eid1);
			output("buffer_%d->AddRef();",eid2);
			
			if (pSrcBox_)
			{
				output("ctx->CopySubresourceRegion(buffer_%d,%u,%u,%u,%u,buffer_%d,%u,&box);",
					eid1,DstSubresource_,DstX_,DstY_,DstZ_,eid2,SrcSubresource_);
			}
			else
			{
				output("ctx->CopySubresourceRegion(buffer_%d,%u,%u,%u,%u,buffer_%d,%u,0);",
					eid1,DstSubresource_,DstX_,DstY_,DstZ_,eid2,SrcSubresource_);
			}

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

VOID ( APIENTRY ResourceCopyRegion )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HRESOURCE  hDstResource, 
									 UINT  DstSubresource, 
									 UINT  DstX, 
									 UINT  DstY, 
									 UINT  DstZ, 
									 D3D10DDI_HRESOURCE  hSrcResource, 
									 UINT  SrcSubresource, 
									 CONST D3D10_DDI_BOX*  pSrcBox
									 )
{
	D3DSwapChain*		pSwapChain	= D3D10_GET_WRAPPER()->GetD3DSwapChain();

	// DEBUG ONLY!
	DEBUG_TRACE3(_T("ResourceCopyRegion(): hDstResource     = 0x%p\n" ), GET_RESOURCE_HANDLE(hDstResource) );
	DEBUG_TRACE3(_T("ResourceCopyRegion(): hSrcResource     = 0x%p\n" ), GET_RESOURCE_HANDLE(hSrcResource) );

// 	if( pSwapChain && pSwapChain->m_bUseKMShutterService )			// TODO: check this piece of code
// 	{
// 		bool isLeft = pSwapChain->m_bLeftEyeActive;
// 		DEBUG_TRACE3(_T("KM Shutter Presenter - ResourceCopyRegion\n"));
// 		SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
// 		if (pRes == NULL)
// 		{
// 			DEBUG_MESSAGE(_T("WARNING: pSwapChain->GetCurrentPrimaryResource() == NULL, probably presenter bug\n"));
// 			return;
// 		}
// 		D3D10DDI_HRESOURCE hPresenterResource = pRes->GetPresenterResource(isLeft);
// 		if (hPresenterResource.pDrvPrivate == NULL)
// 		{
// 			DEBUG_MESSAGE(_T("WARNING: m_hPresenterResourceLeft == NULL, probably presenter bug\n"));
// 			return;
// 		}
// 
// 		THREAD_GUARD_D3D10();
// 		D3D10DDI_HRESOURCE hDstRes = GET_RESOURCE_HANDLE(hDstResource);
// 		D3D10_GET_ORIG().pfnResourceCopyRegion( D3D10_DEVICE,
// 			hDstRes,
// 			DstSubresource,
// 			DstX, DstY, DstZ,
// 			hPresenterResource,
// 			0,
// 			pSrcBox
// 		);
// 
// 		return;
// 	}

	if( CPresenterX::Get() && !D3D10_GET_WRAPPER()->SkipPresenterThread() && pSwapChain )
	{
		if (pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId())
		{
			DEBUG_TRACE3(_T("Presenter - ResourceCopyRegion\n"));
			pSwapChain->m_bResourceCopyRegionCalled = true;
			//DEBUG_MESSAGE(_T("Presenter - ResourceCopyRegion Src %p Dst %p\n"), 
			//	hSrcResource.pDrvPrivate, hDstResource.pDrvPrivate);
			UINT frame = CPresenterX::Get()->GetFrameCount();
			bool isLeft = !( frame & 0x0001 );
			SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
			D3D10DDI_HRESOURCE hPresenterResource = pRes->GetPresenterResource(isLeft);
			if (hPresenterResource.pDrvPrivate == NULL)
			{
				DEBUG_MESSAGE(_T("WARNING: m_hPresenterResourceLeft == NULL, probably presenter bug\n"));
				return;
			}

			THREAD_GUARD_D3D10();
			D3D10DDI_HRESOURCE hDstRes = GET_RESOURCE_HANDLE(hDstResource);
			D3D10_GET_ORIG().pfnResourceCopyRegion( D3D10_DEVICE,
				hDstRes,
				DstSubresource,
				DstX, DstY, DstZ,
				hPresenterResource,
				0,
				pSrcBox
				);
			//DEBUG_MESSAGE(_T("Presenter2 - ResourceCopyRegion Src %p Dst %p\n"), 
			//	hPresenterResource.pDrvPrivate, hDstResource.pDrvPrivate);

#ifndef FINAL_RELEASE
			if( D3D10_GET_WRAPPER()->m_DumpType >= dtOnlyRT )
			{
				ResourceWrapper* pSrcWrapper;
				InitWrapper(pRes->GetPresenterResourceWrap(), pSrcWrapper);
				ResourceWrapper* pWrapper;
				InitWrapper(hDstResource, pWrapper);

				TCHAR szFileName[MAX_PATH];
				_stprintf_s(szFileName, L"%s\\Frame.%.4d.%s.ResourceCopyRegion.Src.RT", 
					D3D10_GET_WRAPPER()->m_DumpDirectory, frame, isLeft ? _T("L") : _T("R")); 
				pSrcWrapper->DumpToFile(D3D10_GET_WRAPPER(), szFileName, NULL, true, true, isLeft ? dvLeft : dvRight);

				_stprintf_s(szFileName, L"%s\\Frame.%.4d.%s.ResourceCopyRegion.Dst.RT", 
					D3D10_GET_WRAPPER()->m_DumpDirectory, frame, isLeft ? _T("L") : _T("R")); 
				pWrapper->DumpToFile(D3D10_GET_WRAPPER(), szFileName, NULL, true, true, isLeft ? dvLeft : dvRight);
			}
#endif
			return;
		}
		else 
		{
			DEBUG_TRACE3(_T("App - ResourceCopyRegion\n"));
			//DEBUG_MESSAGE(_T("App - ResourceCopyRegion Src %p Dst %p\n"), 
			//	hSrcResource.pDrvPrivate, hDstResource.pDrvPrivate);
			//DEBUG_MESSAGE(_T("App2 - ResourceCopyRegion Src %p Dst %p\n"), 
			//	hSrcResource.pDrvPrivate, hDstResource.pDrvPrivate);
		}
	}

#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ResourceCopyRegion* command = new Commands::ResourceCopyRegion(
		hDstResource,
		DstSubresource,
		DstX, DstY, DstZ,
		hSrcResource,
		SrcSubresource,
		pSrcBox
		);
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceCopyRegion( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hDstResource),
			DstSubresource,
			DstX, DstY, DstZ,
			UNWRAP_CV_RESOURCE_HANDLE_EX(hSrcResource),
			SrcSubresource,
			pSrcBox
			);
	}
#endif
}