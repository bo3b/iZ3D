#include "stdafx.h"
#include "DynamicIABufferMapDiscard.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{

	void DynamicIABufferMapDiscard::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		// only mono
		pWrapper->OriginalDeviceFuncs.pfnDynamicIABufferMapDiscard( pWrapper->hDevice, GET_RESOURCE_HANDLE(hResource_), Subresource_,
			DDIMap_, Flags_, pMappedSubResource_ );
		AFTER_EXECUTE(this); 
	}

	bool DynamicIABufferMapDiscard::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DynamicIABufferMapDiscard" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::Value( "DDIMap", DDIMap_ );
		WriteStreamer::Value( "Flags", Flags_ );
		WriteStreamer::Value( "MappedSubResource", *pMappedSubResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}
	
	bool DynamicIABufferMapDiscard::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);
		ReadStreamer::Reference( hResource_);
		ReadStreamer::Value( Subresource_);
		ReadStreamer::Value( DDIMap_);
		ReadStreamer::Value( Flags_);
		ReadStreamer::Skip(); // pMappedSubResource_
		ReadStreamer::CmdEnd();
		return true;
	}
}

VOID ( APIENTRY DynamicIABufferMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource,
											D3D10_DDI_MAP DDIMap, UINT Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::DynamicIABufferMapDiscard* command = new Commands::DynamicIABufferMapDiscard( hResource, Subresource, 
		DDIMap, Flags, pMappedSubResource );
	if (!ENABLE_BUFFERING)
	D3D10_GET_WRAPPER()->AddCommand( command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	// only mono
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDynamicIABufferMapDiscard( D3D10_DEVICE, GET_RESOURCE_HANDLE(hResource), Subresource, DDIMap, Flags, pMappedSubResource );
	}
#endif
}
