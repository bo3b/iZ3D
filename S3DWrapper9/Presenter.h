//--------------------------------------------------------------------------------------
// File: Presenter.h
//
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include "BasePresenter.h"
#include "ProxySwapChain9.h"

#define USE_RENDERER_SLEEP			1
#define RENDERER_SLEEP_TIME			0

//#define	DEBUG_SHOW_WAIT_LINES		1	
#define SHARED_TEXTURE_PAIR_COUNT	3
#define SHARED_TEXTURE_COUNT		(SHARED_TEXTURE_PAIR_COUNT * 2)

class CBaseStereoRenderer;
class CBaseSwapChain;
class CPresenterSCData;

class	CPresenter : public CBasePresenter
{
public:
	virtual CBasePresenterSCData*	CreateSwapChainData( CBaseSwapChain* pSwapChain ) = 0;
	virtual void	SetPresentationParameters( D3DPRESENT_PARAMETERS* params ) = 0;

	virtual ~CPresenter();

	HRESULT					WaitD3DCreation		()
	{	
		while( m_hrReset == 0xFFFFFFFF )
			Sleep( PRESENTER_SLEEP_TIME );
		return m_hrReset;
	}

	void					GetDirect3DDevice9			( IDirect3DDevice9 **ppDevice )
	{
		*ppDevice = m_pDev9;
		(*ppDevice)->AddRef();
	}

	IDirect3DDevice9*					m_pDev9;
	D3DPRESENT_PARAMETERS				m_D3DPresentParameters;

protected:

	//--- methods ---
	virtual HRESULT			CreateD3D9Device			() = 0; 
	virtual	void			ResetDevice					() = 0;
	void					TestCooperativeLevel		();

	void					Initialize();
	D3DDISPLAYMODEEX*		PrepareDisplayMode( D3DDISPLAYMODEEX &DisplayMode );
	virtual DWORD WINAPI	BackgroundThreadProc		();

	void					DestroyFullscreenWindow		();
	HRESULT					CreatePresenterWindow		();
	HRESULT					D3DWindowedWaitForVsync ();

	void					CleanupBackground			()
	{
		DestroyFullscreenWindow();
		SAFE_RELEASE( m_pDev9 );
	}

	CPresenter( CBaseStereoRenderer* pStereoRenderer, IDirect3D9* pD3D9, UINT adapter, HWND hWnd );
	//--- data members ---
	static size_t						m_NumCopies;

	IDirect3D9*							m_pD3D;
	HWND								m_hWnd;
	HWND								m_hPresenterWnd;
	UINT								m_Adapter;
	bool								m_bReadScanLineSupported;

	friend CPresenterSCData;
};

class	CPresenterSCData : public CBasePresenterSCData
{
public:
	CPresenterSCData( CPresenter* pPresenter, 
		CBaseSwapChain* pSwapChain );
	virtual ~CPresenterSCData();

	virtual void	Initialize();

	virtual void	Cleanup();

	void	DetachSwapChain() { m_pSwapChain->Release(); }

	void	SetPresentPairIndex( long  ind );

	DWORD	GetPresentPairIndex( );

	DWORD	GetRenderingPairIndex( );

	virtual HRESULT			CreateSharedRenderTextures	() { return S_OK; }

	HANDLE					GetSharedTextureHandle		( UINT	 indx = 0 )
	{
		_ASSERT( indx<4	);
		return	m_SharedHandle[ indx ];
	}

	virtual HRESULT			UpdateSharedRenderTextures	( HANDLE* pShared ) = 0;

	virtual HRESULT			WaitForVBlank				( );

	IDirect3DSurface9*					GetViewRT(bool bLeft);

	CComPtr<IDirect3DSurface9>			m_pPrimaryBackBuffer;
	ProxySwapChain9*					m_pSwapChain;

protected:
	void								CalculateFPS();
	HRESULT								CallPresent(bool bExMode = false);
	CPresenter*							m_pPresenter;
	CriticalSection						m_CS;
	HANDLE								m_SharedHandle		[SHARED_TEXTURE_COUNT];		// 2 sets of image pairs (left + right)
	IDirect3DSurface9*					m_pViewSurface		[SHARED_TEXTURE_COUNT];
	IDirect3DTexture9*					m_pSharedTexture	[SHARED_TEXTURE_COUNT];
	IDirect3DSurface9*					m_pSharedSurface	[SHARED_TEXTURE_COUNT];
	long volatile						m_PresentedPairIndex;
	long volatile						m_RenderingPairIndex;
	LARGE_INTEGER						m_FrameTime;
	LARGE_INTEGER						m_LastFrameTime;
	ULONG								m_FrameCount;
	ULONG								m_VSyncCount;
	LARGE_INTEGER						m_RenderingTime;

	friend CPresenter;
};

inline IDirect3DSurface9* CPresenterSCData::GetViewRT(bool bLeft)
{
	return m_pViewSurface[ 2 * m_PresentedPairIndex + (bLeft ? 0 : 1) ];
}
