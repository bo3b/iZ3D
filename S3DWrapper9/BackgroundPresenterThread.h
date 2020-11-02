//--------------------------------------------------------------------------------------
// File: BackgroundPresenterThread.h
//
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include "Presenter.h"

class	BackgroundPresenterThread;

class	BackgroundPresenterThreadSCData : public CPresenterSCData
{
public:
	BackgroundPresenterThreadSCData( BackgroundPresenterThread* pPresenter, CBaseSwapChain* pSwapChain );

	virtual void					Cleanup();

	virtual HRESULT					CreateSharedRenderTextures	();

	virtual HRESULT					UpdateSharedRenderTextures	( HANDLE* pShared );

	virtual void					RenderBackground			( );

private:
	BackgroundPresenterThread*			GetBackgroundPresenter() { return (BackgroundPresenterThread*)m_pPresenter; }
	IDirect3DSurface9*			m_pStagingSurface;

	friend BackgroundPresenterThread;
};

class	BackgroundPresenterThread: public CPresenter
{
public:
	static	BackgroundPresenterThread* CreateBackgroundPresenterThread(
		CBaseStereoRenderer*		pStereoRenderer,
		IDirect3D9Ex*					pD3D9,
		UINT						adapter,
		HWND						hFocusWindow
		);

	virtual CBasePresenterSCData*	CreateSwapChainData( CBaseSwapChain* pSwapChain );

	void	SetPresentationParameters( D3DPRESENT_PARAMETERS* params );

private:
	//--- methods ---
	virtual HRESULT						CreateD3D9Device			();
	virtual void						ResetDevice					();

	BackgroundPresenterThread( CBaseStereoRenderer* pStereoRenderer, IDirect3D9Ex* pD3D9, UINT adapter, HWND hWnd );

	IDirect3D9Ex*						m_pD3DEx;
	IDirect3DDevice9Ex*					m_pDev9Ex;

	friend BackgroundPresenterThreadSCData;
};

