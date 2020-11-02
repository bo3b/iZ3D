#include "stdafx.h"
#include "DefaultConstantBufferUpdateSubresourceUP.h"
#include "DynamicConstantBufferUnmap.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

#include "DynamicConstantBufferMapDiscard.h"

namespace Commands
{

	void DynamicConstantBufferUnmap::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);

		D3D10DDI_HRESOURCE hResource = hResource_;		
		ResourceWrapper* pWrp;
		InitWrapper(hResource, pWrp);

		if (pWrp->m_pCB)
			pWrp->m_pCB->Unmap(pWrapper);

		// do not execute command if we used faked map for bufferization
		if (!pWrp->m_pCurrentMapCmd || !pWrp->m_pCurrentMapCmd->UpdateCommand_)
		{
			pWrapper->OriginalDeviceFuncs.pfnDynamicConstantBufferUnmap( pWrapper->hDevice, 
				GET_RESOURCE_HANDLE( hResource_ ), Subresource_ );
		}

		AFTER_EXECUTE(this); 
	}

	bool DynamicConstantBufferUnmap::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DynamicConstantBufferUnmap" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool DynamicConstantBufferUnmap::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hResource_);
		ReadStreamer::Value( Subresource_);
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DynamicConstantBufferUnmap )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::DynamicConstantBufferUnmap* command = new Commands::DynamicConstantBufferUnmap( hResource, Subresource );
	if (!ENABLE_BUFFERING)
	D3D10_GET_WRAPPER()->AddCommand( command , true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();

	D3D10DDI_HRESOURCE hRes = hResource;
	ResourceWrapper* pWrp;	
	InitWrapper(hRes, pWrp);	
	if (pWrp->m_pCB)		
	{
		//THREAD_GUARD_D3D10();
		pWrp->m_pCB->Unmap(D3D10_GET_WRAPPER());
	}
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDynamicConstantBufferUnmap( D3D10_DEVICE, GET_RESOURCE_HANDLE( hResource ), Subresource );
	}
#endif
}
