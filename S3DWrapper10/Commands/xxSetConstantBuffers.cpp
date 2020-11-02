#include "stdafx.h"
#include "xxSetConstantBuffers.h"
#include "xxSetShader.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{1,   16384},
		{2,   2048},
		{3,   2048},
		{4,   2048},
		{16,  4096}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	xxSet<D3D10DDI_HRESOURCE>::value_vector::allocator_type	xxSet<D3D10DDI_HRESOURCE>::allocator(allocation_distribution, allocation_distribution + num_allocations);
	const D3D10DDI_HRESOURCE								xxSetConstantBuffers::NULL_RESOURCE = MAKE_D3D10DDI_HRESOURCE(0);
	D3D10DDI_HRESOURCE										xxSetConstantBuffers::phTempBuffers_[D3D10_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT];
	
	void xxSetConstantBuffers::Init()
	{
		Offset_ = 0;
		NumValues_ = D3D10_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT;
		bIsUsedStereoResources_ = false;
		bNeedCheckStereoResources_ = false;
		Values_.assign(NumValues_, NULL_RESOURCE);
	}

	template<typename T>
	void xxSetConstantBuffers::SetMatrices(D3DDeviceWrapper *pWrapper, const T* pShaderData, DWORD shaderCRC32)
	{
		assert(pShaderData);

		UINT nStart	= Offset_;
		UINT nEnd   = Offset_ + NumValues_;
		for (auto mit  = pShaderData->matrixData.cb.cbegin(); 
				  mit != pShaderData->matrixData.cb.end(); 
				  ++mit)
		{
			UINT nCBIndex = mit->first;
			if (nCBIndex >= nStart && nCBIndex < nEnd)
			{
				ResourceWrapper* pBufferWrapper = NULL;
				InitWrapper(Values_[nCBIndex - nStart], pBufferWrapper);
				if (!pBufferWrapper) {
					continue;
				}

				if (!pBufferWrapper->m_pCB) 
				{
					pBufferWrapper->InitializeCB(pWrapper);
					if (!pBufferWrapper->m_pCB) {
						continue;
					}
				}

				pBufferWrapper->m_pCB->Initialize(pWrapper, CBUpdateData(mit->second), shaderCRC32);
			}
		}
	}

	template<>
	void xxSetConstantBuffers::SetMatrices(D3DDeviceWrapper *pWrapper, const ModifiedShaderData* pShaderData, DWORD shaderCRC32)
	{
		assert(pShaderData);

		UINT nCBIndex = pShaderData->CBIndex;
		if (nCBIndex >= Offset_ && nCBIndex < Offset_ + NumValues_)
		{
			ResourceWrapper* pBufferWrapper = NULL;
			InitWrapper(Values_[nCBIndex - Offset_], pBufferWrapper);
			if (!pBufferWrapper) {
				return;
			}

			if (!pBufferWrapper->m_pCB) 
			{
				pBufferWrapper->InitializeCB(pWrapper);
				if (!pBufferWrapper->m_pCB) {
					return;
				}
			}

			pBufferWrapper->m_pCB->Initialize(pWrapper, CBUpdateData(*pShaderData), shaderCRC32);
		}
	}

	bool xxSetConstantBuffers::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (bNeedCheckStereoResources_) 
		{
			bIsUsedStereoResources_ = false;
			for (UINT i = 0; i < Values_.size(); i++)
			{
				if (IS_STEREO_RESOURCE_HANDLE(Values_[i]))
				{
					bIsUsedStereoResources_ = true;
					break;
				}
			}

			bNeedCheckStereoResources_ = false;
		}
		
		return bIsUsedStereoResources_;
	}
	
	bool xxSetConstantBuffers::WriteToFileEx( D3DDeviceWrapper *pWrapper, const char* szCommandName ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this, szCommandName );
		WriteStreamer::Value( "StartBuffer", Offset_ );
		WriteStreamer::Value( "NumBuffers", NumValues_ );
		WriteStreamer::ArrayReferences( "hBuffers", NumValues_, Values_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool xxSetConstantBuffers::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( Offset_ );
		ReadStreamer::Value( NumValues_ );
		ReadStreamer::ArrayReferences( Values_, NumValues_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void xxSetConstantBuffers::UpdateDeviceState( D3DDeviceWrapper* pWrapper, D3DDeviceState* pState )
	{
		pState->GetShaderPipeline(ShaderPipeline_)->m_ConstantBuffers->MergeCommands(pWrapper, this);
	}

	template void xxSetConstantBuffers::SetMatrices<ProjectionShaderData>(D3DDeviceWrapper*, const ProjectionShaderData*, DWORD);
	template void xxSetConstantBuffers::SetMatrices<UnprojectionShaderData>(D3DDeviceWrapper*, const UnprojectionShaderData*, DWORD);

	void xxSetConstantBuffers::BuildCommand(CDumpState *ds)
	{
		if (NumValues_ == 1 && Values_[0].pDrvPrivate)
		{
			output("ctx->%cSSetConstantBuffers(%u,%u,&buffer_%d);",
				Char(),Offset_,NumValues_,ds->GetHandleEventId(Values_[0]));
		}
		else if (NumValues_ > 0)
		{
			std::vector<int> eids;
			for(UINT i = 0;i < NumValues_;i++)
			{
				eids.push_back(Values_[i].pDrvPrivate ? ds->GetHandleEventId(Values_[i]) : 0);
			}

			static std::map<std::vector<int>,int> CBCache;
			auto it = CBCache.find(eids);
			if (it != CBCache.end())
			{
				output("ctx->%cSSetConstantBuffers(%u,%u,const_buffers_%d);",Char(),Offset_,NumValues_,it->second);
			}
			else
			{
				CBCache[eids] = EventID_;

				output(glob,"DXBUFFER *const_buffers_%d[%d];",EventID_,NumValues_);
				for(UINT i = 0;i < NumValues_;i++)
				{
					if (Values_[i].pDrvPrivate)
					{
						int eid = ds->GetHandleEventId(Values_[i]);
						output(init,"const_buffers_%d[%d] = buffer_%d;",EventID_,i,eid);
					}
				}

				output("ctx->%cSSetConstantBuffers(%u,%u,const_buffers_%d);",Char(),Offset_,NumValues_,EventID_);
			}
		}
		else
		{
			output("ctx->%cSSetConstantBuffers(%u,%u,0);",Char(),Offset_,NumValues_);
		}
	}
}
