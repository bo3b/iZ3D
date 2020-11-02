#pragma once

#include <atlbase.h>
#include <set>
#include <sstream>
#include <boost/unordered_map.hpp>
#include <dxgi.h>
#include "D3DDeviceWrapper.h"
#include "SwapChainResources.h"
#include "../uilib/WizardSCData.h"
//////////////////////////////////////////////////////////////////////////

enum SwapChainState
{
	scNormal,
	scCreating,
	scResizing,
	scDestroing,
	scChangeFullscreenMode,
};

struct IDXGISwapChain;
extern TLS IDXGISwapChain*				g_pCurrentSwapChain;
extern TLS SwapChainState				g_SwapChainMode;

//////////////////////////////////////////////////////////////////////////

class D3DSwapChain
{	
public:
	D3DSwapChain( D3DDeviceWrapper*	pD3DDeviceWrapper );
	~D3DSwapChain(void);

	void								InitializeSwapChains();
	void								Clear(SwapChainResources* pRes);
	void								SetDXGISwapChain(IDXGISwapChain* pSwapChain);
	void								StopPresenter();

	void								Present( UINT SyncInterval, UINT Flags );
	HRESULT								DoScaling();
	void								PresentData();
	void								WriteJPSScreenshot( bool bApplyGammaCorrection_ = false );
	void								CheckSecondWindowPosition(bool bShowWindow = true);
	void								PrepareFPSMessage( D3DDeviceWrapper* pWrapper_ );
	UINT								GetLineOffset() const { return m_nLineOffset; };

	HRESULT								SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget );
	void								ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	void								ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters);
	void								CheckSwapChainMode();
	bool								IsSwapChainsChangeMode();

	inline ShiftFinder10*				GetShiftFinder()	{ return &m_ShiftFinder;    }
	inline LaserSightData*				GetLaserSightData()	{ return &m_LaserSightData; }
#ifndef DISABLE_WIZARD
	inline uilib::WizardSCData*			GetWizardData()		{ return &m_WizardSCData; }
#endif

	D3DDeviceWrapper*					m_pD3DDeviceWrapper;
	IDXGISwapChain*						m_pSwapChain;

	DXGI_SWAP_CHAIN_DESC				m_Description;
	SIZE								m_BackBufferSize;
	SIZE								m_BackBufferSizeBeforeScaling;
	POINT								m_DstMonitorPoint;

	PrimaryResourcesMap					m_ResourcesPrimary;
	PrimaryResourcesMap::iterator		m_LatestResourcePrimary;

	bool BackBufferExist( DXGI_DDI_HRESOURCE hSurfaceToPresent ) const;
	SwapChainResources* CreateOrFindBackBufferResources( DXGI_DDI_HRESOURCE hSurfaceToPresent, DXGI_DDI_HRESOURCE hBackBufferPrimaryApp = 0 );

	inline SwapChainResources*			GetCurrentPrimaryResource()	const
	{
		if (m_LatestResourcePrimary != m_ResourcesPrimary.end())
			return &m_LatestResourcePrimary->second;
		else
			return NULL;
	}

	inline D3D10DDI_HRESOURCE			GetPresenerBackBuffer( bool isLeft ) const
	{
		SwapChainResources* pRes = GetCurrentPrimaryResource();
		if( !m_bUseSimplePresenter /*&& !m_bUseKMShutterService*/ )
		{
			// use that for USE_UM_PRESENTER and USE_ATI_PRESENTER
			return isLeft ? pRes->m_hPresenterResourceLeft : pRes->m_hPresenterResourceRight;
		}
		return pRes->GetBackBufferPrimary();
	}

	//--- rect's for stretching in DoScaling() ---
	RECT								m_SrcRect,  m_DstRect;

	DXGI_SAMPLE_DESC					m_MultiSample;

	CComPtr<IDXGISwapChain>				m_pSecondSwapChain;
	CComPtr<IDXGIFactory>				m_pDXGIFactory;

	// second window
	bool								m_bClassWasRegistered;
	HANDLE								m_hSecondWindowInitialized;
	HANDLE								m_hS3DSecondWindowThread;  
	WNDCLASS							m_S3DWindowClass;
	HWND								m_hS3DSecondWindow;
	RECT								m_S3DSecondWindowRect;
	bool								m_bS3DSecondWindowVisible;

	LARGE_INTEGER 						m_nFrameTimeDelta;
	LARGE_INTEGER 						m_nLastFrameTime;
	LARGE_INTEGER						m_nFPSDropShowTimeLeft;
	LARGE_INTEGER						m_nFPSTimeDelta;
	LARGE_INTEGER						m_nLastDropTime;
	unsigned int						m_nSessionFrameCount;
	std::wostringstream					m_szFPS;
	DOUBLE								m_fLastMonoFPS;
	DOUBLE								m_fFPS;
	bool								m_bLeftEyeActive;

	// Presenter resources
	bool								m_bUseSimplePresenter;
	volatile bool						m_bPresenterCall;
	volatile bool						m_bResourceCopyRegionCalled;
	volatile ULONG						m_bPresenterThreadId;
	UINT								m_nLineOffset; // For AQBS

	// KM shutter presenter
	bool								m_bUseKMShutterService;
	bool								m_bPresentationCall;

private:
	//--- per swapchain laser sight and auto-shift data ---
	ShiftFinder10						m_ShiftFinder;
	LaserSightData						m_LaserSightData;
#ifndef DISABLE_WIZARD
	uilib::WizardSCData					m_WizardSCData;
#endif

	void								SetPrimaryBackBuffer(D3D10DDI_HRESOURCE hResource);
	void								Initialize();
	bool								GetSecondaryWindowRect(RECT* rcWindow, RECT* rcClient);
	bool								SendOrPostMessage(WPARAM wParam);

	bool								GetOutputs(IDXGIFactory* pDXGIFactory, IDXGIOutput** pFirstOutput, IDXGIOutput** pSecondOutput);
	void								InitWindows();

	void								SetBackBufferSize();
	void								ResolveMethodResources(SwapChainResources* pRes);
	void								InitAQBSData();

	HRESULT							    ScalingNone( SwapChainResources* pRes );
	HRESULT							    ScalingGeneral( SwapChainResources* pRes );
	HRESULT							    ScalingAspectX( SwapChainResources* pRes );

};

//////////////////////////////////////////////////////////////////////////
