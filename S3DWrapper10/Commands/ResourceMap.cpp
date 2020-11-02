#include "stdafx.h"
#include "ResourceMap.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void ResourceMap::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnResourceMap( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hResource_), 
			Subresource_,
			DDIMap_, 
			Flags_,
			pMappedSubResource_		// FIXME!		
			);
		AFTER_EXECUTE(this); 
	}

	bool ResourceMap::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "VsSetShaderResources" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::Value( "DDIMap", DDIMap_ ); 
		WriteStreamer::Value( "Flags", Flags_ ); 
		WriteStreamer::ValueByRef( "pMappedSubResource", pMappedSubResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool ResourceMap::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::Reference( hResource_ );
		ReadStreamer::Value( Subresource_ );
		ReadStreamer::Value( DDIMap_ ); 
		ReadStreamer::Value( Flags_ ); 
		ReadStreamer::Skip(); // pMappedSubResource_
		ReadStreamer::CmdEnd();
		return true;
	}
}


VOID ( APIENTRY ResourceMap )(
							  D3D10DDI_HDEVICE  hDevice, 
							  D3D10DDI_HRESOURCE  hResource, 
							  UINT  Subresource, 
							  D3D10_DDI_MAP  DDIMap, 
							  UINT  Flags, 
							  D3D10DDI_MAPPED_SUBRESOURCE*  pMappedSubResource
							  )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::ResourceMap* command = new Commands::ResourceMap(
		hResource, 
		Subresource, 
		DDIMap, 
		Flags, 
		pMappedSubResource
		);
	if (!ENABLE_BUFFERING)
		D3D10_GET_WRAPPER()->AddCommand(command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnResourceMap( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hResource), 
			Subresource, 
			DDIMap, 
			Flags, 
			pMappedSubResource
			);
	}
#endif
}

