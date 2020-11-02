#include "stdafx.h"
#include "SetRenderTargets11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"
#include "RenderTargetViewWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{
	D3D11DDI_HUNORDEREDACCESSVIEW SetRenderTargets11_0::phTempUnorderedAccessView_[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];

	void SetRenderTargets11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		for (UINT i = 0; i < NumValues_; i++)
			phTempRenderTargetView_[i] = UNWRAP_CV_RTV_HANDLE(Values_[i]);
		for (UINT i = 0; i < NumUAVs_; i++)
			phTempUnorderedAccessView_[i] = UNWRAP_CV_UAV_HANDLE(phUnorderedAccessView_[i]);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnSetRenderTargets( pWrapper->hDevice, phTempRenderTargetView_, 
			NumValues_, 
			GetClearCount(NumValues_, pWrapper->m_DeviceState.m_NumRenderTargets), 
			UNWRAP_CV_DSV_HANDLE(hDepthStencilView_),
			phTempUnorderedAccessView_, 
			GetPointerToVector(pUAVInitialCounts_), 
			UAVIndex_, 
			NumUAVs_, 
			UAVFirsttoSet_, 
			UAVNumberUpdated_ );
		AFTER_EXECUTE(this); 
	}

	bool SetRenderTargets11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetRenderTargets11_0" );
		WriteStreamer::Value( "RTargets", NumValues_ );
		WriteStreamer::ArrayReferences( "hRenderTargetView", NumValues_, Values_ );
		WriteStreamer::Value( "ClearTargets", ClearTargets_ );
		WriteStreamer::Reference( "hDepthStencilView", hDepthStencilView_ );

		WriteStreamer::Value( "NumUAVs", NumUAVs_ );
		WriteStreamer::ArrayReferences( "phUnorderedAccessView", NumUAVs_, phUnorderedAccessView_ );
		WriteStreamer::ArrayValues( "pUAVInitialCounts", NumUAVs_, pUAVInitialCounts_ );
		WriteStreamer::Value( "UAVIndex", UAVIndex_ );
		WriteStreamer::Value( "UAVFirstToSet", UAVFirsttoSet_ );
		WriteStreamer::Value( "UAVNumberUpdated", UAVNumberUpdated_ );

		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetRenderTargets11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( NumValues_ );
		ReadStreamer::ArrayReferences( Values_, NumValues_ );
		ReadStreamer::Value( ClearTargets_ );
		ReadStreamer::Reference( hDepthStencilView_ );

		ReadStreamer::Value( NumUAVs_ );
		ReadStreamer::ArrayReferences( phUnorderedAccessView_, NumUAVs_ );
		ReadStreamer::ArrayValues( pUAVInitialCounts_, NumUAVs_ );
		ReadStreamer::Value( UAVIndex_ );
		ReadStreamer::Value( UAVFirsttoSet_ );
		ReadStreamer::Value( UAVNumberUpdated_ );

		ReadStreamer::CmdEnd();
		return true;
	}

	void SetRenderTargets11_0::BuildCommand(CDumpState *ds)
	{
		if (NumUAVs_ == 0)
		{
			xxSetRenderTargets::BuildCommand(ds);
			return;
		}

		// TODO
		__debugbreak();
		for(unsigned i = 0;i < NumValues_;i++)
		{
			D3D10DDI_HRESOURCE hres = ds->GetResourceOfView(Values_[i]);
			ds->SetUsedAsRenderTarget(hres);
			if (hres.pDrvPrivate) output("// lock: buffer_%d",ds->GetHandleEventId(hres));
		}

		if (NumValues_ == 1 && Values_[0].pDrvPrivate)
		{
			int eid0 = ds->GetHandleEventId(Values_[0]);

			if (hDepthStencilView_.pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(hDepthStencilView_);
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,&rtview_%d,sview_%d,%d,0,0);",NumValues_,eid0,eid,NumUAVs_);
			}
			else
			{
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,&rtview_%d,0,%d,0,0);",NumValues_,eid0,NumUAVs_);
			}
		}
		else if (NumValues_ > 0)
		{
			output(glob,"DXRTVIEW *rtvs_%d[%d];",EventID_,NumValues_);
			for(unsigned i = 0;i < NumValues_;i++)
			{
				if (Values_[i].pDrvPrivate)
				{
					int eid = ds->GetHandleEventId(Values_[i]);
					output(eid == ds->GetMainRTViewEventId() ? draw : init,"rtvs_%d[%d] = rtview_%d;",EventID_,i,eid,NumUAVs_);
				}
			}

			if (hDepthStencilView_.pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(hDepthStencilView_);
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,rtvs_%d,sview_%d,%d,0,0);",NumValues_,EventID_,eid,NumUAVs_);
			}
			else
			{
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,rtvs_%d,0,%d,0,0);",NumValues_,EventID_,NumUAVs_);
			}
		}
		else
		{
			if (hDepthStencilView_.pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(hDepthStencilView_);
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,0,sview_%d,%d,0,0);",NumValues_,eid,NumUAVs_);
			}
			else
			{
				output("ctx->OMSetRenderTargetsAndUnorderedAccessViews(%d,0,0,%d,0,0,%d,0,0);",NumValues_,NumUAVs_);
			}
		}
	}
}

VOID ( APIENTRY SetRenderTargets11_0 )( D3D10DDI_HDEVICE  hDevice,  CONST D3D10DDI_HRENDERTARGETVIEW*  phRenderTargetView, 
								   UINT  RTargets, UINT  ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView,
								   CONST D3D11DDI_HUNORDEREDACCESSVIEW*  phUnorderedAccessView,  CONST UINT*  pUAV, 
								   UINT  UAVIndex, UINT  NumUAVs, UINT  UAVFirsttoSet, UINT  UAVNumberUpdated)
{	
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetRenderTargets11_0* command = new Commands::SetRenderTargets11_0( phRenderTargetView, RTargets, ClearTargets, hDepthStencilView,
		phUnorderedAccessView, pUAV, UAVIndex, NumUAVs, UAVFirsttoSet, UAVNumberUpdated);
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	Commands::fast_vector<D3D10DDI_HRENDERTARGETVIEW>::type  RTVBuffer(RTargets);
	Commands::fast_vector<D3D11DDI_HUNORDEREDACCESSVIEW>::type UAVBuffer(NumUAVs);
	for (UINT i = 0; i < RTargets; i++)
		RTVBuffer[i] = UNWRAP_CV_RTV_HANDLE(phRenderTargetView[i]);
	for (UINT i = 0; i < NumUAVs; i++)
		UAVBuffer[i] = UNWRAP_CV_UAV_HANDLE(phUnorderedAccessView[i]);
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnSetRenderTargets(D3D10_DEVICE, GetPointerToVector(RTVBuffer), RTargets, ClearTargets, UNWRAP_CV_DSV_HANDLE(hDepthStencilView),
			GetPointerToVector(UAVBuffer), pUAV, UAVIndex, NumUAVs, UAVFirsttoSet, UAVNumberUpdated );
	}
#endif	
}