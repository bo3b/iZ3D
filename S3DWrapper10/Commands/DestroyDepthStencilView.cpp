#include "stdafx.h"
#include "DepthStencilViewWrapper.h"
#include "DestroyDepthStencilView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DestroyDepthStencilView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyDepthStencilView(pWrapper->hDevice, GET_DSV_HANDLE(hDepthStencilView_));
		AFTER_EXECUTE(this); 
	}

	bool DestroyDepthStencilView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyDepthStencilView" );
		WriteStreamer::Reference( "hDepthStencilView", hDepthStencilView_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hDepthStencilView_);
		return true;
	}

	bool DestroyDepthStencilView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hDepthStencilView_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyDepthStencilView )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView )
{
	DepthStencilViewWrapper* pWrp;
	InitWrapper(hDepthStencilView, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyDepthStencilView* command = new Commands::DestroyDepthStencilView( hDepthStencilView );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyDepthStencilView( D3D10_DEVICE, GET_DSV_HANDLE(hDepthStencilView) );
	}
#endif
	DESTROY_DSV_WRAPPER();
}

