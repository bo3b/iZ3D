#pragma once
#include "commondevice.h"
#include "DX9Font.h"
#include <d3d9.h>
#include <list>
#include <atlcomcli.h>
#include <vector>
class DX9Device : public iz3d::uilib::CommonDevice
{
	IDirect3DDevice9* m_pDevice;
	DWORD m_AlphaTestEnable;
	DWORD m_AlphaRef;
	DWORD m_SrcBlend;
	DWORD m_DestBlend;
	virtual ~DX9Device();
public:
	DX9Device( LPDIRECT3DDEVICE9 pDevice );
	virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile, ICommonTexture** ppTexture );
	virtual HRESULT CreateSprite( ICommonSprite** ppSprite );
	virtual HRESULT DeleteSprite( ICommonSprite** ppSprite );
	virtual HRESULT CreateTextureFromResource( HMODULE module, LPCTSTR pSrcResource, ICommonTexture** ppTexture );
	virtual HRESULT CreateTextureFromMemory( const void* pData, size_t s, ICommonTexture** ppTexture );
	virtual HRESULT CreateFont( const LOGFONT& Font, ICommonFont** ppFont );
	virtual HRESULT CreateLine( ICommonLine** ppLine );
	virtual HRESULT BeginScene();
	virtual HRESULT EndScene();
	virtual HRESULT SetNeededRenderState();
	virtual HRESULT ReturnRenderState();
	virtual HRESULT SetRenderTarget( void* pHandle );
	virtual SIZE GetViewportWidthHeight();
	virtual HRESULT SetCurrentSwapChain( void* pSwapChain );
	virtual void Release();
};
