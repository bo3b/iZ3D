#include "stdafx.h"
#include "CreateDepthStencilView11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "DepthStencilViewWrapper.h"

namespace Commands
{

	void CreateDepthStencilView11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		D3D10DDI_HRESOURCE hSource = CreateDepthStencilView_.hDrvResource;
		CreateDepthStencilView_.hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(CreateDepthStencilView_.hDrvResource);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCreateDepthStencilView(pWrapper->hDevice, 
			&CreateDepthStencilView_, GET_DSV_HANDLE(hDepthStencilView_), hRTDepthStencilView_);
		AFTER_EXECUTE(this); 

		CreateDepthStencilView_.hDrvResource = hSource;
	}

	bool CreateDepthStencilView11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateDepthStencilView11_0" );
		WriteStreamer::Value( "CreateDepthStencilView", CreateDepthStencilView_ );
		WriteStreamer::Reference( "hDepthStencilView", hDepthStencilView_ );
		WriteStreamer::Handle( "hRTDepthStencilView", hRTDepthStencilView_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateDepthStencilView11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateDepthStencilView_ );
		ReadStreamer::Reference( hDepthStencilView_ );
		ReadStreamer::Handle( hRTDepthStencilView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateDepthStencilView11_0::BuildCommand(CDumpState *ds)
	{
		output(glob,"ID3D11DepthStencilView *sview_%d;",EventID_);
		set_output_target(init);

		int eid = ds->GetHandleEventId(CreateDepthStencilView_.hDrvResource);
		D3D10DDIARG_CREATERESOURCE &cres = *ds->GetCreateResourceStruct(CreateDepthStencilView_.hDrvResource);

		output("dsvd11.Format = %s;",EnumToString(CreateDepthStencilView_.Format));
		output("dsvd11.ViewDimension = (D3D11_DSV_DIMENSION)%s; // %s",
			ds->ToViewDimDsv(CreateDepthStencilView_.ResourceDimension,CreateDepthStencilView_.hDrvResource),
			EnumToString(CreateDepthStencilView_.ResourceDimension));
		output("dsvd11.Flags = 0x%x;",CreateDepthStencilView_.Flags);
		switch (CreateDepthStencilView_.ResourceDimension)
		{
		case D3D10DDIRESOURCE_TEXTURE2D:
			if (cres.SampleDesc.Count > 1)
			{
				//output("dsvd11.Texture2DMSArray.MipSlice = %u;",CreateDepthStencilView_.Tex2D.MipSlice);
				output("dsvd11.Texture2DMSArray.FirstArraySlice = %u;",CreateDepthStencilView_.Tex2D.FirstArraySlice);
				output("dsvd11.Texture2DMSArray.ArraySize = %u;",CreateDepthStencilView_.Tex2D.ArraySize);
			}
			else
			{
				output("dsvd11.Texture2DArray.MipSlice = %u;",CreateDepthStencilView_.Tex2D.MipSlice);
				output("dsvd11.Texture2DArray.FirstArraySlice = %u;",CreateDepthStencilView_.Tex2D.FirstArraySlice);
				output("dsvd11.Texture2DArray.ArraySize = %u;",CreateDepthStencilView_.Tex2D.ArraySize);
			}
			break;
		case D3D10DDIRESOURCE_TEXTURECUBE:
			output("dsvd11.Texture2DArray.MipSlice = %u;",CreateDepthStencilView_.TexCube.MipSlice);
			output("dsvd11.Texture2DArray.FirstArraySlice = %u;",CreateDepthStencilView_.TexCube.FirstArraySlice);
			output("dsvd11.Texture2DArray.ArraySize = %u;",CreateDepthStencilView_.TexCube.ArraySize);
			break;
		case D3D10DDIRESOURCE_TEXTURE1D:
			output("dsvd11.Texture1D.MipSlice = %u;",CreateDepthStencilView_.Tex1D.MipSlice);
			output("dsvd11.Texture1D.FirstArraySlice = %u;",CreateDepthStencilView_.Tex1D.FirstArraySlice);
			output("dsvd11.Texture1D.ArraySize = %u;",CreateDepthStencilView_.Tex1D.ArraySize);
			break;
		default:
			output("-- not implemented --");
			__debugbreak();
		}

		output("V_(device->CreateDepthStencilView(buffer_%d,&dsvd11,&sview_%d));",eid,EventID_);

		ds->AddHandleEventId(hDepthStencilView_,EventID_);
	}
}

VOID ( APIENTRY CreateDepthStencilView11_0 )( D3D10DDI_HDEVICE  hDevice, 
										 CONST D3D11DDIARG_CREATEDEPTHSTENCILVIEW*  pCreateDepthStencilView, 
										 D3D10DDI_HDEPTHSTENCILVIEW  hDepthStencilView, D3D10DDI_HRTDEPTHSTENCILVIEW  hRTDepthStencilView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	DepthStencilViewWrapper* pWrp;
	InitWrapper(hDepthStencilView, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateDepthStencilView11_0* command = new Commands::CreateDepthStencilView11_0(pCreateDepthStencilView,
		hDepthStencilView, hRTDepthStencilView);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10DDI_HRESOURCE hSource = pCreateDepthStencilView->hDrvResource;
	((D3D11DDIARG_CREATEDEPTHSTENCILVIEW*)pCreateDepthStencilView)->hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(pCreateDepthStencilView->hDrvResource);

	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCreateDepthStencilView(D3D10_DEVICE, pCreateDepthStencilView,
			GET_DSV_HANDLE(hDepthStencilView), hRTDepthStencilView);
	}
	((D3D11DDIARG_CREATEDEPTHSTENCILVIEW*)pCreateDepthStencilView)->hDrvResource = hSource;
#endif
	CREATE_DSV_WRAPPER();
}