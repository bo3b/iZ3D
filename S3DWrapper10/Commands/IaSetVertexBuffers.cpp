#include "stdafx.h"
#include "IaSetVertexBuffers.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{2,   128},
		{3,   64},
		{4,   2048},
		{16,  64}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	const IaBuffer									IaSetVertexBuffers::NULL_VALUE = { MAKE_D3D10DDI_HRESOURCE(0), 0, 0 };
	xxSet<IaBuffer>::value_vector::allocator_type	xxSet<IaBuffer>::allocator(allocation_distribution, allocation_distribution + num_allocations);
	D3D10DDI_HRESOURCE								IaSetVertexBuffers::phTempBuffers_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	UINT											IaSetVertexBuffers::phTempOffsets_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	UINT											IaSetVertexBuffers::phTempStrides_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

	IaSetVertexBuffers::IaSetVertexBuffers()
	{
		CommandId = idIaSetVertexBuffers;
		State_    = COMMAND_CHANGE_DEVICE_STATE;
	}

	IaSetVertexBuffers::IaSetVertexBuffers( UINT						StartBuffer, 
											UINT						NumBuffers, 
											CONST D3D10DDI_HRESOURCE*	phBuffers, 
											CONST UINT*					pStrides, 
											CONST UINT*					pOffsets )
	{
		Offset_    = StartBuffer;
		NumValues_ = NumBuffers;
		Values_.resize(NumValues_);
		for (size_t i = 0; i<NumValues_; ++i)
		{
			Values_[i].buffer = phBuffers[i];
			Values_[i].offset = pOffsets[i];
			Values_[i].stride = pStrides[i];
		}
		CommandId = idIaSetVertexBuffers;
		State_    = COMMAND_CHANGE_DEVICE_STATE;
	}
	
	IaSetVertexBuffers::IaSetVertexBuffers( UINT			StartBuffer, 
											UINT			NumBuffers, 
											CONST IaBuffer*	buffers )
	:	base_type(StartBuffer, NumBuffers, buffers)	
	{
		CommandId = idIaSetVertexBuffers;
		State_	  = COMMAND_CHANGE_DEVICE_STATE;
	}

	void IaSetVertexBuffers::Execute( D3DDeviceWrapper *pWrapper )
	{		
		// only mono
		for (UINT i = 0; i < NumValues_; i++)
		{
			phTempBuffers_[i] = GET_RESOURCE_HANDLE(Values_[i].buffer);
			phTempOffsets_[i] = Values_[i].offset;
			phTempStrides_[i] = Values_[i].stride;
		}

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnIaSetVertexBuffers(pWrapper->hDevice, Offset_, NumValues_, phTempBuffers_, phTempStrides_, phTempOffsets_);
		AFTER_EXECUTE(this); 

		Bd().SetLastResources(pWrapper,CommandId,Values_.empty() ? 0 : Values_.data(),NumValues_,Offset_);
	}

	void IaSetVertexBuffers::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		pWrapper->m_DeviceState.m_IaVertexBuffers->MergeCommands(pWrapper, this);
	}
		
	void IaSetVertexBuffers::Init()
	{
		Offset_ = 0;
		NumValues_ = D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
		Values_.assign(NumValues_, NULL_VALUE);
	}

	bool IaSetVertexBuffers::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		for (UINT i = 0; i < NumValues_; i++)
		{
			phTempBuffers_[i] = Values_[i].buffer;
			phTempOffsets_[i] = Values_[i].offset;
			phTempStrides_[i] = Values_[i].stride;
		}

		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "IaSetVertexBuffers" );
		WriteStreamer::Value( "StartBuffer", Offset_ );
		WriteStreamer::Value( "NumBuffers", NumValues_ );
		WriteStreamer::ArrayReferences( "hBuffers", NumValues_, phTempBuffers_ );
		WriteStreamer::ArrayValues( "Strides", NumValues_, phTempStrides_ );
		WriteStreamer::ArrayValues( "Offsets", NumValues_, phTempOffsets_ );			
		WriteStreamer::CmdEnd();
		return true;
	}

	bool IaSetVertexBuffers::ReadFromFile()	
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Offset_ );
		ReadStreamer::Value( NumValues_ );
		ReadStreamer::ArrayReferences( phTempBuffers_, NumValues_ );
		ReadStreamer::ArrayValues( phTempStrides_, NumValues_ );
		ReadStreamer::ArrayValues( phTempOffsets_, NumValues_ );
		ReadStreamer::CmdEnd();
		
		Values_.resize(NumValues_);
		for (UINT i = 0; i < NumValues_; i++)
		{
			Values_[i].buffer = phTempBuffers_[i];
			Values_[i].offset = phTempOffsets_[i];
			Values_[i].stride = phTempStrides_[i];
		}

		return true;
	}

	void IaSetVertexBuffers::BuildCommand(CDumpState *ds)
	{
		std::vector<UINT> strs;
		std::vector<UINT> offs;
		std::vector<int> eids;

		for(unsigned i = 0;i < NumValues_;i++)
		{
			strs.push_back(Values_[i].stride);
			offs.push_back(Values_[i].offset);
			eids.push_back(Values_[i].buffer.pDrvPrivate ? ds->GetHandleEventId(Values_[i].buffer) : 0);
		}

		static std::map<std::vector<UINT>,int> strideCache;
		static std::map<std::vector<UINT>,int> offsetCache;
		static std::map<std::vector<int>,int> vbCache;
		int strideId = EventID_,offsetId = EventID_,vertexBufferId = EventID_;

		auto strideIt = strideCache.find(strs);
		if (strideIt != strideCache.end())
		{
			strideId = strideIt->second;
		}
		else
		{
			strideCache[strs] = EventID_;

			output(decl,"extern UINT stride_%d[%d];",EventID_,NumValues_);
			output(glob,"UINT stride_%d[%d];",EventID_,NumValues_);
			for(unsigned i = 0;i < NumValues_;i++)
			{
				if (Values_[i].stride)
					output(init,"stride_%d[%d] = %u;",EventID_,i,Values_[i].stride);
			}
		}

		auto offsetIt = offsetCache.find(offs);
		if (offsetIt != offsetCache.end())
		{
			offsetId = offsetIt->second;
		}
		else
		{
			offsetCache[offs] = EventID_;

			output(decl,"extern UINT offset_%d[%d];",EventID_,NumValues_);
			output(glob,"UINT offset_%d[%d];",EventID_,NumValues_);
			for(unsigned i = 0;i < NumValues_;i++)
			{
				if (Values_[i].offset)
					output(init,"offset_%d[%d] = %u;",EventID_,i,Values_[i].offset); 
			}
		}

		auto vbIt = vbCache.find(eids);
		if (vbIt != vbCache.end())
		{
			vertexBufferId = vbIt->second;
		}
		else
		{
			vbCache[eids] = EventID_;

			output(decl,"extern DXBUFFER *vertexBuffers_%d[%d];",EventID_,NumValues_);
			output(glob,"DXBUFFER *vertexBuffers_%d[%d];",EventID_,NumValues_);
			for(unsigned i = 0;i < NumValues_;i++)
			{
				if (Values_[i].buffer.pDrvPrivate)
				{
					int eid = ds->GetHandleEventId(Values_[i].buffer);
					output(init,"vertexBuffers_%d[%d] = buffer_%d;",EventID_,i,eid);
				}
			}
		}

		output("ctx->IASetVertexBuffers(%u,%u,vertexBuffers_%d,stride_%d,offset_%d);",
			Offset_,NumValues_,vertexBufferId,strideId,offsetId);
	}

}

VOID ( APIENTRY IaSetVertexBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
									 CONST D3D10DDI_HRESOURCE*  phBuffers, CONST UINT*  pStrides, CONST UINT*  pOffsets )
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::IaSetVertexBuffers* command = new Commands::IaSetVertexBuffers(StartBuffer, NumBuffers, phBuffers, pStrides, pOffsets);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(NumBuffers);
	for (UINT i = 0; i < NumBuffers; i++)
		hRESOURCEBuffer[i] = UNWRAP_CV_RESOURCE_HANDLE(phBuffers[i]);
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnIaSetVertexBuffers(D3D10_DEVICE, StartBuffer, NumBuffers, GetPointerToVector( hRESOURCEBuffer ), pStrides, pOffsets);
	}
#endif	
}
