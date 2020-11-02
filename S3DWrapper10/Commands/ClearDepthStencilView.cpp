#include "stdafx.h"
#include "ClearDepthStencilView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"

namespace Commands
{

	void ClearDepthStencilView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnClearDepthStencilView(pWrapper->hDevice, UNWRAP_CV_DSV_HANDLE(hDepthStencilView_),
			Flags_, Depth_, Stencil_);
		AFTER_EXECUTE(this); 
	}

	bool ClearDepthStencilView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ClearDepthStencilView" );
		WriteStreamer::Reference( "hDepthStencilView", hDepthStencilView_ );
		WriteStreamer::Value( "Flags", Flags_ );
		WriteStreamer::Value( "Depth", Depth_ );
		WriteStreamer::Value( "Stencil", Stencil_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ClearDepthStencilView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDepthStencilView_ );
		ReadStreamer::Value( Flags_ );
		ReadStreamer::Value( Depth_ );
		ReadStreamer::Value( Stencil_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ClearDepthStencilView::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_DSV_HANDLE(hDepthStencilView_))
			return true;
		return false;
	}

	void ClearDepthStencilView::SetDestResourceType( TextureType type )
	{
		if (hDepthStencilView_.pDrvPrivate)
		{
			DepthStencilViewWrapper* pDSVWrapper = (DepthStencilViewWrapper*)hDepthStencilView_.pDrvPrivate;
			if (pDSVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pDSVWrapper->GetResourceWrapper();
				pRes->m_Type = type;
			}
		} 
	}

	void ClearDepthStencilView::GetDestResources(DestResourcesSet &res)
	{
		if (hDepthStencilView_.pDrvPrivate)
		{
			DepthStencilViewWrapper* pDSVWrapper = (DepthStencilViewWrapper*)hDepthStencilView_.pDrvPrivate;
			if (pDSVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pDSVWrapper->GetResourceWrapper();
				res.insert(*pRes);
			}
		} 
	}

	void ClearDepthStencilView::BuildCommand(CDumpState *ds)
	{
		output("ctx->ClearDepthStencilView(sview_%d,%d,%f,%d);",
			ds->GetHandleEventId(hDepthStencilView_),Flags_,Depth_,Stencil_);
	}
}

VOID ( APIENTRY ClearDepthStencilView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, 
										UINT  Flags, FLOAT  Depth, UINT8  Stencil )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ClearDepthStencilView* command = new Commands::ClearDepthStencilView(hDepthStencilView, Flags, Depth, Stencil);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnClearDepthStencilView(D3D10_DEVICE, UNWRAP_CV_DSV_HANDLE(hDepthStencilView), Flags, Depth, Stencil);
	}
#endif
}