#include "stdafx.h"
#include "Flush.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "Presenter.h"
#include "D3DSwapChain.h"

namespace Commands
{

	void Flush::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnFlush(pWrapper->hDevice);
		pWrapper->m_nFlushCounter++;
		AFTER_EXECUTE(this); 
	}

	bool Flush::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "Flush" );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool Flush::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY Flush )( D3D10DDI_HDEVICE  hDevice )
{
	D3DSwapChain*	pSwapChain	= D3D10_GET_WRAPPER()->GetD3DSwapChain();

	if( CPresenterX::Get() && pSwapChain)
	{
		if ( pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId() )
		{
			DEBUG_TRACE3(_T("Presenter - Flush\n"));
			THREAD_GUARD_D3D10();
			D3D10_GET_ORIG().pfnFlush(D3D10_DEVICE);
			D3D10_GET_WRAPPER()->m_nFlushCounter++;
			return;
		}
	}
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::Flush* command = new Commands::Flush();
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnFlush(D3D10_DEVICE);
		D3D10_GET_WRAPPER()->m_nFlushCounter++;
	}
#endif
}