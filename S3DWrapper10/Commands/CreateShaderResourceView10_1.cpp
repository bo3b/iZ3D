#include "stdafx.h"
#include "CreateShaderResourceView10_1.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void CreateShaderResourceView10_1::Execute( D3DDeviceWrapper *pWrapper )
	{
		D3D10DDI_HRESOURCE hSource = CreateShaderResourceView_.hDrvResource;
		CreateShaderResourceView_.hDrvResource = UNWRAP_CV_RESOURCE_HANDLE( CreateShaderResourceView_.hDrvResource );

		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs10_1.pfnCreateShaderResourceView(pWrapper->hDevice, 
			&CreateShaderResourceView_, GET_SRV_HANDLE(hShaderResourceView_), hRTShaderResourceView_);
		AFTER_EXECUTE(this);

		CreateShaderResourceView_.hDrvResource = hSource;

		Bd().SetResourceOfView(hShaderResourceView_,CreateShaderResourceView_.hDrvResource);
	}

	bool CreateShaderResourceView10_1::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "CreateShaderResourceView10_1" );
		WriteStreamer::Value( "CreateShaderResourceView", CreateShaderResourceView_ );
		WriteStreamer::Reference( "hShaderResourceView", hShaderResourceView_ );
		WriteStreamer::Handle( "hRTShaderResourceView", hRTShaderResourceView_.handle );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool CreateShaderResourceView10_1::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Value( CreateShaderResourceView_ );
		ReadStreamer::Reference( hShaderResourceView_ );
		ReadStreamer::Handle( hRTShaderResourceView_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	void CreateShaderResourceView10_1::BuildCommand(CDumpState *ds)
	{
		if (!CreateShaderResourceView_.hDrvResource.pDrvPrivate)
		{
			output(decl,"extern ID3D10ShaderResourceView *res_view_%d;",EventID_);
			output(glob,"ID3D10ShaderResourceView *res_view_%d = 0;",EventID_);
			return;
		}

		set_output_target(init);

		int eid = ds->GetHandleEventId(CreateShaderResourceView_.hDrvResource);
		output("vdesc1.Format = %s;",EnumToString(CreateShaderResourceView_.Format));
		output("vdesc1.ViewDimension = %s; // %s",
			ds->ToViewDim1(CreateShaderResourceView_.ResourceDimension,CreateShaderResourceView_.hDrvResource),
			EnumToString(CreateShaderResourceView_.ResourceDimension));
		switch (CreateShaderResourceView_.ResourceDimension)
		{
		case D3D10DDIRESOURCE_TEXTURE2D:
			output("vdesc1.Texture2DArray.MostDetailedMip = %u;",CreateShaderResourceView_.Tex2D.MostDetailedMip);
			output("vdesc1.Texture2DArray.FirstArraySlice = %u;",CreateShaderResourceView_.Tex2D.FirstArraySlice);
			output("vdesc1.Texture2DArray.MipLevels = %u;",CreateShaderResourceView_.Tex2D.MipLevels);
			output("vdesc1.Texture2DArray.ArraySize = %u;",CreateShaderResourceView_.Tex2D.ArraySize);
			break;
		case D3D10DDIRESOURCE_TEXTURECUBE:
			output("vdesc1.TextureCubeArray.First2DArrayFace = %u;",CreateShaderResourceView_.TexCube.First2DArrayFace);
			output("vdesc1.TextureCubeArray.NumCubes = %u;",CreateShaderResourceView_.TexCube.NumCubes);

			output("vdesc1.TextureCubeArray.MostDetailedMip = %u;",CreateShaderResourceView_.TexCube.MostDetailedMip);
			output("vdesc1.TextureCubeArray.MipLevels = %u;",CreateShaderResourceView_.TexCube.MipLevels);
			break;
		case D3D10DDIRESOURCE_TEXTURE3D:
			output("vdesc1.Texture3D.MostDetailedMip = %u;",CreateShaderResourceView_.Tex3D.MostDetailedMip);
			output("vdesc1.Texture3D.MipLevels = %u;",CreateShaderResourceView_.Tex3D.MipLevels);
			break;
		case D3D10DDIRESOURCE_TEXTURE1D:
			output("vdesc1.Texture1DArray.MostDetailedMip = %u;",CreateShaderResourceView_.Tex1D.MostDetailedMip);
			output("vdesc1.Texture1DArray.FirstArraySlice = %u;",CreateShaderResourceView_.Tex1D.FirstArraySlice);
			output("vdesc1.Texture1DArray.MipLevels = %u;",CreateShaderResourceView_.Tex1D.MipLevels);
			output("vdesc1.Texture1DArray.ArraySize = %u;",CreateShaderResourceView_.Tex1D.ArraySize);
			break;
		case D3D10DDIRESOURCE_BUFFER:
			output("vdesc1.Buffer.FirstElement = %u;",CreateShaderResourceView_.Buffer.FirstElement);
			output("vdesc1.Buffer.NumElements = %u;",CreateShaderResourceView_.Buffer.NumElements);
			break;
		default:
			__debugbreak(); // not implemented
		}

		output("ID3D10ShaderResourceView1 *res_view_%da;",EventID_);
		output("V_(device->CreateShaderResourceView1(buffer_%d,&vdesc1,&res_view_%da));",eid,EventID_);

		output(decl,"extern ID3D10ShaderResourceView *res_view_%d;",EventID_);
		output(glob,"ID3D10ShaderResourceView *res_view_%d;",EventID_);
		output("V_(res_view_%da->QueryInterface(__uuidof(ID3D10ShaderResourceView),(LPVOID*)&res_view_%d));",EventID_,EventID_);
		output("res_view_%da->Release();",EventID_);

		ds->AddHandleEventId(hShaderResourceView_,EventID_);
		ds->SetResourceOfView(hShaderResourceView_,CreateShaderResourceView_.hDrvResource);
	}

}

VOID ( APIENTRY CreateShaderResourceView10_1 )( D3D10DDI_HDEVICE  hDevice,
											   CONST D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*  pCreateShaderResourceView, 
											   D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRTSHADERRESOURCEVIEW  hRTShaderResourceView )
{
	_ASSERT(D3D10_GET_WRAPPER()->GetD3DVersion() == TD3DVersion_10_1);
	ShaderResourceViewWrapper* pWrp;
	InitWrapper(hShaderResourceView, pWrp);	
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::CreateShaderResourceView10_1* command = new Commands::CreateShaderResourceView10_1(pCreateShaderResourceView,
		hShaderResourceView, hRTShaderResourceView);
	D3D10_GET_WRAPPER()->AddCommand(command, true );
#else
	D3D10DDI_HRESOURCE hSource = pCreateShaderResourceView->hDrvResource;
	((D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*)pCreateShaderResourceView)->hDrvResource = UNWRAP_CV_RESOURCE_HANDLE( pCreateShaderResourceView->hDrvResource );

	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_1_GET_ORIG().pfnCreateShaderResourceView(D3D10_DEVICE, pCreateShaderResourceView,
			GET_SRV_HANDLE(hShaderResourceView), hRTShaderResourceView);
	}
	((D3D10_1DDIARG_CREATESHADERRESOURCEVIEW*)pCreateShaderResourceView)->hDrvResource = hSource;
#endif
	CREATE_SRV_WRAPPER();
}