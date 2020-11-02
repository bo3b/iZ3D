/* 
* Project : iZ3D Stereo Driver
* Copyright (C) iZ3D Inc. 2002 - 2010
*/

#pragma once

#include <boost/shared_ptr.hpp>
#include <sstream>
#include "VertexType.h"
#include "MonoSwapChain.h"
#include "DX9AutoShift.h"
#include "DX9LaserSight.h"
#include "..\UILIB\WizardSCData.h"
#include "Presenter.h"

class CBaseStereoRenderer;

class ScalingHook;
class MouseHook;
typedef boost::shared_ptr<ScalingHook>	ScalingHookPtrT;
typedef boost::shared_ptr<MouseHook>	MouseHookPtrT;

class CBaseSwapChain : public CMonoSwapChain
{
public:
	CBaseStereoRenderer*		GetBaseDevice() { return (CBaseStereoRenderer*)m_pD3D9Device; };

	//--- RouterType = ROUTER_TYPE_HOOK case functions ----
	void						HookIDirect3DSwapChain9All();
	void						UnHookIDirect3DSwapChain9All();

	D3DPRESENT_PARAMETERS       m_PresentationParameters[2];
	D3DDISPLAYMODEEX			m_FullscreenDisplayMode[2];
	SIZE						m_BackBufferSizeBeforeScaling;
	SIZE						m_BackBufferSize;

	D3DMULTISAMPLE_TYPE         m_MultiSampleType;
	DWORD                       m_MultiSampleQuality;
	HWND						m_hDestWindowOverride;

	CComPtr<IDirect3DSwapChain9>m_pAdditionalSwapChain;
	UINT						m_iAdditionalSwapChain;

	CComPtr<IDirect3DSurface9>	m_pPrimaryBackBuffer;		// Saved Original BackBuffer
	CComPtr<IDirect3DSurface9>	m_pSecondaryBackBuffer;		// Saved Original	
	CComPtr<IDirect3DSurface9>	m_pPrimaryDepthStencil;		// Saved Original DepthStencil
	CComPtr<IDirect3DSurface9>	m_pSecondaryDepthStencil;	// Saved Original

	CComPtr<IDirect3DSurface9>	m_pWidePresenterSurface;

	bool						m_bDepthBufferFounded;
	DWORD						m_PrevDrawCountAfterClearDepthBuffer;
	DWORD						m_DrawCountAfterClearDepthBuffer;
	CComPtr<IDirect3DTexture9>	m_pLeftDepthStencilTexture;
	CComPtr<IDirect3DTexture9>	m_pRightDepthStencilTexture;
	CComPtr<IDirect3DSurface9>	m_pMainDepthStencilSurface;
	CComPtr<IDirect3DTexture9>	m_pLeftDepthStencilCorrectTexture;
	CComPtr<IDirect3DTexture9>	m_pRightDepthStencilCorrectTexture;

	bool						m_bLockableBackBuffer;
	bool						m_bScalingEnabled;
	IDirect3DSurface9*			m_pLeftBackBufferBeforeScaling;
	IDirect3DSurface9*			m_pRightBackBufferBeforeScaling;
	IDirect3DSurface9*			m_pLeftBackBuffer;
	IDirect3DSurface9*			m_pRightBackBuffer;
	SIZE                        m_BBOffsetBeforeScaling;
	SIZE                        m_BBOffset;

	CComPtr<IDirect3DTexture9>	m_pLeftMethodTexture;
	CComPtr<IDirect3DTexture9>	m_pRightMethodTexture;
	RECT						m_LeftViewRect;					// left  image coordinates on image render target 
	RECT						m_RightViewRect;				// right image coordinates on image render target 
	RECT						m_LeftViewRectBeforeScaling;	// left  image coordinates on image render target before scaling
	RECT						m_RightViewRectBeforeScaling;	// right image coordinates on image render target before scaling
	//--- rect's for stretching in DoScaling() ---
	RECT						m_SrcLeft,  m_DstLeft;
	RECT						m_SrcRight, m_DstRight;

	//--- gamma correction ---
	bool                        m_CurrentRAMPisIdentity;
	D3DGAMMARAMP                m_IdentityRAMP;
	D3DGAMMARAMP                m_SavedGammaRamp;			// always keep actual ramp for GammaTexture restoring
	D3DGAMMARAMP                m_CurrentRAMP;				// RAMP for CBaseSwapChain::GetGammaRamp() function 
	CComPtr<IDirect3DTexture9>	m_GammaRAMPTexture;
	HRESULT UpdateGammaRampTexture(CONST D3DGAMMARAMP *pRamp);
	void SetGammaRamp(CONST D3DGAMMARAMP* pRamp, bool bUpdateCurrentGamma);
	void GetGammaRamp(D3DGAMMARAMP* pRamp);

	// output
	D3DTVERTEX_2T               m_Vertex[4 * 2];
	D3DTVERTEX_2T*				m_VertexSinglePass1Pass;
	D3DTVERTEX_2T*				m_VertexSinglePass2Pass;

