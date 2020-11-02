#include "stdafx.h"
#include "StagingResourceMap.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void StagingResourceMap::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		pWrapper->OriginalDeviceFuncs.pfnStagingResourceMap( pWrapper->hDevice,
			UNWRAP_CV_RESOURCE_HANDLE(hResource_),
			Subresource_,
			DDIMap_, 
			Flags_,
			pMappedSubResource_
			);
		AFTER_EXECUTE(this); 
	}

	bool StagingResourceMap::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		D3D10DDI_MAPPED_SUBRESOURCE MappedSubResource_ = *pMappedSubResource_;
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "StagingResourceMap" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::Value( "DDIMap", DDIMap_ );
		WriteStreamer::Value( "Flags", Flags_ );
		WriteStreamer::ValueByRef( "MappedSubResource", pMappedSubResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool StagingResourceMap::ReadFromFile()
	{
		D3D10DDI_MAPPED_SUBRESOURCE MappedSubResource_;
		memset( &MappedSubResource_, 0, sizeof( D3D10DDI_MAPPED_SUBRESOURCE ) );
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference(  hResource_);
		ReadStreamer::Value( Subresource_);
		ReadStreamer::Value( DDIMap_);
		ReadStreamer::Value( Flags_);
		ReadStreamer::ValueByRef( pMappedSubResource_);
		ReadStreamer::CmdEnd();
		return true;
	}

}

VOID ( APIENTRY StagingResourceMap )(
									 D3D10DDI_HDEVICE  hDevice, 
									 D3D10DDI_HRESOURCE  hResource, 
									 UINT  Subresource, 
									 D3D10_DDI_MAP  DDIMap, 
									 UINT  Flags, 
									 D3D10DDI_MAPPED_SUBRESOURCE*  pMappedSubResource
									 )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::StagingResourceMap* command = new Commands::StagingResourceMap(
		hResource, 
		Subresource, 
		DDIMap, 
		Flags, 
		pMappedSubResource
		);
	if (!ENABLE_BUFFERING)
	D3D10_GET_WRAPPER()->AddCommand( command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnStagingResourceMap( D3D10_DEVICE,
			UNWRAP_CV_RESOURCE_HANDLE(hResource), 
			Subresource, 
			DDIMap, 
			Flags, 
			pMappedSubResource
			);
	}
#endif
}
