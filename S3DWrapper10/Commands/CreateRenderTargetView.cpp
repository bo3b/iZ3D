#include "stdafx.h"
#include "CreateRenderTargetView.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "RenderTargetViewWrapper.h"

namespace Commands
{

	void CreateRenderTargetView::Execute( D3DDeviceWrapper *pWrapper )
	{
		D3D10DDI_HRESOURCE hSource = CreateRenderTargetView_.hDrvResource;
		CreateRenderTargetView_.hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(CreateRenderTargetView_.hDrvResource);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnCreateRenderTargetView(pWrapper->hDevice, 
			&CreateRenderTargetView_, GET_RTV_HANDLE(hRenderTargetView_), hRTRenderTargetView_);
		AFTER_EXECUTE(this);

		CreateRenderTargetView_.hDrvResource = hSource;
	}

	bool CreateRenderTargetView::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateRenderTargetView" );
		WriteStreamer::Value( "CreateRenderTargetView", CreateRenderTargetView_ );
		WriteStreamer::Reference( "hRenderTargetView", hRenderTargetView_ );
		WriteStreamer::Handle( "hRTRenderTargetView", hRTRenderTargetView_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateRenderTargetView::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateRenderTargetView_ );
		ReadStreamer::Reference( hRenderTargetView_ );
		ReadStreamer::Handle( hRTRenderTargetView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateRenderTargetView::BuildCommand(CDumpState *ds)
	{
		int eid = ds->GetHandleEventId(CreateRenderTargetView_.hDrvResource);
		if (eid == ds->GetMainTextureEventId())
		{
			output("static DXRTVIEW *rtview_%d = renderTargetView; // hResource = 0x%08x",EventID_,CreateRenderTargetView_.hDrvResource);
			ds->SetMainRTViewEventId(EventID_);
			output(ds->GetClearAllString().c_str());
		}
		else
		{
			set_output_target(init);

			output("rtvd.Format = %s;",EnumToString(CreateRenderTargetView_.Format));
			output("rtvd.ViewDimension = (DXRTVDIM)%s; // %s",
				ds->ToViewDimRtv(CreateRenderTargetView_.ResourceDimension,CreateRenderTargetView_.hDrvResource),
				EnumToString(CreateRenderTargetView_.ResourceDimension));
			switch (CreateRenderTargetView_.ResourceDimension)
			{
			case D3D10DDIRESOURCE_TEXTURE2D:
				output("rtvd.Texture2DArray.MipSlice = %u;",CreateRenderTargetView_.Tex2D.MipSlice);
				output("rtvd.Texture2DArray.FirstArraySlice = %u;",CreateRenderTargetView_.Tex2D.FirstArraySlice);
				output("rtvd.Texture2DArray.ArraySize = %u;",CreateRenderTargetView_.Tex2D.ArraySize);
				break;
			case D3D10DDIRESOURCE_TEXTURECUBE:
				output("rtvd.Texture2DArray.MipSlice = %u;",CreateRenderTargetView_.TexCube.MipSlice);
				output("rtvd.Texture2DArray.FirstArraySlice = %u;",CreateRenderTargetView_.TexCube.FirstArraySlice);
				output("rtvd.Texture2DArray.ArraySize = %u;",CreateRenderTargetView_.TexCube.ArraySize);
				break;
			case D3D10DDIRESOURCE_TEXTURE3D:
				output("rtvd.Texture3D.MipSlice = %u;",CreateRenderTargetView_.Tex3D.MipSlice);
				output("rtvd.Texture3D.FirstWSlice = %u;",CreateRenderTargetView_.Tex3D.FirstW);
				output("rtvd.Texture3D.WSize = %u;",CreateRenderTargetView_.Tex3D.WSize);
				break;
			case D3D10DDIRESOURCE_TEXTURE1D:
				output("rtvd.Texture1DArray.MipSlice = %u;",CreateRenderTargetView_.Tex1D.MipSlice);
				output("rtvd.Texture1DArray.FirstArraySlice = %u;",CreateRenderTargetView_.Tex1D.FirstArraySlice);
				output("rtvd.Texture1DArray.ArraySize = %u;",CreateRenderTargetView_.Tex1D.ArraySize);
				break;
			case D3D10DDIRESOURCE_BUFFER:
				output("rtvd.Buffer.FirstElement = %u;",CreateRenderTargetView_.Buffer.FirstElement);
				output("rtvd.Buffer.NumElements = %u;",CreateRenderTargetView_.Buffer.NumElements);
				break;
			default:
				__debugbreak(); // not implemented
			}

			output(glob,"DXRTVIEW *rtview_%d;",EventID_);
			output("V_(device->CreateRenderTargetView(buffer_%d,&rtvd,&rtview_%d));",eid,EventID_);
		}

		ds->SetResourceOfView(hRenderTargetView_,CreateRenderTargetView_.hDrvResource);
		ds->AddHandleEventId(hRenderTargetView_,EventID_);
	}
}

VOID ( APIENTRY CreateRenderTargetView )( D3D10DDI_HDEVICE  hDevice, 
										 CONST D3D10DDIARG_CREATERENDERTARGETVIEW*  pCreateRenderTargetView, 
										 D3D10DDI_HRENDERTARGETVIEW  hRenderTargetView, D3D10DDI_HRTRENDERTARGETVIEW  hRTRenderTargetView )
{
	RenderTargetViewWrapper* pWrp;
	InitWrapper(hRenderTargetView, pWrp);
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateRenderTargetView* command = new Commands::CreateRenderTargetView(pCreateRenderTargetView,
		hRenderTargetView, hRTRenderTargetView);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10DDI_HRESOURCE hSource = pCreateRenderTargetView->hDrvResource;
	((D3D10DDIARG_CREATERENDERTARGETVIEW*)pCreateRenderTargetView)->hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(pCreateRenderTargetView->hDrvResource);

	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnCreateRenderTargetView(D3D10_DEVICE, pCreateRenderTargetView,
			GET_RTV_HANDLE(hRenderTargetView), hRTRenderTargetView);
	}
	((D3D10DDIARG_CREATERENDERTARGETVIEW*)pCreateRenderTargetView)->hDrvResource = hSource;
#endif
	CREATE_RTV_WRAPPER();
}