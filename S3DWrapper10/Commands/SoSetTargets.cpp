#include "stdafx.h"
#include "SoSetTargets.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace 
{
	const allocation_desc allocation_distribution[] = 
	{
		{2,   256},
		{3,   64},
		{4,   64},
		{16,  256}
	};
	const size_t num_allocations = sizeof(allocation_distribution) / sizeof(allocation_desc);

} // anonymous namespace

namespace Commands
{
	const D3D10DDI_HRESOURCE					SoSetTargets::NULL_RESOURCE = MAKE_D3D10DDI_HRESOURCE(0);
	SoSetTargets::handle_vector::allocator_type	SoSetTargets::handleAllocator(allocation_distribution, allocation_distribution + num_allocations);
	SoSetTargets::uint_vector::allocator_type	SoSetTargets::uintAllocator(allocation_distribution, allocation_distribution + num_allocations);

	void SoSetTargets::Execute( D3DDeviceWrapper *pWrapper )
	{
		for (UINT i = 0; i < SOTargets_; i++)
			phTempResource_[i] = UNWRAP_CV_RESOURCE_HANDLE(phResource_[i]);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSoSetTargets(pWrapper->hDevice, 
			SOTargets_, GetClearCount(SOTargets_, pWrapper->m_DeviceState.m_NumSOTargets), 
			GetPointerToVector(phTempResource_), GetPointerToVector(pOffsets_));
		AFTER_EXECUTE(this); 
	}

	void SoSetTargets::UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState )
	{
		//SoSetTargets* pCmd = (SoSetTargets*)pState->m_SOTargets.get();
		// asserts when trying to setup zero SO targets
		//_ASSERT(pCmd->SOTargets_ <= SOTargets_ ? ClearTargets_ == 0 : 
		//	ClearTargets_ == (pCmd->SOTargets_ - SOTargets_));
		pState->m_SOTargets = this;
	}

	bool SoSetTargets::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{	
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SoSetTargets" );
		WriteStreamer::Value( "SOTargets", SOTargets_ );
		WriteStreamer::Value( "ClearTargets", ClearTargets_ );
		WriteStreamer::ArrayReferences( "hResource", SOTargets_, phResource_ );
		WriteStreamer::ArrayValues( "Offsets", SOTargets_, pOffsets_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SoSetTargets::ReadFromFile()
	{	
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( SOTargets_);
		ReadStreamer::Value( ClearTargets_);
		ReadStreamer::ArrayReferences( phTempResource_, SOTargets_ );
		ReadStreamer::ArrayValues( pOffsets_, SOTargets_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void SoSetTargets::BuildCommand( CDumpState *ds )
	{
		if (SOTargets_ == 0)
		{
			output("ctx->SOSetTargets(0,0,0);");
			return;
		}
		output(decl,"extern DXBUFFER *SOTargers_%d[%d];",EventID_,SOTargets_);
		output(glob,"DXBUFFER *SOTargers_%d[%d];",EventID_,SOTargets_);
		for(unsigned i = 0;i < SOTargets_;i++)
		{
			if (phTempResource_[i].pDrvPrivate)
			{
				int eid = ds->GetHandleEventId(phTempResource_[i]);
				output(init,"SOTargers_%d[%d] = buffer_%d;",EventID_,i,eid);
			}
		}
		output(decl,"extern UINT offset_%d[%d];",EventID_,SOTargets_);
		output(glob,"UINT offset_%d[%d];",EventID_,SOTargets_);
		for(unsigned i = 0;i < SOTargets_;i++)
		{
			if (pOffsets_[i])
				output(init,"offset_%d[%d] = %u;",EventID_,i,pOffsets_[i]); 
		}
		output("ctx->SOSetTargets(%d,SOTargers_%d,offset_%d);",
			SOTargets_, EventID_, EventID_);
	}
}

VOID ( APIENTRY SoSetTargets )( D3D10DDI_HDEVICE  hDevice, UINT  SOTargets, UINT  ClearTargets, 
							   CONST D3D10DDI_HRESOURCE*  phResource, CONST UINT*  pOffsets )
{	
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SoSetTargets* command = new Commands::SoSetTargets(SOTargets, ClearTargets, phResource, pOffsets);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	Commands::fast_vector<D3D10DDI_HRESOURCE>::type hRESOURCEBuffer(SOTargets);
	for (UINT i = 0; i < SOTargets; i++)
		hRESOURCEBuffer[i] = UNWRAP_CV_RESOURCE_HANDLE(phResource[i]);
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSoSetTargets(D3D10_DEVICE, SOTargets, ClearTargets, GetPointerToVector( hRESOURCEBuffer ), pOffsets);
	}
#endif
}