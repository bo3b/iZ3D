#include "stdafx.h"
#include "CreateUnorderedAccessView11_0.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "UnorderedAccessViewWrapper.h"

namespace Commands
{

	void CreateUnorderedAccessView11_0::Execute( D3DDeviceWrapper *pWrapper )
	{
		D3D10DDI_HRESOURCE hSource = CreateUnorderedAccessView_.hDrvResource;
		CreateUnorderedAccessView_.hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(CreateUnorderedAccessView_.hDrvResource);

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs11.pfnCreateUnorderedAccessView(pWrapper->hDevice, 
			&CreateUnorderedAccessView_, GET_UAV_HANDLE(hUnorderedAccessView_), hRTUnorderedAccessView_);
		AFTER_EXECUTE(this);

		CreateUnorderedAccessView_.hDrvResource = hSource;
	}

	bool CreateUnorderedAccessView11_0::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateUnorderedAccessView11_0" );
		WriteStreamer::Value( "CreateUnorderedAccessView", CreateUnorderedAccessView_ );
		WriteStreamer::Reference( "hUnorderedAccessView_", hUnorderedAccessView_ );
		WriteStreamer::Handle( "hRTUnorderedAccessView_", hRTUnorderedAccessView_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateUnorderedAccessView11_0::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Value( CreateUnorderedAccessView_);
		ReadStreamer::Reference( hUnorderedAccessView_);
		ReadStreamer::Handle( hRTUnorderedAccessView_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateUnorderedAccessView11_0::BuildCommand(CDumpState *ds)
	{
		set_output_target(init);

		int eid = ds->GetHandleEventId(CreateUnorderedAccessView_.hDrvResource);
		output("uavd11.Format = %s;",EnumToString(CreateUnorderedAccessView_.Format));
		output("uavd11.ViewDimension = %s; // %s",
			ds->ToViewDimUav(CreateUnorderedAccessView_.ResourceDimension,CreateUnorderedAccessView_.hDrvResource),
			EnumToString(CreateUnorderedAccessView_.ResourceDimension));
		switch( CreateUnorderedAccessView_.ResourceDimension )
		{
		case D3D10DDIRESOURCE_BUFFER:
		case D3D11DDIRESOURCE_BUFFEREX:
			output("uavd11.Buffer.FirstElement = %u;",CreateUnorderedAccessView_.Buffer.FirstElement);
			output("uavd11.Buffer.NumElements = %u;",CreateUnorderedAccessView_.Buffer.NumElements);
			output("uavd11.Buffer.Flags  = %u;",CreateUnorderedAccessView_.Buffer.Flags);
			break;
		case D3D10DDIRESOURCE_TEXTURE1D:
			output("uavd11.Texture1DArray.MipSlice = %u;",CreateUnorderedAccessView_.Tex1D.MipSlice);
			output("uavd11.Texture1DArray.FirstArraySlice = %u;",CreateUnorderedAccessView_.Tex1D.FirstArraySlice);
			output("uavd11.Texture1DArray.ArraySize = %u;",CreateUnorderedAccessView_.Tex1D.ArraySize);
			break;
		case D3D10DDIRESOURCE_TEXTURE2D:
			output("uavd11.Texture2DArray.MipSlice = %u;",CreateUnorderedAccessView_.Tex2D.MipSlice);
			output("uavd11.Texture2DArray.FirstArraySlice = %u;",CreateUnorderedAccessView_.Tex2D.FirstArraySlice);
			output("uavd11.Texture2DArray.ArraySize = %u;",CreateUnorderedAccessView_.Tex2D.ArraySize);
			break;
		case D3D10DDIRESOURCE_TEXTURE3D:
			output("uavd11.Texture3D.MipSlice = %u;",CreateUnorderedAccessView_.Tex3D.MipSlice);
			output("uavd11.Texture3D.FirstWSlice = %u;",CreateUnorderedAccessView_.Tex3D.FirstW);
			output("uavd11.Texture3D.WSize = %u;",CreateUnorderedAccessView_.Tex3D.WSize);
			break;
		default:
			__debugbreak(); // not implemented
		}

		output(glob,"ID3D11UnorderedAccessView  *uav_%d;",EventID_);

		output("V_(device->CreateUnorderedAccessView(buffer_%d,&uavd11,&uav_%d));",eid,EventID_);

		ds->AddHandleEventId(hUnorderedAccessView_,EventID_);
		ds->SetResourceOfView(hUnorderedAccessView_,CreateUnorderedAccessView_.hDrvResource);
	}
}

VOID ( APIENTRY CreateUnorderedAccessView11_0 )( D3D10DDI_HDEVICE  hDevice, 
												CONST D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*  pCreateUnorderedAccessView, 
												D3D11DDI_HUNORDEREDACCESSVIEW  hUnorderedAccessView, 
												D3D11DDI_HRTUNORDEREDACCESSVIEW  hRTUnorderedAccessView  )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() >= TD3DVersion_11_0);
	UnorderedAccessViewWrapper* pWrp;
	InitWrapper(hUnorderedAccessView, pWrp);
	CREATE_UAV_WRAPPER();
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateUnorderedAccessView11_0* command = new Commands::CreateUnorderedAccessView11_0(pCreateUnorderedAccessView,
		hUnorderedAccessView, hRTUnorderedAccessView);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10DDI_HRESOURCE hSource = pCreateUnorderedAccessView->hDrvResource;
	((D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*)pCreateUnorderedAccessView)->hDrvResource = UNWRAP_CV_RESOURCE_HANDLE(pCreateUnorderedAccessView->hDrvResource);

	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D11_GET_ORIG().pfnCreateUnorderedAccessView(D3D10_DEVICE, pCreateUnorderedAccessView,
			GET_UAV_HANDLE(hUnorderedAccessView), hRTUnorderedAccessView);
	}
	((D3D11DDIARG_CREATEUNORDEREDACCESSVIEW*)pCreateUnorderedAccessView)->hDrvResource = hSource;
#endif
}