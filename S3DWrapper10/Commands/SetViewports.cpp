#include "stdafx.h"
#include "SetViewports.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{1,   256},
		{16,  256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	SetViewports::viewport_vector::allocator_type SetViewports::viewportAllocator(allocation_distribution, allocation_distribution + num_allocations);

	void SetViewports::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		D3D10_DDI_VIEWPORT* pVP = GetPointerToVector(pViewports_);
		UINT cc = GetClearCount(NumViewports_, pWrapper->m_DeviceState.m_NumViewports);
		pWrapper->OriginalDeviceFuncs.pfnSetViewports(pWrapper->hDevice, 
			NumViewports_, cc, pVP);
		AFTER_EXECUTE(this); 
	}

	void SetViewports::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		SetViewports* pCmd = (SetViewports*)pState->m_RSViewports.get();
		DBG_UNREFERENCED_LOCAL_VARIABLE(pCmd);
		// asserts when trying to setup zero viewports
		//_ASSERT(pCmd->NumViewports_ <= NumViewports_ ? ClearViewports_ == 0 : 
		//	ClearViewports_ == (pCmd->NumViewports_ - NumViewports_));
		pState->m_RSViewports = this;
	}

	bool SetViewports::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetViewports" );
		WriteStreamer::Value( "NumViewports", NumViewports_ );
		WriteStreamer::Value( "ClearViewports", ClearViewports_ );
		WriteStreamer::ArrayValues( "Viewports", NumViewports_, pViewports_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetViewports::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( NumViewports_);
		ReadStreamer::Value( ClearViewports_);
		ReadStreamer::ArrayValues( pViewports_, NumViewports_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void SetViewports::BuildCommand(CDumpState *ds)
	{
		if (NumViewports_ > 0)
		{
			output(glob,"DXVIEWPORT vp_%d[%d];",EventID_,NumViewports_);
			for(unsigned i = 0;i < NumViewports_;i++)
			{
				D3D10_DDI_VIEWPORT &vp = pViewports_[i];

				if (vp.Width) output(init,"vp_%d[%d].Width = %u;",EventID_,i,(UINT)vp.Width);
				if (vp.Height) output(init,"vp_%d[%d].Height = %u;",EventID_,i,(UINT)vp.Height);
				if (vp.MinDepth) output(init,"vp_%d[%d].MinDepth = %ff;",EventID_,i,vp.MinDepth);
				if (vp.MaxDepth) output(init,"vp_%d[%d].MaxDepth = %ff;",EventID_,i,vp.MaxDepth);
				if (vp.TopLeftX) output(init,"vp_%d[%d].TopLeftX = %d;",EventID_,i,(INT)vp.TopLeftX);
				if (vp.TopLeftY) output(init,"vp_%d[%d].TopLeftY = %d;",EventID_,i,(INT)vp.TopLeftY);
			}
			output("ctx->RSSetViewports(%d,vp_%d);",NumViewports_,EventID_);
		}
		else
		{
			output("ctx->RSSetViewports(%d,0);",NumViewports_);
		}
	}

}

VOID ( APIENTRY SetViewports )( D3D10DDI_HDEVICE  hDevice, UINT  NumViewports, UINT  ClearViewports, 
							   CONST D3D10_DDI_VIEWPORT*  pViewports )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetViewports* command = new Commands::SetViewports(NumViewports, ClearViewports, pViewports);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetViewports(D3D10_DEVICE, NumViewports, ClearViewports, pViewports);
	}
#endif
}