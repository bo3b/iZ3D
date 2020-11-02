#include "stdafx.h"
#include "xxUpdateSubresourceUP.h"
#include "D3DDeviceWrapper.h"

namespace Commands 
{
	xxUpdateSubresourceUP::xxUpdateSubresourceUP()
		:   SysMemUP_(0)
		,	pWrapper_(0)
		,   pDstBox_(0)
		,   pSysMemUP_(0)
		,   RowPitch_(0)
		,   DepthPitch_(0)
	{
	}

	xxUpdateSubresourceUP::~xxUpdateSubresourceUP()
	{
		if (SysMemUP_) {
			pWrapper_->m_ConstantBufferAllocator.deallocate(SysMemUP_);
		}
	}

	UINT xxUpdateSubresourceUP::Size() const
	{
		UINT size;
		ResourceWrapper* pRes;
		InitWrapper(hDstResource_, pRes);
		if (pDstBox_) {
			size = (pDstBox_->back - pDstBox_->front) * (pDstBox_->right - pDstBox_->left) * (pDstBox_->bottom - pDstBox_->top);
		}
		else
		{
			UINT MipSlice = DstSubresource_ % pRes->m_CreateResource.MipLevels;
			D3D10DDI_MIPINFO& info = pRes->m_MipInfoList[MipSlice];
			size = info.PhysicalWidth * info.PhysicalHeight * info.PhysicalDepth;
		}
		if (pRes->m_CreateResource.ResourceDimension != D3D10DDIRESOURCE_BUFFER && pRes->m_CreateResource.ResourceDimension != D3D11DDIRESOURCE_BUFFEREX)
		{
			size = (UINT)(size * GetBitWidthOfDXGIFormat(pRes->m_CreateResource.Format) / 8);
		}
		return size;
	}

	bool xxUpdateSubresourceUP::MakeInternalDataCopy(D3DDeviceWrapper* pWrapper)
	{
		if (!SysMemUP_) 
		{
			UINT size  = Size();
			SysMemUP_  = (BYTE*)pWrapper->m_ConstantBufferAllocator.allocate(size);
			if (SysMemUP_)
			{
				// pSysMemUP_ can be NULL
				if (pSysMemUP_) {
					memcpy(SysMemUP_, pSysMemUP_, size);
				}
				pSysMemUP_ = SysMemUP_;
				pWrapper_ = pWrapper;
				return true;
			}

			return false;
		}

		return true;
	}
	
	bool xxUpdateSubresourceUP::AllocateMemory(D3DDeviceWrapper* pWrapper, size_t size)
	{
		if (!SysMemUP_) {
			SysMemUP_ = (BYTE*)pWrapper->m_ConstantBufferAllocator.allocate(size);
		}
		else if ( size > sizeof(SysMemUP_) )
		{
			pWrapper->m_ConstantBufferAllocator.deallocate(SysMemUP_);
			SysMemUP_ = (BYTE*)pWrapper->m_ConstantBufferAllocator.allocate(size);
		}
		else {
			return true;
		}

		if (SysMemUP_) 
		{
			pSysMemUP_ = SysMemUP_;
			pWrapper_  = pWrapper;
			return true;
		}

		return false;
	}

	bool xxUpdateSubresourceUP::Merge(D3DDeviceWrapper* pWrapper, const xxUpdateSubresourceUP& cmd)
	{
		_ASSERT( hDstResource_ == cmd.hDstResource_ 
				 && DstSubresource_ == cmd.DstSubresource_
				 && RowPitch_ == cmd.RowPitch_ 
				 && DepthPitch_ == cmd.DepthPitch_ );

		if (!cmd.pDstBox_)
		{
			UINT size = cmd.Size();
			if ( !AllocateMemory(pWrapper, size) ) {
				return false;
			}

			memcpy( (VOID*)pSysMemUP_, cmd.pSysMemUP_, size );
			pDstBox_ = 0;

			return true;
		}
		else if (!pDstBox_) // && cmd.pDstBox_
		{
			UINT size = Size();
			if ( !AllocateMemory(pWrapper, size) ) {
				return false;
			}

			// copy by rows
			BYTE* pSlice = SysMemUP_ 
							+ cmd.pDstBox_->top * cmd.RowPitch_
							+ cmd.pDstBox_->front * cmd.DepthPitch_
							+ cmd.pDstBox_->left;
			BYTE* pSrc   = cmd.SysMemUP_;
			for (LONG i = cmd.pDstBox_->front; i<cmd.pDstBox_->back; ++i)
			{
				BYTE* pRow = pSlice;
				for (LONG j = cmd.pDstBox_->top; j<cmd.pDstBox_->bottom; ++j)
				{
					LONG toCopy = cmd.pDstBox_->right - cmd.pDstBox_->left;
					memcpy(pRow, pSrc, toCopy);
					pRow += RowPitch_;
					pSrc += toCopy;
				}
				pSlice += DepthPitch_;
			}

			return true;
		}

		return false;
	}

	void xxUpdateSubresourceUP::SetDestResourceType( TextureType type )
	{
		ResourceWrapper* pRes;
		InitWrapper(hDstResource_, pRes);
		pRes->m_Type = type;
	}

	void xxUpdateSubresourceUP::GetDestResources(DestResourcesSet &res)
	{
		res.insert(hDstResource_);
	}
}