#include "stdafx.h"
#include "Blt.h"
#include "..\Streamer\CodeGenerator.h"
#include "D3DDeviceWrapper.h"
#include "Present.h"
#include "Presenter.h"
#include "..\D3DSwapChain.h"
#include "..\presenter.h"

namespace Commands
{

	void Blt::Execute( D3DDeviceWrapper *pWrapper )
	{
		// pWrapper->OriginalDXGIDDIBaseFunctions.pfnBlt(&BltData_);
	}

	bool Blt::WriteToFile( D3DDeviceWrapper *pWrapper ) const
	{
		WriteStreamer::CmdBegin( pWrapper->GetWrapperHandle(), ( Command* )this,  "Blt" );
		WriteStreamer::ValueByRef( "BltData", pBltData_ );
		WriteStreamer::Value( "RetValue", RetValue_ );
		WriteStreamer::CmdEnd();
		return true;
	}

	bool Blt::ReadFromFile()
	{
		D3D10DDI_HDEVICE hDevice = { NULL };
		ReadStreamer::CmdBegin( hDevice );
		ReadStreamer::ValueByRef( pBltData_ );
		ReadStreamer::Value( RetValue_ );
		ReadStreamer::CmdEnd();
		return true;
	}
	
	bool Blt::IsPresentBlt = false;
}

