#include "StdAfx.h"
#include "DXGISwapChainWrapper.h"
#include "D3DDeviceWrapper.h"
#include <madCHook.h>
#include "../OutputMethods/OutputLib/OutputData.h"
#include "../S3DAPI/ScalingAgent.h"
#include "../S3DAPI/ShutterAPI.h"
#include "DXGIFactoryWrapper.h"
#include "presenter.h"

void UnhookCreateD3DDevice();

HRESULT (STDMETHODCALLTYPE *Original_CreateSwapChain)( IDXGIFactory* This, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain );

void FindNativeFullScreenResolution()
{
	if(gInfo.DisplayNativeWidth == 0 || gInfo.DisplayNativeHeight == 0)
	{
		DEVMODE mode;
		EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &mode);	// FIXME! May return wrong data if the monitor was replaced with the different one!
		gInfo.DisplayNativeWidth = mode.dmPelsWidth;
		gInfo.DisplayNativeHeight = mode.dmPelsHeight;
		gInfo.DisplayNativeFrequency.Numerator = mode.dmDisplayFrequency;
		gInfo.DisplayNativeFrequency.Denominator = 1;
	}
}

bool InitAQBS( IDXGIDevice* pDXGIDevice )
{	
	bool bResult = false;

	if( !D3DDeviceWrapper::IsAmdStereoInited( pDXGIDevice ) )
	{
		D3DDeviceWrapper::CloseAMDStereoInterface();

		CComQIPtr<ID3D11Device>	pD3D11Device = pDXGIDevice;		

		if ( pD3D11Device )
			bResult =  D3DDeviceWrapper::OpenAMDStereoInterface( pDXGIDevice, pD3D11Device );
		else 
		{
			CComQIPtr<ID3D10Device>	pD3D10Device = pDXGIDevice;
			if (pD3D10Device)
				bResult = D3DDeviceWrapper::OpenAMDStereoInterface( pDXGIDevice, pD3D10Device );
		}
	}
	else
	{
		bResult = true;
	}

	return bResult;
}

