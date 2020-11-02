#include "stdafx.h"
#include "DestroyShaderResourceView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void DestroyShaderResourceView::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnDestroyShaderResourceView( pWrapper->hDevice, GET_SRV_HANDLE(hShaderResourceView_) );
		AFTER_EXECUTE(this); 
	}

	bool DestroyShaderResourceView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DestroyShaderResourceView" );
		WriteStreamer::Reference( "hShaderResourceView", hShaderResourceView_ );
		WriteStreamer::CmdEnd();
		DESTROY_RESOURCE(hShaderResourceView_);
		return true;
	}

	bool DestroyShaderResourceView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShaderResourceView_ );
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DestroyShaderResourceView )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HSHADERRESOURCEVIEW hShaderResourceView )
{
	ShaderResourceViewWrapper* pWrp;
	InitWrapper(hShaderResourceView, pWrp);	
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::DestroyShaderResourceView* command = new Commands::DestroyShaderResourceView( hShaderResourceView );
	D3D10_GET_WRAPPER()->AddCommand( command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDestroyShaderResourceView( D3D10_DEVICE, GET_SRV_HANDLE(hShaderResourceView) );
	}
#endif
	DESTROY_SRV_WRAPPER();
}