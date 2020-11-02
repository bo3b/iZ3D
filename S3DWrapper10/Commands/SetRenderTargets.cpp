#include "stdafx.h"
#include "SetRenderTargets.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void SetRenderTargets::Execute( D3DDeviceWrapper *pWrapper )
	{
		for (UINT i = 0; i < NumValues_; i++)
			phTempRenderTargetView_[i] = UNWRAP_CV_RTV_HANDLE(Values_[i]);
		
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnSetRenderTargets( pWrapper->hDevice, 
														   phTempRenderTargetView_, 
														   NumValues_, 
														   GetClearCount(NumValues_, pWrapper->m_DeviceState.m_NumRenderTargets), 
														   UNWRAP_CV_DSV_HANDLE(hDepthStencilView_) );
		AFTER_EXECUTE(this); 
	}

	bool SetRenderTargets::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "SetRenderTargets" );
		WriteStreamer::Value( "RTargets", NumValues_ );
		WriteStreamer::ArrayReferences( "hRenderTargetView", NumValues_, Values_ );
		WriteStreamer::Value( "ClearTargets", ClearTargets_ );
		WriteStreamer::Reference( "hDepthStencilView", hDepthStencilView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool SetRenderTargets::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( NumValues_ );
		ReadStreamer::ArrayReferences( Values_, NumValues_ );
		ReadStreamer::Value( ClearTargets_ );
		ReadStreamer::Reference( hDepthStencilView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY SetRenderTargets )( D3D10DDI_HDEVICE  hDevice,  CONST D3D10DDI_HRENDERTARGETVIEW*  phRenderTargetView, 
								   UINT  RTargets, UINT  ClearTargets, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView )
{	
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::SetRenderTargets* command = new Commands::SetRenderTargets( phRenderTargetView, RTargets, ClearTargets, hDepthStencilView );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10DDI_HRENDERTARGETVIEW RTVBuffer[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	for (UINT i = 0; i < RTargets; i++)
		RTVBuffer[i] = UNWRAP_CV_RTV_HANDLE(phRenderTargetView[i]);
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnSetRenderTargets(D3D10_DEVICE, RTVBuffer, RTargets, ClearTargets, UNWRAP_CV_DSV_HANDLE(hDepthStencilView) );
	}
#endif	
}