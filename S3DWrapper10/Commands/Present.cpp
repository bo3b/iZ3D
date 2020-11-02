#include "stdafx.h"
#include "Present.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "D3DSwapChain.h"
#include "Presenter.h"
#include "..\DXGISwapChainWrapper.h"
#include "..\presenter.h"

#include "..\OutputMethods\OutputLib\OutputMethod_dx10.h"
#include "GlobalData.h"

namespace Commands
{

	void Present::Execute( D3DDeviceWrapper *pWrapper )
	{
		//pWrapper->OriginalDXGIDDIBaseFunctions.pfnPresent(&PresentData_);
	}

	bool Present::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "Present" );
		WriteStreamer::ValueByRef( "PresentData", pPresentData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool Present::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL }; 
		ReadStreamer::CmdBegin( hDevice);				
		ReadStreamer::ValueByRef( pPresentData_);
		ReadStreamer::Value( RetValue_);
		ReadStreamer::CmdEnd();
		return true;
	}

	DXGI_DDI_HRESOURCE Present::hRealSurfaceToPresent   = NULL;
	DXGI_DDI_HRESOURCE Present::hLastKMSurfaceToPresent = NULL;
}

HRESULT ( APIENTRY  Present )  (DXGI_DDI_ARG_PRESENT* pPresentData)
{
	DXGI_MODIFY_DEVICE(pPresentData);
	HRESULT				hResult		= S_OK;
	D3DSwapChain*		pSwapChain	= DXGI_GET_WRAPPER()->GetD3DSwapChain();
	SwapChainResources*	pRes		= NULL;
	_ASSERT( pSwapChain );

	DXGI_DDI_HRESOURCE hDstResource			= pPresentData->hDstResource;
	DXGI_DDI_HRESOURCE hSurfaceToPresent	= pPresentData->hSurfaceToPresent;
	pPresentData->hDstResource		= (DXGI_DDI_HRESOURCE) GET_RESOURCE_HANDLE( hDstResource );
	ResourceWrapper* pBackBufferWrp = (ResourceWrapper*)hSurfaceToPresent; 

#ifndef EXECUTE_IMMEDIATELY_G2	
	if (!IsInternalCall())
	{
		pRes = pSwapChain->CreateOrFindBackBufferResources(pPresentData->hSurfaceToPresent, Commands::Present::hRealSurfaceToPresent);
		Commands::Present::hRealSurfaceToPresent = NULL;
	}
	else
	{
		pRes = pSwapChain->GetCurrentPrimaryResource();
		_ASSERT(pRes);
		if (pRes == NULL)
		{
			DEBUG_MESSAGE(_T("WARNING: currentPrimaryResource == NULL in Present()\n"));
			return S_OK;
		}
	}

	if( CPresenterX::Get() && pSwapChain && 
		pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId())
	{
		THREAD_GUARD_DXGI();
		DEBUG_TRACE3(_T("Presenter - Present\n"));
		//DEBUG_MESSAGE(_T("Presenter - Present Src %p Dst %p\n"), 
		//	pPresentData->hSurfaceToPresent, pPresentData->hDstResource);
		DXGI_DDI_HRESOURCE hDstResource			= pPresentData->hDstResource;
		DXGI_DDI_HRESOURCE hSurfaceToPresent	= pPresentData->hSurfaceToPresent;
		pPresentData->hDstResource		= (DXGI_DDI_HRESOURCE) GET_RESOURCE_HANDLE( hDstResource );
		pPresentData->hSurfaceToPresent	= (DXGI_DDI_HRESOURCE) GET_RESOURCE_HANDLE( hSurfaceToPresent );

		ResourceWrapper* pSurfaceToPresentWrp = (ResourceWrapper*)hSurfaceToPresent;
		DBG_UNREFERENCED_LOCAL_VARIABLE(pSurfaceToPresentWrp);
		// for mode without ResourceCopyRegion
		if (!pSwapChain->m_bResourceCopyRegionCalled)
		{
			UINT frame = CPresenterX::Get()->GetFrameCount();
			bool isLeft = !( frame & 0x0001 );
			SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
			pPresentData->hSurfaceToPresent	= (UINT_PTR)pRes->GetPresenterResource(isLeft).pDrvPrivate;
			//DEBUG_MESSAGE(_T("Presenter2 - Present Src %p Dst %p\n"), 
			//	pPresentData->hSurfaceToPresent, hDstResource);
			if (pPresentData->hSurfaceToPresent == NULL)
			{
				DEBUG_MESSAGE(_T("WARNING: m_hPresenterResourceLeft == NULL, probably presenter bug\n"));
				return S_OK;
			}

#ifndef FINAL_RELEASE
			if( DXGI_GET_WRAPPER()->m_DumpType >= dtOnlyRT )
			{
				ResourceWrapper* pSrcWrapper;
				InitWrapper(pRes->GetPresenterResourceWrap(), pSrcWrapper);
				TCHAR szFileName[MAX_PATH];
				_stprintf_s(szFileName, L"%s\\Frame.%.4d.%s.Present.Src.RT", 
					DXGI_GET_WRAPPER()->m_DumpDirectory, frame, isLeft ? _T("L") : _T("R")); 
				pSrcWrapper->DumpToFile(DXGI_GET_WRAPPER(), szFileName, NULL, true, true, isLeft ? dvLeft : dvRight);
			}
#endif
		}
		else
		{
			//DEBUG_MESSAGE(_T("Presenter2 - Present Src %p Dst %p\n"), 
			//	hSurfaceToPresent, hDstResource);
		}

		HRESULT hResult;
		NSCALL(DXGI_GET_ORIG().pfnPresent(pPresentData));
#ifndef FINAL_RELEASE
		if (FAILED(hResult))
			__debugbreak();
#endif
		DXGI_RESTORE_DEVICE(pPresentData);
		return hResult;
	}

	//DEBUG_MESSAGE(_T("App - Present Src %p Dst %p\n"), 
	//	pPresentData->hSurfaceToPresent, pPresentData->hDstResource);

	DXGI_GET_WRAPPER()->ProcessCB();

	if ( CPresenterX::Get() && USE_UM_PRESENTER )
	{
		CPresenterX::Get()->InitResources( );
		//--- warning!  pPresentData.pDXGIContext  is thread dependent variable! Presenter thread != application render thread ---
		CPresenterX::Get()->SetPresentData( pPresentData );
		CPresenterX::Get()->AddBackBuffer( pPresentData->hDstResource, pPresentData->DstSubResourceIndex );
	}

	D3D10DDI_HRESOURCE hResource = MAKE_D3D10DDI_HRESOURCE((void*)pPresentData->hSurfaceToPresent);
	if (!IsInternalCall())
	{
		DXGI_GET_WRAPPER()->DoPresent();
	}
	else
	{
		DXGI_GET_WRAPPER()->UpdateSecondSwapChain(hResource);
	}

	if ( CPresenterX:: Get () && USE_UM_PRESENTER )
	{
		if ( CPresenterX::Get()->GetAction() == ptNone )
			CPresenterX::Get()->SetAction( ptRender );
	}

	if( CPresenterX::Get() && !DXGI_GET_WRAPPER()->SkipPresenterThread() && pSwapChain &&  
		!(pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId()))
	{
		DEBUG_TRACE3(_T("App - Present\n"));
		DXGI_RESTORE_DEVICE(pPresentData);

		//DEBUG_MESSAGE(_T("App2 - Present Src %p Dst %p\n"), 
		//	pPresentData->hSurfaceToPresent, pPresentData->hDstResource);
		return S_OK;
	}

	if (!IsInternalCall())
	{

		if( !CPresenterX::Get() || DXGI_GET_WRAPPER()->SkipPresenterThread() )
		{
			if( USE_IZ3DKMSERVICE_PRESENTER )		// don't replace the incoming hSurfaceToPresent, just unwrap it and update it! 
			{
				static bool bLeftEyeWasShown	= false; DBG_UNREFERENCED_LOCAL_VARIABLE(bLeftEyeWasShown);
				static bool bRightEyeWasShown	= false; DBG_UNREFERENCED_LOCAL_VARIABLE(bRightEyeWasShown);
				bool		bLeftEyeActive		= pRes->IsStereoActive() ? pSwapChain->m_bLeftEyeActive : true;
#if 1
				pPresentData->hSurfaceToPresent = (DXGI_DDI_HRESOURCE) pBackBufferWrp->GetHandle().pDrvPrivate;		// unwrap the handle

				D3D10DDI_HRESOURCE hPresenterResource	= pSwapChain->GetPresenerBackBuffer( bLeftEyeActive );
				D3D10DDI_HRESOURCE hDstPrimaryRes		= {};
				hDstPrimaryRes.pDrvPrivate				= (void *)pPresentData->hSurfaceToPresent;
				
				// TODO: change to TRACE3 level
				DEBUG_TRACE1(_T("Present(): pRes                            = 0x%p\n"), pRes );
				DEBUG_TRACE1(_T("Present(): pPresentData->hSurfaceToPresent = 0x%p\n"), pPresentData->hSurfaceToPresent );
				DEBUG_TRACE1(_T("Present(): hPresenterResource              = 0x%p\n"), hPresenterResource	);

				if( false )		// No-copy codepath
				{
					// don't copy the content, just use the presenter resource as the offscreen primary!
					Commands::Present::hLastKMSurfaceToPresent = pPresentData->hSurfaceToPresent = (DXGI_DDI_HRESOURCE) hPresenterResource.pDrvPrivate;
				}
				else
				{
					// update hSurfaceToPresent with hPresenterResource data
					DXGI_GET_WRAPPER()->OriginalDeviceFuncs.pfnResourceCopyRegion(
						DXGI_GET_WRAPPER()->hDevice,
						hDstPrimaryRes,
						pPresentData->SrcSubResourceIndex,
						0, 0, 0,					// x, y, z
						hPresenterResource,
						0,
						NULL
					);
				}

#ifndef FINAL_RELEASE
				// update hSurfaceToPresent with hKMShutterMarkerResource data
				D3D10DDI_HRESOURCE hMarkerResource = pRes->GetKMShutterMarkerResource();
				DXGI_GET_WRAPPER()->OriginalDeviceFuncs.pfnResourceCopyRegion(
					DXGI_GET_WRAPPER()->hDevice,
					hDstPrimaryRes,
					pPresentData->SrcSubResourceIndex,
					0, 1, 0,					// x, y, z
					hMarkerResource,
					0,
					NULL
				);
#endif
#else
				// just update hSurfaceToPresent with PresenterResourceLeft/Right
				//pPresentData->hSurfaceToPresent	= (DXGI_DDI_HRESOURCE)pRes->GetPresenterResource(pSwapChain->m_bLeftEyeActive).pDrvPrivate;
				DEBUG_TRACE1(_T("Present(): pPresentData->hSurfaceToPresent = 0x%p\n"), pPresentData->hSurfaceToPresent );
				DEBUG_TRACE1(_T("Present(): PresenerBackBufferLeft          = 0x%p\n"), pSwapChain->GetPresenerBackBuffer( true ) );
				DEBUG_TRACE1(_T("Present(): PresenerBackBufferRight         = 0x%p\n"), pSwapChain->GetPresenerBackBuffer( false ) );
				pPresentData->hSurfaceToPresent	= (DXGI_DDI_HRESOURCE) pRes->GetPresenterResource(bLeftEyeActive).pDrvPrivate;
				DEBUG_TRACE1(_T("Present(): *updated*     hSurfaceToPresent = 0x%p\n"), pPresentData->hSurfaceToPresent );

				if( bLeftEyeActive )
					bLeftEyeWasShown	= true;
				else
					bRightEyeWasShown	= true;
	
				// check that code!
				if( bLeftEyeWasShown && bRightEyeWasShown )
				{
					pRes->SwapKMPresenterResources();
					bLeftEyeWasShown = bRightEyeWasShown = false;
				}
#endif

			}
			else
				pPresentData->hSurfaceToPresent = (DXGI_DDI_HRESOURCE)pRes->GetBackBufferPrimary().pDrvPrivate;

		}
		else
			pPresentData->hSurfaceToPresent	= (DXGI_DDI_HRESOURCE)pRes->GetPresenterResource(true).pDrvPrivate;
	}
	else
	{
		pPresentData->hSurfaceToPresent	= (DXGI_DDI_HRESOURCE)pBackBufferWrp->GetHandle().pDrvPrivate;
	}
	
//	HRESULT hResult = S_OK;
	Commands::Present* command = new Commands::Present( pPresentData );
	BEFORE_EXECUTE(command);	
	{
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG().pfnPresent(pPresentData);

		if ( IS_SHUTTER_OUTPUT && !CPresenterX::Get() && !pSwapChain->m_Description.Windowed && !USE_IZ3DKMSERVICE_PRESENTER )
		{			
			DXGI_GET_WRAPPER()->m_pOutputMethod->AfterPresent( DXGI_GET_WRAPPER(), pSwapChain->m_bLeftEyeActive );
			pSwapChain->m_bLeftEyeActive = false;

			DXGI_GET_WRAPPER()->DoPresent();
			hResult = DXGI_GET_ORIG().pfnPresent(pPresentData);

			DXGI_GET_WRAPPER()->m_pOutputMethod->AfterPresent( DXGI_GET_WRAPPER(), pSwapChain->m_bLeftEyeActive );
			pSwapChain->m_bLeftEyeActive = true;
		}		
	}
	command->RetValue_ = hResult;
	AFTER_EXECUTE(command); 

	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	{
		pPresentData->hSurfaceToPresent = (DXGI_DDI_HRESOURCE)pBackBufferWrp->GetHandle().pDrvPrivate;
		THREAD_GUARD_DXGI();
		hResult	= DXGI_GET_ORIG().pfnPresent(pPresentData);

		if ( IS_SHUTTER_OUTPUT && !CPresenterX::Get() && !pSwapChain->m_Description.Windowed && !USE_IZ3DKMSERVICE_PRESENTER )
		{
			DXGI_GET_WRAPPER()->m_pOutputMethod->AfterPresent( DXGI_GET_WRAPPER(), pSwapChain->m_bLeftEyeActive );
			pSwapChain->m_bLeftEyeActive = false;

			DXGI_GET_WRAPPER()->DoPresent();
			hResult = DXGI_GET_ORIG().pfnPresent(pPresentData);

			DXGI_GET_WRAPPER()->m_pOutputMethod->AfterPresent( DXGI_GET_WRAPPER(), pSwapChain->m_bLeftEyeActive );
			pSwapChain->m_bLeftEyeActive = true;
		}
	}
#endif
#ifndef FINAL_RELEASE
	if (pBackBufferWrp)
	{
		if (!IsInternalCall())
		{
			if (pPresentData->Flags.Blt)  {
				DEBUG_TRACE3(_T("\tPresent - Blt\n"));
			} 
			if (pPresentData->Flags.Flip) {
				DEBUG_TRACE3(_T("\tPresent - Flip\n"));
			} 
			if (pPresentData->Flags.Reserved) {
				DEBUG_TRACE3(_T("\tPresent - %d\n"), pPresentData->Flags.Reserved);
			}
			if (pBackBufferWrp->m_CreateResource.pPrimaryDesc) 
			{
				if (pBackBufferWrp->m_CreateResource.pPrimaryDesc->Flags & DXGI_DDI_PRIMARY_OPTIONAL) {
					DEBUG_TRACE3(_T("\tPresent: %p pPrimaryDesc -> Optional\n"), pBackBufferWrp);
				} else {
					DEBUG_TRACE3(_T("\tPresent: %p pPrimaryDesc\n"), pBackBufferWrp);
				}
			}
			else
				DEBUG_TRACE3(_T("\tPresent: %p\n"), pBackBufferWrp);
		}
		if( DXGI_GET_WRAPPER()->m_DumpType >= dtOnlyRT)
		{
			THREAD_GUARD_DXGI();
			TCHAR szFileName[MAX_PATH];
			TCHAR resourceName[128];
			int resID = DXGI_GET_WRAPPER()->m_ResourceManager.GetID(pBackBufferWrp->GetHandle());
			_stprintf_s(szFileName, L"%s\\%.4d.SurfaceToPresent.", 
				DXGI_GET_WRAPPER()->m_DumpDirectory, DXGI_GET_WRAPPER()->m_CBCount);
			_stprintf_s(resourceName, L"%s%d-%d",  pBackBufferWrp->GetResourceName(), resID, pPresentData->SrcSubResourceIndex);
			pBackBufferWrp->DumpToFile(DXGI_GET_WRAPPER(), szFileName, resourceName, DXGI_GET_WRAPPER()->IsStereoActive(), false);
		}
	}
#endif
	DXGI_RESTORE_DEVICE(pPresentData);
	return hResult;
}
