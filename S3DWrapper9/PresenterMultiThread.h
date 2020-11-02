//--------------------------------------------------------------------------------------
// File: PresenterMultiThread.h
//
//
//
//--------------------------------------------------------------------------------------

#pragma once

#include "Presenter.h"

class	PresenterMultiThread;

class	PresenterMultiThreadSCData : public CPresenterSCData
{
public:
	PresenterMultiThreadSCData( PresenterMultiThread* pPresenter, CBaseSwapChain* pSwapChain );

	virtual HRESULT					UpdateSharedRenderTextures	( HANDLE* pShared );

	virtual void					RenderBackground			( );

private:
	PresenterMultiThread*			GetMultiThreadPresenter() { return 	(PresenterMultiThread*)m_pPresenter; }

	friend PresenterMultiThread;
};


class	PresenterMultiThread: public CPresenter
{
public:
	static	PresenterMultiThread* CreatePresenterMultiThread(
		CBaseStereoRenderer*		pStereoRenderer,
		IDirect3D9*					pD3D9,
		UINT						adapter,
		HWND						hFocusWindow
		);

	virtual CBasePresenterSCData*	CreateSwapChainData( CBaseSwapChain* pSwapChain );

	virtual void	SetPresentationParameters( D3DPRESENT_PARAMETERS* params );

private:
	//--- methods ---
	virtual HRESULT			CreateD3D9Device			();
	virtual void			ResetDevice					();

	PresenterMultiThread( CBaseStereoRenderer* pStereoRenderer, IDirect3D9* pD3D9, UINT adapter, HWND hWnd );

	friend PresenterMultiThreadSCData;
};

