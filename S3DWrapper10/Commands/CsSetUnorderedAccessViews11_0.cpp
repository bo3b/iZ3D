#include "stdafx.h"
#include "CsSetUnorderedAccessViews11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{

	void CsSetUnorderedAccessViews11_0::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		// treat every non empty UAV as stereo
		pState->m_CsUnorderedAccessViews = this;
		pState->m_IsUAVStereo = IsUsedStereoResources(pWrapper);
	}

	bool CsSetUnorderedAccessViews11_0::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
	{
		bIsStereo_ = NumViews_ > 0; 
		// FIXME: following code should work, but pUAV is NULL...
		/*
		if (bNeedCheckStereoResources_) 
		{
			bIsStereo_ = false;
			for (UINT i = 0; i<NumViews_; ++i)
			{
				UnorderedAccessViewWrapper* pUAV = 0;
				InitWrapper(phUnorderedAccessView_[i], pUAV);
				if ( pUAV && pUAV->IsStereo() )
				{
					bIsStereo_ = true;
					break;
				}
			}

			bNeedCheckStereoResources_ = false;
		} 
		*/

		return bIsStereo_;
	}

	void CsSetUnorderedAccessViews11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		for (UINT i = 0; i < NumViews_; i++)
			phTempUnorderedAccessView_[i] = UNWRAP_CV_UAV_HANDLE(phUnorderedAccessView_[i]);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCsSetUnorderedAccessViews(pWrapper->hDevice, Offset_, NumViews_, 
			GetPointerToVector(phTempUnorderedAccessView_), GetPointerToVector(pUAVInitialCounts_));
		AFTER_EXECUTE(this);
	}

	bool CsSetUnorderedAccessViews11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CsSetUnorderedAccessViews11_0" );
		WriteStreamer::Value( "Offset", Offset_ );
		WriteStreamer::Value( "NumViews", NumViews_ );
		WriteStreamer::ArrayReferences( "phUnorderedAccessView", NumViews_, phUnorderedAccessView_ );
		WriteStreamer::ArrayValues( "pUAVInitialCounts", NumViews_, pUAVInitialCounts_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CsSetUnorderedAccessViews11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Offset_ );
		ReadStreamer::Value( NumViews_ );
		ReadStreamer::ArrayReferences( phUnorderedAccessView_, NumViews_ );
		ReadStreamer::ArrayValues( pUAVInitialCounts_, NumViews_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	TCsSetUnorderedAccessViewsCmdPtr CsSetUnorderedAccessViews11_0::Clone() const 
	{ 
		return new CsSetUnorderedAccessViews11_0( Offset_, 
												  NumViews_, 
												  NumViews_ > 0 ? &phUnorderedAccessView_[0] : 0, 
												  NumViews_ > 0 ? &pUAVInitialCounts_[0] : 0 ); 
	}

	void CsSetUnorderedAccessViews11_0::BuildCommand(CDumpState *ds)
	{
		if (NumViews_ == 0)
		{
			output("ctx->CSSetUnorderedAccessViews(%u,%u,0,0);",Offset_,NumViews_);
		}
		else
		{
			std::vector<int> uaveids;
			std::vector<UINT> uavsInitialCount;
			for(UINT i = 0;i < NumViews_;i++)
			{
				uaveids.push_back(phUnorderedAccessView_[i].pDrvPrivate ? ds->GetHandleEventId(phUnorderedAccessView_[i]) : 0);
				uavsInitialCount.push_back(pUAVInitialCounts_[i]);
			}

			static std::map<std::vector<int>,int> uavsCache;
			static std::map<std::vector<UINT>,int> uavsInitialCountCache;
			int uavsId = EventID_,uavsInitialCountId = EventID_;

			auto vbIt = uavsCache.find(uaveids);
			if (vbIt != uavsCache.end())
			{
				uavsId = vbIt->second;
			}
			else
			{
				uavsCache[uaveids] = EventID_;

				output(decl,"extern ID3D11UnorderedAccessView *uavs_%d[%d];",EventID_,NumViews_);
				output(glob,"ID3D11UnorderedAccessView *uavs_%d[%d];",EventID_,NumViews_);
				for(unsigned i = 0;i < NumViews_;i++)
				{
					if (phUnorderedAccessView_[i].pDrvPrivate)
					{
						int eid = ds->GetHandleEventId(phUnorderedAccessView_[i]);
						output(init,"uavs_%d[%d] = uav_%d;",EventID_,i,eid);
					}
				}
			}

			auto uavsInitialCountIt = uavsInitialCountCache.find(uavsInitialCount);
			if (uavsInitialCountIt != uavsInitialCountCache.end())
			{
				uavsInitialCountId = uavsInitialCountIt->second;
			}
			else
			{
				uavsInitialCountCache[uavsInitialCount] = EventID_;

				output(decl,"extern UINT uavs_initial_count_%d[%d];",EventID_,NumViews_);
				output(glob,"UINT uavs_initial_count_%d[%d];",EventID_,NumViews_);
				for(unsigned i = 0;i < NumViews_;i++)
				{
					if (pUAVInitialCounts_[i])
						output(init,"uavs_initial_count_%d[%d] = %u;",EventID_,i,pUAVInitialCounts_[i]); 
				}
			}

			output("ctx->CSSetUnorderedAccessViews(%u,%u,uavs_%d,uavs_initial_count_%d);",Offset_,NumViews_,uavsId,uavsInitialCountId);
		}
	}

}

VOID ( APIENTRY CsSetUnorderedAccessViews11_0 )( D3D10DDI_HDEVICE  hDevice, UINT  Offset, UINT  NumViews, 
							CONST D3D11DDI_HUNORDEREDACCESSVIEW*  phUnorderedAccessView, CONST UINT*  phSizes )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::CsSetUnorderedAccessViews11_0* command = new Commands::CsSetUnorderedAccessViews11_0(Offset, NumViews, phUnorderedAccessView, phSizes);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D11DDI_HUNORDEREDACCESSVIEW>::type  UAVBuffer(NumViews);
	for (UINT i = 0; i < NumViews; i++)
		UAVBuffer[i] = UNWRAP_CV_UAV_HANDLE(phUnorderedAccessView[i]);
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCsSetUnorderedAccessViews(D3D10_DEVICE, Offset, NumViews, GetPointerToVector(UAVBuffer), phSizes);
	}
#endif	
}