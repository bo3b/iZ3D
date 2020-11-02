#include "stdafx.h"
#include "xxSetSamplers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{1,   2048},
		{2,   64},
		{3,   64},
		{4,   64},
		{8,   64},
		{16,  256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	const D3D10DDI_HSAMPLER									xxSetSamplers::NULL_SAMPLER = MAKE_D3D10DDI_HSAMPLER(0);
	xxSet<D3D10DDI_HSAMPLER>::value_vector::allocator_type	xxSet<D3D10DDI_HSAMPLER>::allocator(allocation_distribution, allocation_distribution + num_allocations);

	bool xxSetSamplers::WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  szCommandName );
		WriteStreamer::Value( "Offset", Offset_ );
		WriteStreamer::Value( "NumSamplers", NumValues_ );
		WriteStreamer::ArrayReferences( "hSamplers", NumValues_, Values_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool xxSetSamplers::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( Offset_);
		ReadStreamer::Value( NumValues_ );
		ReadStreamer::ArrayReferences( Values_, NumValues_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void xxSetSamplers::UpdateDeviceState( D3DDeviceWrapper* pWrapper, D3DDeviceState* pState )
	{
		if (ShaderPipeline_ != SP_NONE) {
			pState->GetShaderPipeline(ShaderPipeline_)->m_Samplers->MergeCommands(pWrapper, this);
		}
	}

	void xxSetSamplers::BuildCommand(CDumpState *ds)
	{
		if (NumValues_ == 1 && Values_[0].pDrvPrivate)
		{
			int eid = ds->GetHandleEventId(Values_[0]);
			output("ctx->%cSSetSamplers(%u,%u,&samp_state_%d);",Char(),Offset_,NumValues_,eid);
		}
		else
		{
			std::vector<int> eids;
			for(UINT i = 0;i < NumValues_;i++)
			{
				eids.push_back(Values_[i].pDrvPrivate ? ds->GetHandleEventId(Values_[i]) : 0);
			}

			static std::map<std::vector<int>,int> statesCache;
			auto it = statesCache.find(eids);
			if (it != statesCache.end())
			{
				output("ctx->%cSSetSamplers(%u,%u,states_%d);",Char(),Offset_,NumValues_,it->second);
			}
			else
			{
				statesCache[eids] = EventID_;

				output(glob,"DXSAMPSTATE *states_%d[%d];",EventID_,NumValues_);
				for(unsigned i = 0;i < NumValues_;i++)
				{
					if (Values_[i].pDrvPrivate)
					{
						int eid = ds->GetHandleEventId(Values_[i]);
						output(init,"states_%d[%d] = samp_state_%d;",EventID_,i,eid);
					}
				}

				output("ctx->%cSSetSamplers(%u,%u,states_%d);",Char(),Offset_,NumValues_,EventID_);
			}
		}
	}
}