	bool						m_bUseSwapChains;
	CONST RECT *				m_pSourceRect;
	CONST RECT *				m_pDestRect;
	CONST RGNDATA *				m_pDirtyRegion;
	DWORD						m_dwFlags;
	POINT						m_DstMonitorPoint;
	void SetPresentParams( CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, 
		CONST RGNDATA * pDirtyRegion, DWORD dwFlags = 0 );

	// presenter resources
	HANDLE						m_SharedHandle	[SHARED_TEXTURE_COUNT];
	IDirect3DTexture9*			m_pSharedTexture[SHARED_TEXTURE_COUNT];
	IDirect3DSurface9*			m_pStagingSurface[SHARED_TEXTURE_COUNT];
	CPresenterSCData*			m_pPresenterData;
	CBasePresenterSCData*		m_pBasePresenterData;

	ULONG						m_nPresentCounter;

	//--- statistics variables ---
	LARGE_INTEGER 				m_nScreensLagTime;
	LARGE_INTEGER 				m_nFrameTimeDelta;	
	LARGE_INTEGER 				m_nLastFrameTime;			//--- time when previous frame was completed ---

	//--- draw FPS related variables ---
	std::wostringstream			m_szFPS;
	DOUBLE						m_fFPS;
	LARGE_INTEGER				m_nFPSTimeDelta;
	LARGE_INTEGER				m_nLastDropTime;
	DOUBLE					    m_fLastMonoFPS;
	LARGE_INTEGER				m_nFPSDropShowTimeLeft;
	ULONG						m_nSessionFrameCount;
	float						m_fMinDepthValue;
	float						m_fMaxDepthValue;
	std::vector<float>			m_MiniHistogram;

	int							m_CurrentGap; // side-by-side

	ShiftFinder9				m_ShiftFinder;
	LaserSightData				m_LaserSightData;
#ifndef DISABLE_WIZARD
	uilib::WizardSCData			m_WizardData;
#endif

	CBaseSwapChain(CMonoRenderer* pDirect3DDevice9, UINT index);
	~CBaseSwapChain(void);
	HRESULT			InitWindows();
	HRESULT			ModifyPresentParameters();
	virtual HRESULT Initialize(IDirect3DSwapChain9* pSwapChain);
	virtual HRESULT InitializeMode(D3DSURFACE_DESC &RenderTargetDesc) = 0;
	HRESULT			PostInitialize();
	virtual void	Clear();
	void			ReleaseSC();

	virtual void	CheckDepthBuffer() {}
	void			ModifyParamsByOutput();
	BOOL			SendOrPostMessage(WPARAM wParam);
	void			CheckSecondWindowPosition();
	void			GetSecondaryWindowRect(RECT* rcWindow, RECT* rcClient);
	void			RestoreDeviceMode( );
	HMONITOR		GetMonitorHandle(int n);

	void			SendImageToPresenter();
	HRESULT			CreateSharedResources();
	void			ClearSharedTextures();

	HRESULT			DoScaling();
	HRESULT			Compose();
	HRESULT			CallPresent();
	HRESULT			PresentData();
	void			PrepareFPSMessage();
	void			ProcessDepthBuffer();

	bool			IsStereoActive();
	bool			IsSwapEyes();
	void			DrawOSD( float fFrameTimeDelta );
	void			WriteJPSScreenshot(bool applyGammaCorrection = false);

	IDirect3DSurface9* GetLeftBackBufferRT();
	IDirect3DTexture9* GetLeftBackBufferTex();
	IDirect3DTexture9* GetLeftDepthStencilTex();
	RECT* GetLeftBackBufferRect();
	IDirect3DSurface9* GetRightBackBufferRT();
	IDirect3DTexture9* GetRightBackBufferTex();
	IDirect3DTexture9* GetRightDepthStencilTex();
	RECT* GetRightBackBufferRect();

	IDirect3DSurface9*	GetViewRT(bool bLeft);
	RECT*				GetViewRect(bool bLeft);
	IDirect3DSurface9*	GetPresenterBackBuffer();

	STDMETHOD(Present)(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags);
	STDMETHOD(GetBackBuffer)(UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	STDMETHOD(GetDisplayMode)(D3DDISPLAYMODE* pMode);
	STDMETHOD(GetDisplayModeEx)(D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);

	friend LRESULT CALLBACK S3DWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

protected:
	SIZE						m_DevScreen1;
	SIZE						m_DevScreen2;
	bool						m_bWorkInWindow;	// for DeviceMode == DEVICE_MODE_FORCEWINDOWED
	ScalingHookPtrT				m_ScalingHook;
	MouseHookPtrT				m_MouseHook;
	HRESULT						m_hPreviousPresentResult;

	// second window
	bool						m_bClassWasRegistered;
	WNDCLASS					m_S3DWindowClass;
	HWND						m_hS3DSecondWindow;
	RECT						m_S3DSecondWindowRect;
	bool						m_bS3DSecondWindowVisible;

	AsyncButton					m_StopPresentButton;
	AsyncButton					m_StopRenderButton;

	ProxyDevice9&				GetD3D9Device();

	int WINAPI D3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName );
	int WINAPI D3DPERF_EndEvent( void );
};
