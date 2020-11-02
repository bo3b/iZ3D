#include "stdafx.h"
#include "CreateResource.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"

namespace Commands
{

	void CreateResource::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateResource(pWrapper->hDevice,
			&CreateResource10_, GET_RESOURCE_HANDLE(hResource_), hRTResource_);
		AFTER_EXECUTE(this);
	}	

	bool CreateResource::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateResource" );
		WriteStreamer::Value( "CreateResource", CreateResource10_ );

		if ( GINFO_DUMP_ON )
			OnWrite();

		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Handle( "hRTResource", hRTResource_.handle );
		WriteStreamer::CmdEnd();

#ifndef FINAL_RELEASE
		/*if (GINFO_DUMP_ON)
		{
			CResource* p = (CResource*)GET_RESOURCE(hResource_); 
			p->SetArgCreateResource(&CreateResource10_);		
			p->SaveCreationParameters( pWrapper->m_ResourceManager.pXMLStreamer );

			if( gInfo.DumpInitialData && 0 ) // resource dump crashes
				pWrapper->m_ResourceManager.SaveInitialResourceData(hResource_, pWrapper, EventID_ );
		}*/
#endif
		return true;
	}

	bool CreateResource::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( CreateResource10_);

		OnRead();

		ReadStreamer::Reference(  hResource_);
		ReadStreamer::Handle( hRTResource_);
		ReadStreamer::CmdEnd();		

		return true;
	}	
}

VOID ( APIENTRY CreateResource )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_CREATERESOURCE*  pCreateResource,
								 D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_0 ||
		D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);

	// DEBUG ONLY!
// 	if( pCreateResource->pPrimaryDesc || (pCreateResource->BindFlags & D3D10_DDI_BIND_PRESENT) )
// 	{
// 		// It's a primary, don't wrap it - use it naked!
// 		THREAD_GUARD_D3D10();
// 		D3D10_GET_ORIG().pfnCreateResource(D3D10_DEVICE, pCreateResource, hResource, hRTResource);
// 	}

	ResourceWrapper* pWrp;
	InitWrapper(hResource, pWrp);
	CREATE_RESOURCE_WRAPPER(pCreateResource, hRTResource);

#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::CreateResource* command = new Commands::CreateResource(pCreateResource, hResource, hRTResource);
	if (!ENABLE_BUFFERING)
		D3D10_GET_WRAPPER()->AddCommand(command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else	
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateResource(D3D10_DEVICE, pCreateResource, GET_RESOURCE_HANDLE(hResource), hRTResource);
	}
#endif
	pWrp->Init(D3D10_GET_WRAPPER());
}