HRESULT ( APIENTRY  Blt )  (DXGI_DDI_ARG_BLT* pBltData)
{
	DXGI_MODIFY_DEVICE(pBltData);

	D3DSwapChain*		pSwapChain	= DXGI_GET_WRAPPER()->GetD3DSwapChain();
	_ASSERT(pSwapChain);

	if( CPresenterX::Get() && !DXGI_GET_WRAPPER()->SkipPresenterThread() && pSwapChain )
	{
		if (pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId())
		{
			DEBUG_TRACE3(_T("Presenter - Blt\n"));
			//DEBUG_MESSAGE(_T("Presenter - Blt Src %p Dst %p\n"), 
			//	pBltData->hSrcResource, pBltData->hDstResource);
			UINT frame = CPresenterX::Get()->GetFrameCount();
			bool isLeft = !( frame & 0x0001 );
			SwapChainResources* pRes = pSwapChain->GetCurrentPrimaryResource();
			if (pRes == NULL) // sometimes happens when change resolution (Dirt 2)
			{
				DEBUG_MESSAGE(_T("WARNING: currentPrimaryResource == NULL in Blt() from presenter\n"));
				return S_OK;
			}
			D3D10DDI_HRESOURCE hPresenterResource = pRes->GetPresenterResource(isLeft);
			//DEBUG_MESSAGE(_T("Presenter2 - Blt Src %p Dst %p\n"), 
			//	hPresenterResource, pBltData->hDstResource);
			if (hPresenterResource.pDrvPrivate == NULL)
			{
				DEBUG_MESSAGE(_T("WARNING: m_hPresenterResourceLeft == NULL, probably presenter bug\n"));
				return S_OK;
			}

			THREAD_GUARD_DXGI();
			D3D10DDI_HRESOURCE hDstResource;
			hDstResource.pDrvPrivate = (void*)GET_RESOURCE_HANDLE(pBltData->hDstResource);
			DXGI_GET_WRAPPER()->OriginalDeviceFuncs.pfnResourceCopyRegion( DXGI_GET_WRAPPER()->GetHandle(),
				hDstResource,
				pBltData->DstSubresource,
				pBltData->DstLeft, pBltData->DstTop, 0,
				hPresenterResource,
				0,
				NULL
				);

#ifndef FINAL_RELEASE
			if( GINFO_DUMP_ON && DXGI_GET_WRAPPER()->m_DumpType >= dtOnlyRT )
			{
				ResourceWrapper* pSrcWrapper;
				InitWrapper(pRes->GetPresenterResourceWrap(), pSrcWrapper);
				ResourceWrapper* pWrapper = (ResourceWrapper*)pBltData->hDstResource;

				TCHAR szFileName[MAX_PATH];
				_stprintf_s(szFileName, L"%s\\Frame.%.4d.%s.Blt.Src.RT", 
					DXGI_GET_WRAPPER()->m_DumpDirectory, frame, isLeft ? _T("L") : _T("R")); 
				pSrcWrapper->DumpToFile(DXGI_GET_WRAPPER(), szFileName, NULL, true, true, isLeft ? dvLeft : dvRight);

				_stprintf_s(szFileName, L"%s\\Frame.%.4d.%s.Blt.Dst.RT", 
					DXGI_GET_WRAPPER()->m_DumpDirectory, frame, isLeft ? _T("L") : _T("R")); 
				pWrapper->DumpToFile(DXGI_GET_WRAPPER(), szFileName, NULL, true, true, isLeft ? dvLeft : dvRight);
			}
#endif

			DXGI_RESTORE_DEVICE(pBltData);
			return S_OK;
		}
		else 
		{
			DEBUG_TRACE3(_T("App - Blt\n"));
		}
	}

	// Code for scaling
	// recheck this code
	// if it is Blt used to copy from scaled backbuffer to actual, when ignore it, but
	// notify next present command about provided scaled backbuffer.
	if (Commands::Blt::IsPresentBlt) 
	{
		Commands::Present::hRealSurfaceToPresent	= pBltData->hSrcResource;
		Commands::Blt::IsPresentBlt					= false;
		DXGI_RESTORE_DEVICE(pBltData);
		return S_OK;
	}

	DXGI_GET_WRAPPER()->ProcessCB();

	DXGI_DDI_HRESOURCE hDstResource = pBltData->hDstResource;
	DXGI_DDI_HRESOURCE hSrcResource = pBltData->hSrcResource;
	ResourceWrapper *pDstWrapper = (ResourceWrapper *)hDstResource;
	ResourceWrapper *pSrcWrapper = (ResourceWrapper *)hSrcResource;

#ifndef EXECUTE_IMMEDIATELY_G2
	// bypass Blt()-Stretch call, because it processed later in Present()
	// bypass Blt()-Resolve call, for multisampled mode in fullscreen
	if ((pBltData->Flags.Stretch || pBltData->Flags.Resolve) && !pBltData->Flags.Present)
	{
		THREAD_GUARD_DXGI();
		
		pBltData->hDstResource	= (DXGI_DDI_HRESOURCE)pDstWrapper->m_hRightHandle.pDrvPrivate;
		pBltData->hSrcResource	= (DXGI_DDI_HRESOURCE)pSrcWrapper->m_hRightHandle.pDrvPrivate;

		HRESULT hResult;
		Commands::Blt* command = new Commands::Blt(DXGI_DEVICE, pBltData);
		BEFORE_EXECUTE(command);
		{
			hResult = DXGI_GET_ORIG().pfnBlt(pBltData);
		}
		command->RetValue_ = hResult;
		AFTER_EXECUTE(command);
		DXGI_GET_WRAPPER()->AddCommand( command, true );

		pBltData->hDstResource	= (DXGI_DDI_HRESOURCE)pDstWrapper->GetHandle().pDrvPrivate;
		pBltData->hSrcResource	= (DXGI_DDI_HRESOURCE)pSrcWrapper->GetHandle().pDrvPrivate;

		command = new Commands::Blt(DXGI_DEVICE, pBltData);
		BEFORE_EXECUTE(command);
		{
			hResult = DXGI_GET_ORIG().pfnBlt(pBltData);
		}
		command->RetValue_ = hResult;
		AFTER_EXECUTE(command);
		DXGI_GET_WRAPPER()->AddCommand( command, true );

		if (!IsInternalCall())
		{
			if (pBltData->Flags.Convert) {
				DEBUG_TRACE3(_T("\t\tBlt - Convert (Skip DoPresent)\n"));
			}
			if (pBltData->Flags.Resolve) {
				DEBUG_TRACE3(_T("\t\tBlt - Resolve (Skip DoPresent)\n"));
			} 
			if (pBltData->Flags.Stretch) {
				DEBUG_TRACE3(_T("\t\tBlt - Stretch (Skip DoPresent)\n"));
			}
		}

#ifndef FINAL_RELEASE
		if( pSrcWrapper && GINFO_DUMP_ON && DXGI_GET_WRAPPER()->m_DumpType >= dtOnlyRT)
		{
			THREAD_GUARD_DXGI();
			TCHAR szFileName[MAX_PATH];
			TCHAR resourceName[128];
			int srcResID = DXGI_GET_WRAPPER()->m_ResourceManager.GetID(pSrcWrapper->GetHandle());
			_stprintf_s(szFileName, L"%s\\%.4d.Blt.SrcResource.", 
				DXGI_GET_WRAPPER()->m_DumpDirectory, DXGI_GET_WRAPPER()->m_CBCount);
			_stprintf_s(resourceName, L"%s%d-%d", 
				pSrcWrapper->GetResourceName(), srcResID, pBltData->SrcSubresource);
			pSrcWrapper->DumpToFile(DXGI_GET_WRAPPER(), szFileName, resourceName, DXGI_GET_WRAPPER()->IsStereoActive(), true);
			int dstResID = DXGI_GET_WRAPPER()->m_ResourceManager.GetID(pDstWrapper->GetHandle());
			DEBUG_TRACE3(L"%.4d.Blt.DstResource.%s%d-%d\n", 
				DXGI_GET_WRAPPER()->m_CBCount, pDstWrapper->GetResourceName(), dstResID, pBltData->DstSubresource);
		}
#endif
		
		DXGI_RESTORE_DEVICE(pBltData);
		return hResult;
	}

	//DEBUG_MESSAGE(_T("App - Blt Src %p Dst %p\n"), 
	//	pBltData->hSrcResource, pBltData->hDstResource);

	SwapChainResources* pRes = NULL;
	if (!IsInternalCall())
		pRes = pSwapChain->CreateOrFindBackBufferResources(pBltData->hSrcResource);
	else
	{
		pRes = pSwapChain->GetCurrentPrimaryResource();
		if (pRes == NULL)
		{
			DEBUG_MESSAGE(_T("WARNING: currentPrimaryResource == NULL in Blt()\n"));
			return S_OK;
		}
	}

	if ( CPresenterX::Get() && USE_UM_PRESENTER )
	{
		CPresenterX::Get()->InitResources( );
		CPresenterX::Get()->SetBltData( pBltData );
		CPresenterX::Get()->AddBackBuffer( pBltData->hDstResource, pBltData->DstSubresource );
		if ( CPresenterX::Get()->GetAction() == ptNone )
			CPresenterX::Get()->SetAction( ptRender );
	}

	D3D10DDI_HRESOURCE hResource = MAKE_D3D10DDI_HRESOURCE((void*)pBltData->hSrcResource);
	if (!IsInternalCall())
	{
		DXGI_GET_WRAPPER()->DoPresent();
	}
	else
	{
		DXGI_GET_WRAPPER()->UpdateSecondSwapChain(hResource);
	}

	if( CPresenterX::Get() && !DXGI_GET_WRAPPER()->SkipPresenterThread() && pSwapChain &&  
		!(pSwapChain->m_bPresenterCall && pSwapChain->m_bPresenterThreadId == GetCurrentThreadId()))
	{
		DEBUG_TRACE3(_T("App - Blt\n"));
		DXGI_RESTORE_DEVICE(pBltData);
		//DEBUG_MESSAGE(_T("App2 - Blt Src %p Dst %p\n"), 
		//	pBltData->hSrcResource, pBltData->hDstResource);
		return S_OK;
	}

	pBltData->hDstResource			= GET_RESOURCE_HANDLE( hDstResource );
	if (!IsInternalCall())
	{
		pBltData->hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->GetBackBufferPrimary().pDrvPrivate;
	}
	else
	{
		pBltData->hSrcResource	= (DXGI_DDI_HRESOURCE)pRes->GetBackBufferSecondary().pDrvPrivate;
	}

	HRESULT hResult = S_OK;
	Commands::Blt* command = new Commands::Blt(DXGI_DEVICE, pBltData);
	{
		THREAD_GUARD_DXGI();
		BEFORE_EXECUTE(command);
		hResult = DXGI_GET_ORIG().pfnBlt(pBltData);
		command->RetValue_ = hResult;
		AFTER_EXECUTE(command);
	}
	DXGI_GET_WRAPPER()->AddCommand( command, true );
#else
	HRESULT hResult;
	{
		pBltData->hDstResource = pDstWrapper ? (DXGI_DDI_HRESOURCE)pDstWrapper->GetHandle().pDrvPrivate : NULL;
		pBltData->hSrcResource = (DXGI_DDI_HRESOURCE)pSrcWrapper->GetHandle().pDrvPrivate;
		THREAD_GUARD_DXGI();
		hResult = DXGI_GET_ORIG().pfnBlt(pBltData);
	}
#endif

	if (!IsInternalCall())
	{
		if (pBltData->Flags.Convert) {
			DEBUG_TRACE3(_T("\t\tBlt - Convert\n"));
		} 
		if (pBltData->Flags.Present) {
			DEBUG_TRACE3(_T("\t\tBlt - Present\n"));
		} 
		if (pBltData->Flags.Resolve) {
			DEBUG_TRACE3(_T("\t\tBlt - Resolve\n"));
		} 
		if (pBltData->Flags.Stretch) {
			DEBUG_TRACE3(_T("\t\tBlt - Stretch\n"));
		}
		/*if (pSrcWrapper->m_CreateResource.pPrimaryDesc) 
		{
			if (pSrcWrapper->m_CreateResource.pPrimaryDesc->Flags & DXGI_DDI_PRIMARY_OPTIONAL) {
				DEBUG_TRACE1(_T("\tBlt: %p pPrimaryDesc -> Optional\n"), pSrcWrapper);
			} else {
				DEBUG_TRACE1(_T("\tBlt: %p pPrimaryDesc\n"), pSrcWrapper);
			}
		}
		else
			DEBUG_TRACE1(_T("\tBlt: %p\n"), pSrcWrapper);*/
	}
#ifndef FINAL_RELEASE
	if( pSrcWrapper && GINFO_DUMP_ON && DXGI_GET_WRAPPER()->m_DumpType >= dtOnlyRT)
	{
		THREAD_GUARD_DXGI();
		TCHAR szFileName[MAX_PATH];
		TCHAR resourceName[128];
		int srcResID = DXGI_GET_WRAPPER()->m_ResourceManager.GetID(pSrcWrapper->GetHandle());
		_stprintf_s(szFileName, L"%s\\%.4d.Blt.SrcResource.", 
			DXGI_GET_WRAPPER()->m_DumpDirectory, DXGI_GET_WRAPPER()->m_CBCount);
		_stprintf_s(resourceName, L"%s%d-%d", 
			pSrcWrapper->GetResourceName(), srcResID, pBltData->SrcSubresource);
		pSrcWrapper->DumpToFile(DXGI_GET_WRAPPER(), szFileName, resourceName, DXGI_GET_WRAPPER()->IsStereoActive(), false);
		int dstResID = DXGI_GET_WRAPPER()->m_ResourceManager.GetID(pDstWrapper->GetHandle());
		DEBUG_TRACE3(L"%.4d.Blt.DstResource.%s%d-%d\n", 
			DXGI_GET_WRAPPER()->m_CBCount, pDstWrapper->GetResourceName(), dstResID, pBltData->DstSubresource);
	}
#endif
	DXGI_RESTORE_DEVICE(pBltData);
	return hResult;
}