STDMETHODIMP CreateSwapChain( IDXGIFactory* This, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain )
{
	DEBUG_TRACE1(_T("CreateSwapChain(): ENTER\n"));
	_ASSERT( pDesc );
	
	HRESULT hResult;
	DXGI_SWAP_CHAIN_DESC *pOriginalSCDesc = pDesc;	// Keep pointer to the original (we'll provide that for GetDesc())

	if (IsInternalCall())
	{
		DEBUG_MESSAGE(_T("Creating swap chain(internal)"));
		g_SwapChainMode = scCreating;
		NSCALL_TRACE1(Original_CreateSwapChain(This, pDevice, pDesc, ppSwapChain), 
			DEBUG_MESSAGE(_T("IDXGIFactory::CreateSwapChain(pDevice = %p, pDesc = (%s), ppSwapChain = %p)"), 
			pDevice, GetDXGISwapChainDescString(pDesc), ppSwapChain));
		g_SwapChainMode = scNormal;
		return hResult;
	}

	CComQIPtr<IDXGIDevice>	pDXGIDevice		= pDevice;
	DXGI_SWAP_CHAIN_DESC	SCDesc			= *pDesc; // Better to keep local copy and don't mess application variable
	CComQIPtr<IDXGIDevice1>	pDXGIDevice1	= pDevice;
	
	DEBUG_MESSAGE(_T("Creating swap chain"));
	if( USE_IZ3DKMSERVICE_PRESENTER )
	{
		// Check for KM shutter service installed
		if( TRUE != g_KMShutter.InitShutter() )
		{
			// Force UM presenter
			gData.ShutterMode = SHUTTER_MODE_UMHOOK_D3D10;
		}
		else
		{
			// Reduce frames-ahead count
			if( pDXGIDevice1 )
				pDXGIDevice1->SetMaximumFrameLatency( 1 );
		}
	}

	if( USE_ATI_PRESENTER )
	{
		bool bResult = InitAQBS(pDXGIDevice);
		if( !bResult )
		{
			if( gInfo.ShutterMode == SHUTTER_MODE_AUTO ) // switch to KM service or UM presenter, if auto mode selected  
			{
				D3DDeviceWrapper::ResetAQBSStates();
				gData.ShutterMode = SHUTTER_MODE_UMHOOK_D3D10;
			}
			else
				gData.ShutterMode = SHUTTER_MODE_DISABLED;
		}
	}

	g_SwapChainMode = scCreating;
	BOOL Fullscreen = !SCDesc.Windowed;
	FindNativeFullScreenResolution();
	SIZE BackBufferSize = {gInfo.DisplayNativeWidth, gInfo.DisplayNativeHeight};
	SIZE BackBufferSizeBeforeScaling = {SCDesc.BufferDesc.Width, SCDesc.BufferDesc.Height};

	ScalingHookPtrT ScalingHook;
	if (Fullscreen) // force for all outputs, because we don't know is current output use two windows
	{
		SCDesc.Windowed = true;

		// create swap chain in native resolution to perform scaling, after resize it and setup hooks
		if (gInfo.DisplayScalingMode != 0)
		{
			// Need to lie application about actual resolution while swap chain is creating
			ScalingHook = ScalingAgent::Instance()->InitializeHook( TDisplayScalingMode(gInfo.DisplayScalingMode), 
																	BackBufferSizeBeforeScaling, 
																	BackBufferSize );

			SCDesc.BufferDesc.Width = gInfo.DisplayNativeWidth;
			SCDesc.BufferDesc.Height = gInfo.DisplayNativeHeight;
			SCDesc.BufferDesc.RefreshRate = gInfo.DisplayNativeFrequency;
		}
	}

	if (gInfo.DisplayScalingMode != 0)
	{
		// disable flags which may distort or break scaling
		SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		//SCDesc.Flags &= !DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		//ScalingHook::IgnoreModeSwitch(true);
	}

	if( IS_SHUTTER_OUTPUT )
	{
		DEBUG_TRACE1(_T("FindShutterClosestMatchingMode <source>: ( pDesc = (%s) )\n"), GetDXGISwapChainDescString(&SCDesc) );
		D3DDeviceWrapper::FindShutterClosestMatchingMode( pDXGIDevice, &SCDesc.BufferDesc );
		DEBUG_TRACE1(_T("FindShutterClosestMatchingMode <modified>: ( pDesc = (%s) )\n"), GetDXGISwapChainDescString(&SCDesc) );

		if( USE_IZ3DKMSERVICE_PRESENTER )
		{
			// Force quadbuffering for KM shutter service (TODO: discuss that)
			if( !pDesc->Windowed )
			{
				// FIXME!
				SCDesc.BufferCount	= 2; // KM_SHUTTER_SERVICE_BUFFER_COUNT;
				SCDesc.SwapEffect	= DXGI_SWAP_EFFECT_SEQUENTIAL;
				SCDesc.Windowed		= pDesc->Windowed;					// don't force windowed mode
			}
			// N.B. Multisampling will not work with DXGI_SWAP_EFFECT_SEQUENTIAL feature
			_ASSERT( SCDesc.SampleDesc.Count	== 1 );
			_ASSERT( SCDesc.SampleDesc.Quality	== 0 );
		}

		if( USE_UM_PRESENTER )
		{
			CPresenterX::Create( SCDesc.BufferDesc.RefreshRate );
			//SCDesc.OutputWindow = CPresenterX::Get()->GetPresenterWindow();
		}
	}

	NSCALL_TRACE1(Original_CreateSwapChain(This, pDevice, &SCDesc, ppSwapChain), 
		DEBUG_MESSAGE(_T("IDXGIFactory::CreateSwapChain(pDevice = %p, pDesc = (%s), ppSwapChain = %p)"), 
		pDevice, GetDXGISwapChainDescString(&SCDesc), ppSwapChain));

	if( Fullscreen )
		SCDesc.Windowed = false;

	if( g_pLastD3DDevice && SUCCEEDED(hResult) )
	{
		IDXGISwapChain *pRealSwapChain = *ppSwapChain;
		D3DSwapChain* p = NULL;
		g_pLastD3DDevice->InitializeDXGIDevice( pDXGIDevice );
		p = g_pLastD3DDevice->MapDXGISwapChain(This, pRealSwapChain);	
		p->m_BackBufferSizeBeforeScaling = BackBufferSize;

		if( USE_IZ3DKMSERVICE_PRESENTER )
		{
			p->m_bUseSimplePresenter	= false;			// turn off simple presentation if KM shutter is available
			p->m_bUseKMShutterService	= true;
		}

		CComPtr<IWrapper> pWrapper;
		NSCALL(DXGISwapChainWrapper::CreateInstance(&pWrapper));
		if(pWrapper)
		{
			NSCALL(pWrapper->Init((IUnknown*)pRealSwapChain));
			(*(IDXGISwapChain**)ppSwapChain)->Release();
			IDXGISwapChain *pSwapChain = NULL;
			pWrapper.QueryInterface(&pSwapChain);
			*ppSwapChain = pSwapChain;
			DXGISwapChainWrapper* pSwapChainWrapper = static_cast<DXGISwapChainWrapper*>(pWrapper.p);
			pSwapChainWrapper->setD3DSwapChain(pDXGIDevice, p);
			pSwapChainWrapper->setOriginalSwapChainDesc( pOriginalSCDesc ); 
			DeviceModes deviceMode = p ? p->m_pD3DDeviceWrapper->m_DeviceMode : (DeviceModes)gInfo.DeviceMode;
			if (deviceMode == DEVICE_MODE_FORCEFULLSCREEN)
				Fullscreen = true;
			else if (deviceMode == DEVICE_MODE_FORCEWINDOWED)
				Fullscreen = false;

			if (CPresenterX::Get())
			{
				SetCurrentSwapChain setSwapChain(pRealSwapChain);
				CPresenterX::Get()->Init(g_pLastD3DDevice);
			}
			if (Fullscreen) // FIXME! What about KM Shutter + Scaling enabled?
			{
				g_SwapChainMode = scNormal;
				if( !USE_IZ3DKMSERVICE_PRESENTER )		// SwapChain is fullscreen already for KM Shutter
				{
					if (deviceMode == DEVICE_MODE_FORCEFULLSCREEN)
						pSwapChainWrapper->ResizeBuffers(SCDesc.BufferCount, gInfo.DisplayNativeWidth, gInfo.DisplayNativeHeight, SCDesc.BufferDesc.Format, 0); 

					pSwapChain->SetFullscreenState( TRUE, NULL );
				}
				// restore requested resolution
				if (gInfo.DisplayScalingMode != 0 && (BackBufferSizeBeforeScaling.cx != gInfo.DisplayNativeWidth || BackBufferSizeBeforeScaling.cy != gInfo.DisplayNativeHeight))
				{
					SCDesc.BufferDesc.Width  = BackBufferSizeBeforeScaling.cx;
					SCDesc.BufferDesc.Height = BackBufferSizeBeforeScaling.cy;
					pSwapChainWrapper->ResizeBuffers(SCDesc.BufferCount, SCDesc.BufferDesc.Width, SCDesc.BufferDesc.Height, SCDesc.BufferDesc.Format, 0); 
					pSwapChainWrapper->ResizeTarget(&SCDesc.BufferDesc);
				}
				if (CPresenterX::Get())
				{
					DXGI_SWAP_CHAIN_DESC	desc;
					pSwapChain->GetDesc(&desc);
					CPresenterX::Get()->ReInit( desc.BufferDesc.RefreshRate );
				}
			}
		}
		g_pLastD3DDevice = NULL;
	}	
	ScalingHook.reset();
	g_SwapChainMode = scNormal;
	
	DEBUG_TRACE1(_T("CreateSwapChain(): EXIT\n"));
	return hResult;
}

