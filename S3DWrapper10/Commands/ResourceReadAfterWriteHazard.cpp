#include "stdafx.h"
#include "ResourceReadAfterWriteHazard.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceReadAfterWriteHazard::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceReadAfterWriteHazard( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hResource_)
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceReadAfterWriteHazard::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceReadAfterWriteHazard" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceReadAfterWriteHazard::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::CmdEnd();
		return true;
	}

	bool ResourceReadAfterWriteHazard::IsUsedStereoResources( D3DDeviceWrapper* /*pWrapper*/ ) const
	{
		if (IS_STEREO_RESOURCE_HANDLE(hResource_))
			return true;
		return false;
	}

	void ResourceReadAfterWriteHazard::SetDestResourceType( TextureType type )
	{
		ResourceWrapper* pRes = (ResourceWrapper*)hResource_.pDrvPrivate;
		if (pRes && pRes->IsStereo())
		{
			pRes->m_Type = type;
		}
	}

	void ResourceReadAfterWriteHazard::GetDestResources( DestResourcesSet &res )
	{
		ResourceWrapper* pRes = (ResourceWrapper*)hResource_.pDrvPrivate;
		if (pRes && pRes->IsStereo())
		{
			res.insert(*pRes);
		}
	}
}

VOID ( APIENTRY ResourceReadAfterWriteHazard )(
	D3D10DDI_HDEVICE  hDevice, 
	D3D10DDI_HRESOURCE  hResource
	)
{
#ifndef EXECUTE_IMMEDIATELY_G1
	Commands::ResourceReadAfterWriteHazard* command = new Commands::ResourceReadAfterWriteHazard(
		hResource
		);
	D3D10_GET_WRAPPER()->AddCommand( command );
#else
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceReadAfterWriteHazard( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hResource)
			);
	}
#endif
}

