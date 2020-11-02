#include "stdafx.h"
#include "DestroyRenderTargetView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void DestroyRenderTargetView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyRenderTargetView(pWrapper->hDevice, GET_RTV_HANDLE(hRenderTargetView_));
		AFTER_EXECUTE(this); 
	}

	bool DestroyRenderTargetView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyRenderTargetView" );
		WriteStreamer::Reference( "hRenderTargetView", hRenderTargetView_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hRenderTargetView_);
		return true;
	}

	bool DestroyRenderTargetView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hRenderTargetView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY DestroyRenderTargetView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRENDERTARGETVIEW hRenderTargetView )
{
	RenderTargetViewWrapper* pWrp;
	InitWrapper(hRenderTargetView, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyRenderTargetView* command = new Commands::DestroyRenderTargetView( hRenderTargetView );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyRenderTargetView( D3D10_DEVICE, GET_RTV_HANDLE(hRenderTargetView) );
	}
#endif
	DESTROY_RTV_WRAPPER();
}

