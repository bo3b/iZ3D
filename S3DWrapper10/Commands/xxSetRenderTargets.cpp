#include "stdafx.h"
#include "SetRenderTargets.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{1,   256},
		{2,   64},
		{3,   64},
		{4,   64},
		{16,  256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	xxSet<D3D10DDI_HRENDERTARGETVIEW>::value_vector::allocator_type	xxSet<D3D10DDI_HRENDERTARGETVIEW>::allocator(allocation_distribution, allocation_distribution + num_allocations);
	const D3D10DDI_HRENDERTARGETVIEW								xxSetRenderTargets::NULL_RENDER_TARGET_VIEW = MAKE_D3D10DDI_HRENDERTARGETVIEW(0);
	const D3D10DDI_HDEPTHSTENCILVIEW								xxSetRenderTargets::NULL_DEPTH_STENCIL_VIEW = MAKE_D3D10DDI_HDEPTHSTENCILVIEW(0);
	D3D10DDI_HRENDERTARGETVIEW										xxSetRenderTargets::phTempRenderTargetView_[D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT];
	
	void xxSetRenderTargets::MergeCommands( D3DDeviceWrapper* pWrapper, const base_type* pCmd, const xxSet* pFullCmd )
	{
		base_type::MergeCommands(pWrapper, pCmd, pFullCmd);

		const xxSetRenderTargets* pSetRt = static_cast<const xxSetRenderTargets*>(pCmd);
		hDepthStencilView_ = pSetRt->hDepthStencilView_;
		ClearTargets_ = pSetRt->ClearTargets_; // actually this variable has no meaning
	}

	void xxSetRenderTargets::Init()
	{
		ClearTargets_ = 0;
		NumValues_ = D3D10_DDI_SIMULTANEOUS_RENDER_TARGET_COUNT;
		hDepthStencilView_ = NULL_DEPTH_STENCIL_VIEW;
		bNeedCheckStereoResources_ = false;
		bIsRTStereo_ = false;
		bIsDSStereo_ = false;
		Values_.assign(NumValues_, NULL_RENDER_TARGET_VIEW);
	}

	void xxSetRenderTargets::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pState->m_OMRenderTargets.reset(this);//->MergeCommands(pWrapper, this);
		pState->m_OMRenderTargets->bNeedCheckStereoResources_ = true;
		pState->m_OMRenderTargets->IsUsedStereoResources(pWrapper); // check stereo resources
		pState->m_IsRTStereo = pState->m_OMRenderTargets->bIsRTStereo_;
		pState->m_IsDSStereo = pState->m_OMRenderTargets->bIsDSStereo_;
	}
	
	bool xxSetRenderTargets::IsUsedStereoResources( D3DDeviceWrapper* pWrapper ) const
	{
		if (bNeedCheckStereoResources_) 
		{
			bIsRTStereo_ = false;
			if (!Values_.empty())
			{
				D3D10DDI_HRENDERTARGETVIEW hRTV = Values_[0];
				if (hRTV.pDrvPrivate)
				{
					RenderTargetViewWrapper* pRTWrp = NULL;
					InitWrapper(hRTV, pRTWrp);
					if ( pRTWrp->IsStereo() )
						bIsRTStereo_ = true;
				}
			}

			bIsDSStereo_ = false;
			if (hDepthStencilView_.pDrvPrivate)
			{
				D3D10DDI_HDEPTHSTENCILVIEW hDSV = hDepthStencilView_;
				DepthStencilViewWrapper* pDSWrp = NULL;
				InitWrapper(hDSV, pDSWrp);
				if ( pDSWrp->IsStereo() )
					bIsDSStereo_ = true;
			}

			bNeedCheckStereoResources_ = false;
		}

		return bIsRTStereo_ || bIsDSStereo_;
	}

	void xxSetRenderTargets::SetDestResourceType( TextureType type )
	{
		for (UINT i = 0; i < NumValues_; i++)
		{
			if (!Values_[i].pDrvPrivate) continue;

			RenderTargetViewWrapper* pRVVWrapper = (RenderTargetViewWrapper*)Values_[i].pDrvPrivate;
			if (!pRVVWrapper->IsStereo()) continue;

			ResourceWrapper* pRes = pRVVWrapper->GetResourceWrapper();
			pRes->m_Type = type;
		}

		if (hDepthStencilView_.pDrvPrivate)
		{
			DepthStencilViewWrapper* pDSVWrapper = (DepthStencilViewWrapper*)hDepthStencilView_.pDrvPrivate;
			if (pDSVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pDSVWrapper->GetResourceWrapper();
				pRes->m_Type = type;
			}
		}

		bNeedCheckStereoResources_ = true;
	}
	
	void xxSetRenderTargets::GetDestResources(DestResourcesSet &res)
	{
		for (UINT i = 0; i < NumValues_; i++)
		{
			if (!Values_[i].pDrvPrivate) continue;

			RenderTargetViewWrapper* pRVVWrapper = (RenderTargetViewWrapper*)Values_[i].pDrvPrivate;
			if (!pRVVWrapper->IsStereo()) continue;

			ResourceWrapper* pRes = pRVVWrapper->GetResourceWrapper();
			res.insert(*pRes);
		}

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
	
	void xxSetRenderTargets::BuildCommand(CDumpState *ds)
	{
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
				output("ctx->OMSetRenderTargets(%d,&rtview_%d,sview_%d);",NumValues_,eid0,eid);
			}
			else
			{
				output("ctx->OMSetRenderTargets(%d,&rtview_%d,0);",NumValues_,eid0);
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
					output(eid == ds->GetMainRTViewEventId() ? draw : init,"rtvs_%d[%d] = rtview_%d;",EventID_,i,eid);
				}
			}

			if (hDepthStencilView_.pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(hDepthStencilView_);
				output("ctx->OMSetRenderTargets(%d,rtvs_%d,sview_%d);",NumValues_,EventID_,eid);
			}
			else
			{
				output("ctx->OMSetRenderTargets(%d,rtvs_%d,0);",NumValues_,EventID_);
			}
		}
		else
		{
			if (hDepthStencilView_.pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(hDepthStencilView_);
				output("ctx->OMSetRenderTargets(%d,0,sview_%d);",NumValues_,eid);
			}
			else
			{
				output("ctx->OMSetRenderTargets(%d,0,0);",NumValues_);
			}
		}
	}
}
