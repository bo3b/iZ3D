#include "stdafx.h"
#include "ClearUnorderedAccessViewFloat11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{

	void ClearUnorderedAccessViewFloat11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnClearUnorderedAccessViewFloat(pWrapper->hDevice, UNWRAP_CV_UAV_HANDLE(hUnorderedAccessView_), pFloats_);
		AFTER_EXECUTE(this); 
	}

	bool ClearUnorderedAccessViewFloat11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ClearUnorderedAccessViewFloat11_0" );
		WriteStreamer::Reference( "hUnorderedAccessView", hUnorderedAccessView_ );
		WriteStreamer::ArrayValues( "pFloats", 4, pFloats_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool  ClearUnorderedAccessViewFloat11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hUnorderedAccessView_ );
		ReadStreamer::ArrayValues( pFloats_, 4 );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ClearUnorderedAccessViewFloat11_0::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_UAV_HANDLE(hUnorderedAccessView_))
			return true;
		return false;
	}

	void ClearUnorderedAccessViewFloat11_0::SetDestResourceType( TextureType type )
	{
		if (hUnorderedAccessView_.pDrvPrivate)
		{
			UnorderedAccessViewWrapper* pUAVWrapper = (UnorderedAccessViewWrapper*)hUnorderedAccessView_.pDrvPrivate;
			if (pUAVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pUAVWrapper->GetResourceWrapper();
				pRes->m_Type = type;
			}
		} 
	}

	void ClearUnorderedAccessViewFloat11_0::GetDestResources( DestResourcesSet &res )
	{
		if (hUnorderedAccessView_.pDrvPrivate)
		{
			UnorderedAccessViewWrapper* pUAVWrapper = (UnorderedAccessViewWrapper*)hUnorderedAccessView_.pDrvPrivate;
			if (pUAVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pUAVWrapper->GetResourceWrapper();
				res.insert(*pRes);
			}
		} 
	}
}

VOID ( APIENTRY ClearUnorderedAccessViewFloat11_0 )( D3D10DDI_HDEVICE  hDevice, D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
													CONST FLOAT  pFloats[4] )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ClearUnorderedAccessViewFloat11_0* command = new Commands::ClearUnorderedAccessViewFloat11_0(hUnorderedAccessView, pFloats);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnClearUnorderedAccessViewFloat(D3D10_DEVICE, UNWRAP_CV_UAV_HANDLE(hUnorderedAccessView), pFloats);
	}
#endif
}