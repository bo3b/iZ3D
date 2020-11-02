#include "stdafx.h"
#include "xxSetShaderResources.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{1,   4096},
		{2,   512},
		{3,   256},
		{4,   128},
		{6,   1024},
		{8,   128},
		{16,  256},
		{32,  64},
		{128, 256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	xxSet<D3D10DDI_HSHADERRESOURCEVIEW>::value_vector::allocator_type	xxSet<D3D10DDI_HSHADERRESOURCEVIEW>::allocator(allocation_distribution, allocation_distribution + num_allocations);
	const D3D10DDI_HSHADERRESOURCEVIEW									xxSetShaderResources::NULL_RESOURCE_VIEW = MAKE_D3D10DDI_HSHADERRESOURCEVIEW(0);
	D3D10DDI_HSHADERRESOURCEVIEW										xxSetShaderResources::phTempShaderResourceViews_[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

	void xxSetShaderResources::Init()
	{
		Offset_ = 0;
		NumValues_ = D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;
		bIsUsedStereoResources_ = false;
		bNeedCheckStereoResources_ = false;
		Values_.assign(NumValues_, NULL_RESOURCE_VIEW);
	}

	bool xxSetShaderResources::WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  szCommandName );
		WriteStreamer::Value( "Offset", Offset_ );
		WriteStreamer::Value( "NumViews", NumValues_ );

		for(unsigned i = 0;i < NumValues_;i++)
			WriteStreamer::Value( "COUNT", (UINT)0 );
		/*if ( GINFO_DUMP_ON )
		{
			for(unsigned i = 0;i < NumViews_;i++)
			{
				D3D10DDI_HRESOURCE hres = Bd().GetResourceOfView(phShaderResourceViews_[i]);
				std::vector<int> poses = Bd().SaveResourceToBinaryFileEx(pWrapper,hres);

				WriteStreamer::Value( "COUNT", (UINT)poses.size() );
				for(UINT k = 0;k < poses.size();k++)
				{
					WriteStreamer::Value( "data.bin", poses[k] );
				}
			}
		}*/

		WriteStreamer::ArrayReferences( "hShaderResourceViews", NumValues_, Values_ );

		WriteStreamer::CmdEnd();
		return true;
	}

	bool xxSetShaderResources::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( Offset_);
		ReadStreamer::Value( NumValues_);

		for(unsigned i0 = 0;i0 < NumValues_;i0++)
		{
			UINT cnt;
			ReadStreamer::Value( cnt );

			for(UINT k = 0;k < cnt;k++)
			{
				int databin_pos;
				ReadStreamer::Value( databin_pos );
			}
		}

		ReadStreamer::ArrayReferences( Values_, NumValues_);

		ReadStreamer::CmdEnd();
		return true;
	}
	
	bool xxSetShaderResources::IsUsedStereoResources(D3DDeviceWrapper* pWrapper) const
	{
		if (bNeedCheckStereoResources_) 
		{
			bIsUsedStereoResources_ = false;
			for (UINT i = 0; i < Values_.size(); i++)
			{
				ShaderResourceViewWrapper* pSRV;
				InitWrapper(Values_[i], pSRV);
				if (pSRV)
				{
					ResourceWrapper* pRes = pSRV->GetResourceWrapper();
					if (pRes->IsStereoState())
					{
						if (!pSRV->IsStereo())
							pSRV->CreateRightView(pWrapper);
						bIsUsedStereoResources_ = true;
					}
				}
			}

			bNeedCheckStereoResources_ = false;
		}
		
		return bIsUsedStereoResources_;
	}

	void xxSetShaderResources::UpdateDeviceState( D3DDeviceWrapper* pWrapper, D3DDeviceState* pState )
	{
		pState->GetShaderPipeline(ShaderPipeline_)->m_ShaderResources->MergeCommands(pWrapper, this);
	}

	void xxSetShaderResources::BuildCommand(CDumpState *ds)
	{
		if (NumValues_ == 0)
		{
			output("ctx->%cSSetShaderResources(%u,%u,0);",Char(),Offset_,NumValues_);
		}
		else if (NumValues_ == 1 && Values_[0].pDrvPrivate)
		{
			int eid = ds->GetHandleEventId(Values_[0]);
			output("ctx->%cSSetShaderResources(%u,%u,&res_view_%d);",Char(),Offset_,NumValues_,eid);
		}
		else
		{
			std::vector<int> eids;
			for(UINT i = 0;i < NumValues_;i++)
			{
				eids.push_back(Values_[i].pDrvPrivate ? ds->GetHandleEventId(Values_[i]) : 0);
			}

			static std::map<std::vector<int>,int> resViewsCache;
			auto it = resViewsCache.find(eids);
			if (it != resViewsCache.end())
			{
				output("ctx->%cSSetShaderResources(%u,%u,res_views_%d);",Char(),Offset_,NumValues_,it->second);
			}
			else
			{
				resViewsCache[eids] = EventID_;

				output(glob,"DXSHADERRESVIEW *res_views_%d[%d];",EventID_,NumValues_);
				for(unsigned i = 0;i < NumValues_;i++)
				{
					if (Values_[i].pDrvPrivate)
					{
						int eid = ds->GetHandleEventId(Values_[i]);
						output(init,"res_views_%d[%d] = res_view_%d;",EventID_,i,eid);
					}
				}

				output("ctx->%cSSetShaderResources(%u,%u,res_views_%d);",Char(),Offset_,NumValues_,EventID_);
			}
		}
	}
}
