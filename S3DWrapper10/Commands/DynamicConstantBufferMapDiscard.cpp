#include "stdafx.h"
#include "DynamicConstantBufferMapDiscard.h"
#include "DefaultConstantBufferUpdateSubresourceUP.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"

namespace Commands
{
	DynamicConstantBufferMapDiscard::DynamicConstantBufferMapDiscard()
	{
		CommandId = idDynamicConstantBufferMapDiscard; 
	}

	DynamicConstantBufferMapDiscard::DynamicConstantBufferMapDiscard( D3D10DDI_HRESOURCE hResource, 
		UINT Subresource, 
		D3D10_DDI_MAP DDIMap, 
		UINT  Flags, 
		D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
	{ 
		CommandId = idDynamicConstantBufferMapDiscard; 
		hResource_ = hResource;
		Subresource_ = Subresource;
		DDIMap_ = DDIMap;
		Flags_ = Flags;
		pMappedSubResource_ = pMappedSubResource;
	}
		
	TUpdateSubresourceUpCmdPtr DynamicConstantBufferMapDiscard::CreateUpdateCommand( D3DDeviceWrapper *pWrapper, const xxUpdateSubresourceUP* pOriginalCmd )
	{
		UpdateCommand_ = new DefaultConstantBufferUpdateSubresourceUP( hResource_,
																	   Subresource_,
																	   0,
																	   pOriginalCmd->pSysMemUP_,
																	   pOriginalCmd->RowPitch_,
																	   pOriginalCmd->DepthPitch_ );
		if ( !UpdateCommand_->MakeInternalDataCopy(pWrapper) ) {
			UpdateCommand_.reset();
		}

		return UpdateCommand_;
	}

	void DynamicConstantBufferMapDiscard::Execute( D3DDeviceWrapper *pWrapper )
	{
		BEFORE_EXECUTE(this);
		if (UpdateCommand_) 
		{
			// execute fake map
			pMappedSubResource_->pData = (VOID*)UpdateCommand_->pSysMemUP_;
			pMappedSubResource_->RowPitch = UpdateCommand_->RowPitch_;
			pMappedSubResource_->DepthPitch = UpdateCommand_->DepthPitch_;
		}
		else
		{
			pWrapper->OriginalDeviceFuncs.pfnDynamicConstantBufferMapDiscard( pWrapper->hDevice, 
				GET_RESOURCE_HANDLE(hResource_), Subresource_, DDIMap_, Flags_, pMappedSubResource_ );
		}

		D3D10DDI_HRESOURCE hResource = hResource_;
		ResourceWrapper* pWrp;
		InitWrapper(hResource, pWrp);
		if (pWrp->m_pCB)
			pWrp->m_pCB->Map(pMappedSubResource_->pData);

		AFTER_EXECUTE(this); 
	}

	bool DynamicConstantBufferMapDiscard::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "DynamicConstantBufferMapDiscard" );
		WriteStreamer::Reference( "hResource", hResource_ );
		WriteStreamer::Value( "Subresource", Subresource_ );
		WriteStreamer::Value( "DDIMap", DDIMap_ );
		WriteStreamer::Value( "Flags", Flags_ );
		WriteStreamer::Value( "MappedSubResource", *pMappedSubResource_ );
		WriteStreamer::CmdEnd();
		return true;
	}	

	bool DynamicConstantBufferMapDiscard::ReadFromFile()
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

VOID ( APIENTRY DynamicConstantBufferMapDiscard )( D3D10DDI_HDEVICE hDevice, D3D10DDI_HRESOURCE hResource, UINT Subresource, 
												  D3D10_DDI_MAP DDIMap, UINT  Flags, D3D10DDI_MAPPED_SUBRESOURCE* pMappedSubResource )
{
#if !defined(EXECUTE_IMMEDIATELY_G2) || ENABLE_BUFFERING
	Commands::DynamicConstantBufferMapDiscard* command = new Commands::DynamicConstantBufferMapDiscard( hResource, Subresource, 
		DDIMap, Flags, pMappedSubResource );
	if (!ENABLE_BUFFERING)
		D3D10_GET_WRAPPER()->AddCommand( command, true );
	else
		D3D10_GET_WRAPPER()->WrapCommand(command);
#else
	D3D10_GET_WRAPPER()->ProcessCB();
	{
		THREAD_GUARD_D3D10();
		D3D10_GET_ORIG().pfnDynamicConstantBufferMapDiscard( D3D10_DEVICE, GET_RESOURCE_HANDLE(hResource), Subresource, 
			DDIMap, Flags, pMappedSubResource );
	}
	D3D10DDI_HRESOURCE hRes = hResource;
	ResourceWrapper* pWrp;
	InitWrapper(hRes, pWrp);
	if (pWrp->m_pCB)
	{
		THREAD_GUARD_D3D10();
		pWrp->m_pCB->Map(pMappedSubResource->pData);
	}
#endif
}
