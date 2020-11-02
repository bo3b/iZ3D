#include "stdafx.h"
#include "OpenResource.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void OpenResource::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnOpenResource(pWrapper->hDevice, &OpenResource_, 
			GET_RESOURCE_HANDLE(hResource_), hRTResource_);
		AFTER_EXECUTE(this); 
	}

	bool OpenResource::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "OpenResource" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Handle( "hRTResource", hRTResource_.handle );
		WriteStreamer::Value( "OpenResource", OpenResource_ );
		WriteStreamer::CmdEnd();

#ifndef FINAL_RELEASE
		/*if (GINFO_DUMP_ON)
		{
			CResource* p = (CResource*)GET_RESOURCE(hResource_); 
			D3D10DDIARG_CREATERESOURCE CreateResource;
			ZeroMemory(&CreateResource, sizeof(CreateResource));
			CreateResource.BindFlags = D3D10_DDI_BIND_SHADER_RESOURCE;
			p->SetArgCreateResource(&CreateResource);		
			p->SaveCreationParameters( pWrapper->m_ResourceManager.pXMLStreamer );
		}*/
#endif
		return true;
	}

	bool OpenResource::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);			
		ReadStreamer::Reference( hResource_);
		ReadStreamer::Handle( hRTResource_);
		ReadStreamer::Value( OpenResource_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void OpenResource::BuildCommand(CDumpState *ds)
	{
		ds->SetUsedAsRenderTarget(hResource_,FALSE);
		output("// unlock: buffer 0x%p",hResource_.pDrvPrivate);
	}

}

VOID ( APIENTRY OpenResource )( D3D10DDI_HDEVICE  hDevice, CONST D3D10DDIARG_OPENRESOURCE*  pOpenResource, 
							   D3D10DDI_HRESOURCE  hResource, D3D10DDI_HRTRESOURCE  hRTResource )
{
	ResourceWrapper* pWrp;
	InitWrapper(hResource, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::OpenResource* command = new Commands::OpenResource(pOpenResource, hResource, hRTResource);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnOpenResource(D3D10_DEVICE, pOpenResource, GET_RESOURCE_HANDLE(hResource), hRTResource);
	}
#endif
	CREATE_RESOURCE_WRAPPER(pOpenResource, hRTResource);
}
