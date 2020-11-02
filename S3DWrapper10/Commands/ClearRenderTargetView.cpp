#include "stdafx.h"
#include "ClearRenderTargetView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void ClearRenderTargetView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnClearRenderTargetView(pWrapper->hDevice, UNWRAP_CV_RTV_HANDLE(hRenderTargetView_), pColorRGBA_);
		AFTER_EXECUTE(this); 
	}

	bool ClearRenderTargetView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ClearRenderTargetView" );
		WriteStreamer::Reference( "hRenderTargetView", hRenderTargetView_ );
		WriteStreamer::ArrayValues( "ColorRGBA", 4, pColorRGBA_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool  ClearRenderTargetView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hRenderTargetView_ );
		ReadStreamer::ArrayValues( pColorRGBA_, 4 );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ClearRenderTargetView::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_RTV_HANDLE(hRenderTargetView_))
			return true;
		return false;
	}

	void ClearRenderTargetView::SetDestResourceType( TextureType type )
	{
		if (hRenderTargetView_.pDrvPrivate)
		{
			RenderTargetViewWrapper* pRVVWrapper = (RenderTargetViewWrapper*)hRenderTargetView_.pDrvPrivate;
			if (pRVVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pRVVWrapper->GetResourceWrapper();
				pRes->m_Type = type;
			}
		} 
	}

	void ClearRenderTargetView::GetDestResources(DestResourcesSet &res)
	{
		if (hRenderTargetView_.pDrvPrivate)
		{
			RenderTargetViewWrapper* pRVVWrapper = (RenderTargetViewWrapper*)hRenderTargetView_.pDrvPrivate;
			if (pRVVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pRVVWrapper->GetResourceWrapper();
				res.insert(*pRes);
			}
		} 
	}

	void ClearRenderTargetView::BuildCommand(CDumpState *ds)
	{
		output("const float color_%d[4] = { %ff,%ff,%ff,%ff };",
			EventID_,pColorRGBA_[0],pColorRGBA_[1],pColorRGBA_[2],pColorRGBA_[3]);

		output("ctx->ClearRenderTargetView(rtview_%d,color_%d);",
			ds->GetHandleEventId(hRenderTargetView_),EventID_);
	}
}

VOID ( APIENTRY ClearRenderTargetView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, 
										FLOAT  pColorRGBA[4] )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ClearRenderTargetView* command = new Commands::ClearRenderTargetView(hRenderTargetView, pColorRGBA);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnClearRenderTargetView(D3D10_DEVICE, UNWRAP_CV_RTV_HANDLE(hRenderTargetView), pColorRGBA);
	}
#endif
}