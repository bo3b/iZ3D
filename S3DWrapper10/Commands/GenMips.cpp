#include "stdafx.h"
#include "GenMips.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void GenMips::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnGenMips(pWrapper->hDevice, UNWRAP_CV_SRV_HANDLE(hShaderResourceView_));
		AFTER_EXECUTE(this); 
	}

	bool GenMips::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "GenMips" );
		WriteStreamer::Reference( "hShaderResourceView", hShaderResourceView_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool GenMips::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hShaderResourceView_);
		ReadStreamer::CmdEnd();
		return true;
	}

	void GenMips::BuildCommand(CDumpState *ds)
	{
		output("ctx->GenerateMips(res_view_%d);",ds->GetHandleEventId(hShaderResourceView_));
	}

	bool GenMips::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_SRV_HANDLE(hShaderResourceView_))
			return true;
		return false;
	}

	void GenMips::SetDestResourceType( TextureType type )
	{
		if (hShaderResourceView_.pDrvPrivate)
		{
			ShaderResourceViewWrapper* pSRVWrapper = (ShaderResourceViewWrapper*)hShaderResourceView_.pDrvPrivate;
			if (pSRVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pSRVWrapper->GetResourceWrapper();
				pRes->m_Type = type;
			}
		} 
	}

	void GenMips::GetDestResources( DestResourcesSet &res )
	{
		if (hShaderResourceView_.pDrvPrivate)
		{
			ShaderResourceViewWrapper* pSRVWrapper = (ShaderResourceViewWrapper*)hShaderResourceView_.pDrvPrivate;
			if (pSRVWrapper->IsStereo())
			{
				ResourceWrapper* pRes = pSRVWrapper->GetResourceWrapper();
				res.insert(*pRes);
			}
		} 
	}
}

VOID ( APIENTRY GenMips )( D3D10DDI_HDEVICE  hDevice, D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::GenMips* command = new Commands::GenMips(hShaderResourceView);
	D3D10_GET_WRAPPER()->AddCommand(command);
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnGenMips(D3D10_DEVICE, UNWRAP_CV_SRV_HANDLE(hShaderResourceView));
	}
#endif
}