STDMETHODIMP Hooked_CreateDXGIFactory(REFIID riid, void **ppFactory, HMODULE hCallingModule)
{
	if (IsInternalCall() || gInfo.UseMonoDeviceWrapper)
		return CreateDXGIFactory(riid, ppFactory);

#ifndef FINAL_RELEASE
	if (hCallingModule)
	{
		TCHAR moduleName[MAX_PATH];
		GetModuleFileName(hCallingModule, moduleName, sizeof(moduleName));
		DEBUG_MESSAGE(_T("Calling Module: %s\n"), moduleName);
	}
#endif

	HRESULT hResult = CreateDXGIFactory(riid, ppFactory);
	if (SUCCEEDED(hResult) && riid == __uuidof(IDXGIFactory))
	{
		void** p = *(void***)(*ppFactory);
		HookCode(p[10], CreateSwapChain, (PVOID*)&Original_CreateSwapChain, HOOKING_FLAG);
	}	
	return hResult;
}

typedef HRESULT (STDMETHODCALLTYPE *PFNCREATEDXGIFACTORY)(REFIID riid, void **ppFactory);

STDMETHODIMP Hooked_CreateDXGIFactory1(REFIID riid, void **ppFactory, HMODULE hCallingModule)
{
	HMODULE hDXGI = GetModuleHandle(_T("dxgi.dll"));
	PFNCREATEDXGIFACTORY p = (PFNCREATEDXGIFACTORY)GetProcAddress(hDXGI, "CreateDXGIFactory1");
	
	if (IsInternalCall() || gInfo.UseMonoDeviceWrapper)
		return p(riid, ppFactory);

#ifndef FINAL_RELEASE
	if (hCallingModule)
	{
		TCHAR moduleName[MAX_PATH];
		GetModuleFileName(hCallingModule, moduleName, sizeof(moduleName));
		DEBUG_MESSAGE(_T("Calling Module: %s\n"), moduleName);
	}
#endif

	HRESULT hResult = p(riid, ppFactory);	
	if (SUCCEEDED(hResult) && riid == __uuidof(IDXGIFactory1))
	{
		void** p = *(void***)(*ppFactory);
		HookCode(p[10], CreateSwapChain, (PVOID*)&Original_CreateSwapChain, HOOKING_FLAG);
	}
	return hResult;
}