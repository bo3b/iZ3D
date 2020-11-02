#include "stdafx.h"
#include "SetScissorRects.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace 
{
	struct allocation { size_t size; size_t count; };

	const allocation allocation_distribution[] = 
	{
		{1,   256},
		{16,  256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation);

} // anonymous namespace

namespace Commands
{
	SetScissorRects::rect_vector::allocator_type SetScissorRects::rectAllocator(allocation_distribution, allocation_distribution + num_allocations);

	void SetScissorRects::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetScissorRects(pWrapper->hDevice, 
			NumScissorRects_, GetClearCount(NumScissorRects_, pWrapper->m_DeviceState.m_NumScissorRects), GetPointerToVector(pRects_));
		AFTER_EXECUTE(this); 
	}

	void SetScissorRects::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		SetScissorRects* pCmd = (SetScissorRects*)pState->m_RSScissorRects.get();
		_ASSERT(pCmd->NumScissorRects_ <= NumScissorRects_ ? ClearScissorRects_ == 0 : 
			ClearScissorRects_ == (pCmd->NumScissorRects_ - NumScissorRects_));
		pState->m_RSScissorRects = this;
	}

	bool SetScissorRects::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetScissorRects" );
		WriteStreamer::Value( "NumScissorRects", NumScissorRects_ );
		WriteStreamer::Value( "ClearScissorRects", ClearScissorRects_ );
		WriteStreamer::ArrayValues( "Rects", NumScissorRects_, pRects_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetScissorRects::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( NumScissorRects_);
		ReadStreamer::Value( ClearScissorRects_);
		ReadStreamer::ArrayValues( pRects_, NumScissorRects_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void SetScissorRects::BuildCommand(CDumpState *ds)
	{
		if (NumScissorRects_ > 0)
		{
			output(glob,"D3D10_RECT rects_%d[%d];",EventID_,NumScissorRects_);
			set_output_target(init);
			for(UINT i = 0;i < NumScissorRects_;i++)
			{
				if (pRects_[i].left) output("rects_%d[%d].left = %d;",EventID_,i,pRects_[i].left);
				if (pRects_[i].top) output("rects_%d[%d].top = %d;",EventID_,i,pRects_[i].top);
				if (pRects_[i].right) output("rects_%d[%d].right = %d;",EventID_,i,pRects_[i].right);
				if (pRects_[i].bottom) output("rects_%d[%d].bottom = %d;",EventID_,i,pRects_[i].bottom);
			}
			output(draw,"ctx->RSSetScissorRects(%d,rects_%d);",NumScissorRects_,EventID_);
		}
		else
		{
			output("ctx->RSSetScissorRects(0,0);");
		}
	}

}

VOID ( APIENTRY SetScissorRects )( D3D10DDI_HDEVICE  hDevice, UINT  NumScissorRects, UINT  ClearScissorRects, 
								  CONST D3D10_DDI_RECT*  pRects )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetScissorRects* command = new Commands::SetScissorRects(NumScissorRects, ClearScissorRects, pRects);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetScissorRects(D3D10_DEVICE, NumScissorRects, ClearScissorRects, pRects);
	}
#endif
}