#include "stdafx.h"
#include "RotateResourceIdentities.h"
#include "Present.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"
#include "Presenter.h"

namespace Commands
{

	void RotateResourceIdentities::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnRotateResourceIdentities(&RotateData_);
	}

	bool RotateResourceIdentities::WriteToFile( D3DDeviceWrapper * pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "ResourceUpdateSubresourceUP" );
		WriteStreamer::ValueByRef( "RotateData", pRotateData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool RotateResourceIdentities::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::ValueByRef( pRotateData_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}

}

HRESULT ( APIENTRY  RotateResourceIdentities )  (DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES*  pRotateData)
{
	DXGI_MODIFY_DEVICE(pRotateData);
	HRESULT hResult = S_OK;
	std::vector<DXGI_DDI_HRESOURCE>	vResources;

	if( USE_IZ3DKMSERVICE_PRESENTER /*&& (KM_SHUTTER_SERVICE_BUFFER_COUNT == pRotateData->Resources)*/ )
	{
		vResources.reserve( pRotateData->Resources /*KM_SHUTTER_SERVICE_BUFFER_COUNT*/ );
		for(UINT i = 0; i < pRotateData->Resources; i++)
			vResources.push_back( pRotateData->pResources[i] );
	}

	D3DSwapChain*	pSwapChain	= DXGI_GET_WRAPPER()->GetD3DSwapChain();
	CONST DXGI_DDI_HRESOURCE* pResources = pRotateData->pResources;
	Commands::fast_vector<DXGI_DDI_HRESOURCE>::type	hRESOURCEBuffer(pResources, pResources + pRotateData->Resources);
	pRotateData->pResources = GetPointerToVector( hRESOURCEBuffer );

	if( CPresenterX::Get() && pSwapChain)
	{
		for(UINT i = 0; i < pRotateData->Resources; i++)
			hRESOURCEBuffer[ i ] = GET_RESOURCE_HANDLE(((DXGI_DDI_HRESOURCE *)pResources)[i]);
		if ( pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId() )
		{
			DEBUG_TRACE3(_T("Presenter - RotateResourceIdentities\n"));
			HRESULT hResult;
			{
				THREAD_GUARD_DXGI();
				hResult = DXGI_GET_ORIG().pfnRotateResourceIdentities(pRotateData);
			}
			pRotateData->pResources = pResources;
			DXGI_RESTORE_DEVICE(pRotateData);
			return hResult;
		}
		else
		{
			DEBUG_TRACE3(_T("App - RotateResourceIdentities\n"));
			CPresenterX::Get()->AddResourceIdentities( pRotateData ); 
			DXGI_RESTORE_DEVICE(pRotateData);
			return S_OK;
		}
	}

	DXGI_GET_WRAPPER()->ProcessCB( );

	const unsigned ADDITIONAL_RESOURCES		= KM_SHUTTER_SERVICE_BUFFER_COUNT + 2;
	DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES	RotateData;
	DXGI_DDI_HRESOURCE						hResources[ ADDITIONAL_RESOURCES ];
	if( USE_IZ3DKMSERVICE_PRESENTER && pSwapChain && pSwapChain->m_bPresentationCall && (2 == pRotateData->Resources) )
	{
		// Call yet another RotateResourceIdentities() for additional two buffers:
		// [1|2] ->  [1|2|3|4|5|6] 
		SwapChainResources *pRes = pSwapChain->GetCurrentPrimaryResource();
		
		RotateData.hDevice		= pRotateData->hDevice;
		RotateData.Resources	= ADDITIONAL_RESOURCES;
		RotateData.pResources	= hResources;

		hResources[0]	= GET_RESOURCE_HANDLE( vResources[0] );
		if( false )		// No-copy codepath
			hResources[1]	= Commands::Present::hLastKMSurfaceToPresent ? Commands::Present::hLastKMSurfaceToPresent : GET_RESOURCE_HANDLE( vResources[1] );
		else
			hResources[1]	= GET_RESOURCE_HANDLE( vResources[1] );

		for( UINT i = 0; i < KM_SHUTTER_SERVICE_BUFFER_COUNT; i++ )
		{
			hResources[2 + i] = (DXGI_DDI_HRESOURCE) pRes->GetKMShutterPrimaryResource( i ).pDrvPrivate; 
		}
		pRotateData = &RotateData;							// replace rotate data
		Commands::Present::hLastKMSurfaceToPresent = NULL;	// reset the hLastKMSurfaceToPresent 

		for(UINT i = 0; i < pRotateData->Resources; i++)
			DEBUG_TRACE1(_T("RotateResorceIdentities(): Resource[%d]   = 0x%p\n"), i, hResources[i] );
	}
	else
	{
		for(UINT i = 0; i < pRotateData->Resources; i++)
		{
			hRESOURCEBuffer[ i ] = (DXGI_DDI_HRESOURCE) GET_RESOURCE_HANDLE(((DXGI_DDI_HRESOURCE *)pResources)[i]);
			DEBUG_TRACE3(_T("RotateResorceIdentities(): Resource[%d]   = 0x%p\n"), i, hRESOURCEBuffer[i] );
		}
	}
	DEBUG_TRACE3(_T("\n"));
#ifndef EXECUTE_IMMEDIATELY_G2
	Commands::RotateResourceIdentities* command = new Commands::RotateResourceIdentities( pRotateData );
	BEFORE_EXECUTE(command);
	{
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG().pfnRotateResourceIdentities(pRotateData);
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	{
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG().pfnRotateResourceIdentities(pRotateData);
	}
#endif

	if( USE_IZ3DKMSERVICE_PRESENTER && pSwapChain && pSwapChain->m_bPresentationCall )
	{
		SwapChainResources *pRes = pSwapChain->GetCurrentPrimaryResource();
		pRes->RotateKMShutterMarkerIdentities();

		// N.B. Don't check for !m_Description.Windowed because RotateResourceIdentities() is fullscreen related anyway
		if( KM_SHUTTER_SERVICE_BUFFER_COUNT == pRotateData->Resources )
		{
			// Call yet another RotateResourceIdentities() for additional two buffers:
			// ([1|2|3|4] -> [2|3|4|1]) + ([1|5|6] -> [5|6|1]) == [1|2|3|4|5|6] -> [2|3|4|5|6|1] 

			const unsigned ADDITIONAL_RESOURCES		= 3;
			DXGI_DDI_ARG_ROTATE_RESOURCE_IDENTITIES	RotateData;
			DXGI_DDI_HRESOURCE						hResources[ ADDITIONAL_RESOURCES ];

			RotateData.hDevice		= pRotateData->hDevice;
			RotateData.Resources	= ADDITIONAL_RESOURCES;
			RotateData.pResources	= hResources;

			hResources[0]	= GET_RESOURCE_HANDLE( vResources[KM_SHUTTER_SERVICE_BUFFER_COUNT - 1] );
			hResources[1]	= (DXGI_DDI_HRESOURCE) pRes->GetKMShutterPrimaryResource( 0 ).pDrvPrivate;
			hResources[2]	= (DXGI_DDI_HRESOURCE) pRes->GetKMShutterPrimaryResource( 1 ).pDrvPrivate;

			HRESULT hResult = DXGI_GET_ORIG().pfnRotateResourceIdentities( &RotateData );
		}
	}

	//Commands::Command::CurrentView_ = view;
	pRotateData->pResources = pResources;
	DXGI_RESTORE_DEVICE(pRotateData);
	return hResult;
};