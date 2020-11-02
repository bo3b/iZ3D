#include "stdafx.h"
#include "ShaderResourceViewReadAfterWriteHazard.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "ShaderResourceViewWrapper.h"

namespace Commands
{

	void ShaderResourceViewReadAfterWriteHazard::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnShaderResourceViewReadAfterWriteHazard(pWrapper->hDevice, 
			UNWRAP_CV_SRV_HANDLE(hShaderResourceView_), UNWRAP_CV_RESOURCE_HANDLE(hResource_));
		AFTER_EXECUTE(this); 
	}

	bool ShaderResourceViewReadAfterWriteHazard::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ShaderResourceViewReadAfterWriteHazard" );
		WriteStreamer::Reference( "hShaderResourceView", hShaderResourceView_ );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ShaderResourceViewReadAfterWriteHazard::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hShaderResourceView_ );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ShaderResourceViewReadAfterWriteHazard::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_SRV_HANDLE(hShaderResourceView_))
			return true;
		return false;
	}

	void ShaderResourceViewReadAfterWriteHazard::SetDestResourceType( TextureType type )
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

	void ShaderResourceViewReadAfterWriteHazard::GetDestResources( DestResourcesSet &res )
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

VOID ( APIENTRY ShaderResourceViewReadAfterWriteHazard )( D3D10DDI_HDEVICE  hDevice, 
														 D3D10DDI_HSHADERRESOURCEVIEW  hShaderResourceView, D3D10DDI_HRESOURCE  hResource )
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ShaderResourceViewReadAfterWriteHazard* command = new Commands::ShaderResourceViewReadAfterWriteHazard(hShaderResourceView,
		hResource);
	//D3D10_GET_WRAPPER()->m_MonoCB.Add(command);
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnShaderResourceViewReadAfterWriteHazard(D3D10_DEVICE, UNWRAP_CV_SRV_HANDLE(hShaderResourceView),
			UNWRAP_CV_RESOURCE_HANDLE(hResource));
	}
#endif
}